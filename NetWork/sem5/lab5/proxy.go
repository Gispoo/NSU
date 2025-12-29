package main

import (
	"encoding/binary"
	"errors"
	"fmt"
	"io"
	"log"
	"net"
	"os"
	"strconv"
	"sync"
)

const (
	VERSION_SOCKS5  = 0x05
	METHOD_NO_AUTH  = 0x00
	CMD_CONNECT     = 0x01 // Ожидаемая команда
	REQUEST_FAILURE = 0x01 // Код ошибки при обработке запроса на подключение
	REQUEST_GRANTED = 0x00 // Код успеха при обработке запроса на подключение
)

func findMethod(methods []byte, method byte) bool {
	for _, m := range methods {
		if m == method {
			return true
		}
	}
	return false
}

/*
VER (1) | NAUTH (1)      | AUTH (var)
версия  | кол-во методов | поддерживаемые методы
*/
func handleGreeting(conn net.Conn) error {
	buffer := make([]byte, 2)
	n, err := conn.Read(buffer)
	if err != nil {
		return err
	}
	if n < 2 {
		return errors.New("expected 2 bytes (VER, NAUTH)")
	}
	if buffer[0] != VERSION_SOCKS5 {
		return errors.New("unsupported SOCKS version")
	}

	nauth := int(buffer[1])
	methods := make([]byte, nauth)
	n, err = conn.Read(methods)
	if err != nil {
		return err
	}
	if n < nauth {
		return errors.New("expected " + strconv.Itoa(nauth) + " bytes (AUTH)")
	}
	if !findMethod(methods, METHOD_NO_AUTH) {
		return errors.New("no authentication method not supported")
	}

	response := []byte{VERSION_SOCKS5, METHOD_NO_AUTH}
	if _, err := conn.Write(response); err != nil {
		return fmt.Errorf("error when sending a response to client greeting: %w", err)
	}
	return nil
}

func resolveDomain(domain string) (string, error) {
	ips, err := net.LookupIP(domain)
	if err != nil {
		return "", fmt.Errorf("failed to resolve domain: %w", err)
	}
	if len(ips) == 0 {
		return "", fmt.Errorf("no IP addresses found for domain: %s", domain)
	}
	return ips[0].String(), nil
}

// (TYPE (1) | ADDR (var) | DSTPORT (2))
func readAddr(conn net.Conn) (string, uint16, error) {
	buf := make([]byte, 1)
	var addr string
	var port uint16

	if n, err := conn.Read(buf); err != nil || n < 1 {
		return "", 0, fmt.Errorf("expected 1 byte (TYPE): %w", err)
	}

	switch buf[0] {
	case 0x01:
		ip := make([]byte, 4)
		if n, err := conn.Read(ip); err != nil || n < 4 {
			return "", 0, fmt.Errorf("expected 4 bytes (IPv4_ADDR): %w", err)
		}
		addr = net.IP(ip).String()
	case 0x03:
		domainSizeBuf := make([]byte, 1)
		if n, err := conn.Read(domainSizeBuf); err != nil || n < 1 {
			return "", 0, fmt.Errorf("expected 1 byte (domain size): %w", err)
		}
		domain := make([]byte, domainSizeBuf[0])
		if _, err := io.ReadFull(conn, domain); err != nil {
			return "", 0, fmt.Errorf("expected %d bytes (domain): %w", domainSizeBuf[0], err)
		}
		addr = string(domain)
		// var err error
		// addr, err = resolveDomain(string(domain))
		// if err != nil {
		// 	return "", 0, err
		// }
	default:
		return "", 0, errors.New("unsupported address type")
	}

	portBuf := make([]byte, 2)
	if n, err := conn.Read(portBuf); err != nil || n < 2 {
		return "", 0, fmt.Errorf("expected 2 bytes (DSTPORT): %w", err)
	}
	port = binary.BigEndian.Uint16(portBuf)

	return addr, port, nil
}

func convertAddrToBytes(addr net.Addr) []byte {
	var addrBytes []byte

	tcpAddr, ok := addr.(*net.TCPAddr)
	if !ok {
		log.Printf("unsupported address type: %T", addr)
		return []byte{1, 0, 0, 0, 0, 0, 0}
	}

	if tcpAddr.IP.To4() != nil {
		addrBytes = append(addrBytes, 1)
		addrBytes = append(addrBytes, tcpAddr.IP.To4()...)
	} else {
		addrBytes = append(addrBytes, 2)
		addrBytes = append(addrBytes, tcpAddr.IP.To16()...)
	}

	portBytes := []byte{
		byte(tcpAddr.Port >> 8),
		byte(tcpAddr.Port & 0xFF),
	}
	addrBytes = append(addrBytes, portBytes...) // распаковка среза

	return addrBytes
}

/*
VER (1) | CMD (1) | RSV (1)        | ATYP (1)   | DST.ADDR (var)   | DST.PORT (2)
версия  | команда | резервный байт | тип адреса | адрес назначения | порт назначения
*/
func handleRequest(clientConn net.Conn) (net.Conn, error) {
	buffer := make([]byte, 3)
	n, err := clientConn.Read(buffer)
	if err != nil {
		return nil, err
	}
	if n < 3 {
		return nil, errors.New("expected 3 bytes (VER, CMD, RSV)")
	}
	if buffer[0] != VERSION_SOCKS5 {
		return nil, errors.New("unsupported SOCKS version")
	}
	if buffer[1] != CMD_CONNECT {
		return nil, errors.New("unsupported command")
	}

	serverAddr, serverPort, err := readAddr(clientConn)
	if err != nil {
		return nil, fmt.Errorf("address reading error: %w", err)
	}
	// fmt.Println("serverAddr: ", serverAddr)
	// fmt.Println("serverPort: ", serverPort)

	serverConn, err := net.Dial("tcp", fmt.Sprintf("%s:%d", serverAddr, serverPort))
	if err != nil {
		response := []byte{VERSION_SOCKS5, REQUEST_FAILURE, 0, 1, 0, 0, 0, 0, 0, 0}
		clientConn.Write(response)
		return nil, fmt.Errorf("server connection error: %w", err)
	}

	localAddr := convertAddrToBytes(serverConn.LocalAddr())
	fmt.Println("localAddr: ", serverConn.LocalAddr())
	fmt.Println("localAddr: ", localAddr)
	response := []byte{VERSION_SOCKS5, REQUEST_GRANTED, 0}
	response = append(response, localAddr...)
	_, err = clientConn.Write(response)
	if err != nil {
		defer serverConn.Close()
		return nil, fmt.Errorf("error: %w", err)
	}

	return serverConn, nil
}

// Проксирование данных между клиентом и сервером назначения
func transfer(clientConn, serverConn net.Conn, clientNum int) {
	var wg sync.WaitGroup
	wg.Add(2)

	var writtenToServer int64
	var writtenToClient int64

	go func() {
		defer wg.Done()
		writtenToServer, _ = io.Copy(serverConn, clientConn)
		serverConn.(*net.TCPConn).CloseWrite()
	}()
	go func() {
		defer wg.Done()
		writtenToClient, _ = io.Copy(clientConn, serverConn)
		clientConn.(*net.TCPConn).CloseWrite()
	}()

	wg.Wait()

	fmt.Printf("client_%d -> server: %d bytes\n", clientNum, writtenToServer)
	fmt.Printf("client_%d <- server: %d bytes\n", clientNum, writtenToClient)
}

func handleClient(clientConn net.Conn, clientNum int) {
	defer clientConn.Close()
	fmt.Printf("client_%d start\n", clientNum)
	defer fmt.Printf("client_%d finish\n", clientNum)

	if err := handleGreeting(clientConn); err != nil {
		log.Println("greeting failed:", err)
		return
	}

	serverConn, err := handleRequest(clientConn)
	if err != nil {
		log.Println("handle request failed:", err)
		return
	}
	defer serverConn.Close()

	transfer(clientConn, serverConn, clientNum)
}

func main() {
	if len(os.Args) != 2 {
		log.Fatalf("Usage: go run proxy.go <port>")
	}
	port := os.Args[1]

	listener, err := net.Listen("tcp", "0.0.0.0:"+port)
	if err != nil {
		log.Fatalf("Failed to bind to port %s: %v", port, err)
	}
	defer listener.Close()
	myAddr := listener.Addr().String()
	fmt.Printf("Proxy is listening on %s\n", myAddr)

	clientNum := 1
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Printf("Failed to accept connection: %v", err)
			continue
		}
		go handleClient(conn, clientNum)
		clientNum += 1
	}
}

// curl --socks5-hostname PROXY_HOST:PROXY_PORT https://api.ipify.org
