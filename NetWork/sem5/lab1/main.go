package main

import (
	"crypto/rand"
	"encoding/hex"
	"fmt"
	"net"
	"os"
	"sort"
	"strings"
	"time"
)

const (
	port     = 9999
	interval = 1 * time.Second
	ttl      = 5 * time.Second
)

type peer struct {
	ip   string
	last time.Time
}

type beat struct {
	id string
	ip string
	t  time.Time
}

// go run . ff12::1234
//go run . 239.255.0.1

func main() {
	network, g := parseGroupArg(os.Args)

	rcv, snd := mustOpenSockets(network, g)
	defer rcv.Close()
	defer snd.Close()

	selfID := newID()
	msg := []byte(selfID)
	beats := make(chan beat)
	go recv(rcv, beats)
	go send(snd, msg)

	store := map[string]peer{} // id -> {ip,last}
	var lastPrinted string
	tick := time.NewTicker(interval)
	defer tick.Stop()

	for {
		select {
		case b := <-beats:
			if b.id != selfID {
				store[b.id] = peer{ip: b.ip, last: b.t}
				lastPrinted = printIfChanged(store, lastPrinted)
			}
		case <-tick.C:
			now := time.Now()
			changed := false
			for id, p := range store {
				if now.Sub(p.last) > ttl {
					delete(store, id)
					changed = true
				}
			}
			if changed {
				lastPrinted = printIfChanged(store, lastPrinted)
			}
		}
	}
}

func parseGroupArg(args []string) (string, *net.UDPAddr) {
	if len(args) < 2 {
		fmt.Fprintf(os.Stderr, "usage: %s <multicast-group>\n", args[0])
		os.Exit(2)
	}
	groupIP := net.ParseIP(args[1])
	if groupIP == nil || !groupIP.IsMulticast() {
		fmt.Fprintln(os.Stderr, "invalid multicast address")
		os.Exit(2)
	}
	network := "udp6"
	if groupIP.To4() != nil {
		network = "udp4"
	}
	return network, &net.UDPAddr{IP: groupIP, Port: port}
}

func mustOpenSockets(network string, g *net.UDPAddr) (*net.UDPConn, *net.UDPConn) {
	rcv, err := net.ListenMulticastUDP(network, nil, g)
	if err != nil {
		fmt.Fprintln(os.Stderr, "listen error:", err)
		os.Exit(1)
	}
	dst := *g
	if network == "udp6" {
		if z := pickIPv6Zone(); z != "" {
			dst.Zone = z
		}
	}
	snd, err := net.DialUDP(network, nil, &dst)
	if err != nil {
		fmt.Fprintln(os.Stderr, "sender error:", err)
		os.Exit(1)
	}
	return rcv, snd
}

func send(c *net.UDPConn, msg []byte) {
	_, _ = c.Write(msg)
	go func() {
		t := time.NewTicker(interval)
		defer t.Stop()
		for range t.C {
			_, _ = c.Write(msg)
		}
	}()
}

func recv(c *net.UDPConn, out chan<- beat) {
	buf := make([]byte, 1500)
	go func() {
		for {
			_ = c.SetReadDeadline(time.Now().Add(2 * time.Second))
			n, addr, err := c.ReadFromUDP(buf)
			if err != nil {
				if ne, ok := err.(net.Error); ok && ne.Timeout() {
					continue
				}
				time.Sleep(100 * time.Millisecond)
				continue
			}
			if id, ok := parseID(buf[:n]); ok {
				out <- beat{id: id, ip: addr.IP.String(), t: time.Now()}
			}
		}
	}()
}

func printIfChanged(store map[string]peer, prev string) string {
	now := time.Now()
	var tokens []string
	for id, p := range store {
		if now.Sub(p.last) <= ttl {
			short := id
			if len(short) > 6 {
				short = short[:6]
			}
			tokens = append(tokens, fmt.Sprintf("%s@%s", short, p.ip))
		}
	}
	sort.Strings(tokens)
	cur := strings.Join(tokens, " ")
	if cur != prev {
		fmt.Println(cur)
		return cur
	}
	return prev
}

func parseID(b []byte) (string, bool) {
	s := strings.TrimSpace(string(b))
	if len(s) != 32 {
		return "", false
	}
	for _, c := range s {
		switch {
		case c >= '0' && c <= '9', c >= 'a' && c <= 'f', c >= 'A' && c <= 'F':
		default:
			return "", false
		}
	}
	return strings.ToLower(s), true
}

func newID() string {
	var b [16]byte
	_, _ = rand.Read(b[:])
	return hex.EncodeToString(b[:])
}

func pickIPv6Zone() string {
	ifaces, _ := net.Interfaces()
	for _, ifi := range ifaces {
		if ifi.Flags&net.FlagUp == 0 || ifi.Flags&net.FlagMulticast == 0 {
			continue
		}
		addrs, _ := ifi.Addrs()
		for _, a := range addrs {
			switch v := a.(type) {
			case *net.IPNet:
				if v.IP != nil && v.IP.To4() == nil {
					return ifi.Name
				}
			case *net.IPAddr:
				if v.IP != nil && v.IP.To4() == nil {
					return ifi.Name
				}
			}
		}
	}
	return ""
}
