package main

import (
	"bufio"
	"errors"
	"fmt"
	"net"
	"strings"
	"sync"
)

type Package struct {
	destinationMAC string
	sourceMAC      string
	requestType    string
	destinationIP  string
	sourceIP       string
	Operation      string
	data           string
}

var macToIpTable = make(map[string]string) // MAC -> IP
var IPPool = []string{
	"192.168.000.001",
	"192.168.000.002",
	"192.168.000.003",
	"192.168.000.004",
	"192.168.000.005",
	"192.168.000.006",
	"192.168.000.007",
	"192.168.000.008",
	"192.168.000.009",
	"192.168.000.010",
	"192.168.000.011",
}
var mutex = &sync.Mutex{}
var protocolTypeToChar = map[string]string{
	"PING": "01",
	"PONG": "02",
	"ARP":  "03",
	"DHCP": "04",
	"UDP":  "05",
	"DNS":  "06",
}
var charToProtocolType = map[string]string{
	"01": "PING",
	"02": "PONG",
	"03": "ARP",
	"04": "DHCP",
	"05": "UDP",
	"06": "DNS",
}

const (
	LengthMAC     = 17
	OffsetDestMAC = 0
	OffsetSrcMAC  = OffsetDestMAC + LengthMAC

	OffsetProtocolType = OffsetSrcMAC + LengthMAC
	LengthProtocolType = 2

	LengthIP     = 15
	OffsetDestIP = OffsetProtocolType + LengthProtocolType
	OffsetSrcIP  = OffsetDestIP + LengthIP

	LengthOperation = 3
	offsetOperation = OffsetSrcIP + LengthIP

	OffsetData = offsetOperation + LengthOperation
)

type DHCP struct {
	MACAddress string
	IPAddress  string
	conn       net.Conn
}

var dhcp = DHCP{
	MACAddress: "DH:DH:DH:DH:DH:DH",
	IPAddress:  "192.168.002.000",
}

var ipDns = "192.168.001.000"

func connectToRouter(localAddr, routerAddress *net.TCPAddr) error {
	conn, err := net.DialTCP("tcp", localAddr, routerAddress)
	if err != nil {
		return fmt.Errorf("ошибка подключения к маршрутизатору: %w", err)
	}
	dhcp.conn = conn
	return nil
}

func listen() (string, error) {
	reader := bufio.NewReader(dhcp.conn)
	inputMessage, err := reader.ReadString('\n')
	inputMessage = strings.TrimSpace(inputMessage)
	if err != nil {
		return "", fmt.Errorf("ошибка чтения ответа: %w\nвероятно, роутер отключен. Завершение работы...", err)
	}

	return inputMessage, nil
}

func processInputPackage(message string) error {
	pkg, err := getPackage(message)
	if err != nil {
		return err
	}

	switch pkg.requestType {
	case "PING":
		fmt.Println("\nполучен PING пакет:\n" + message)
		return processPing(pkg)

	case "PONG":
		fmt.Println("\nполучен PONG пакет:\n" + message)

	case "ARP":
		fmt.Println("\nполучен ARP пакет:\n" + message)
		return processArp(pkg)

	case "UDP":
		fmt.Println("\nполучен UDP пакет:\n" + message)

	case "DHCP":
		fmt.Println("\nполучен DHCP пакет:\n" + message)
		return processDhcp(pkg)

	default:
		fmt.Printf("\nНеизвестный тип протокола: %s\n%s\n", pkg.requestType, message)
	}
	return nil
}

func getPackage(message string) (Package, error) {
	if len(message) < offsetOperation {
		return Package{}, fmt.Errorf("сообщение слишком короткое\n%s", message)
	}

	destMAC := message[OffsetDestMAC:OffsetSrcMAC]
	srcMAC := message[OffsetSrcMAC:OffsetProtocolType]

	typeChar := message[OffsetProtocolType:OffsetDestIP]
	requestType, ok := charToProtocolType[typeChar]
	if !ok {
		return Package{}, fmt.Errorf("неизвестный тип пакета: %s\n", typeChar)
	}

	destIP := strings.TrimSpace(message[OffsetDestIP:OffsetSrcIP])
	srcIP := strings.TrimSpace(message[OffsetSrcIP:offsetOperation])

	var oper, data string
	if requestType == "PING" || requestType == "PONG" || requestType == "UDP" {
		oper = ""
		data = message[offsetOperation:]
	} else {
		oper = message[offsetOperation:OffsetData]
		data = message[OffsetData:]
	}

	return Package{
		requestType:    requestType,
		destinationMAC: destMAC,
		sourceMAC:      srcMAC,
		destinationIP:  destIP,
		sourceIP:       srcIP,
		Operation:      oper,
		data:           data,
	}, nil
}

func processPing(inputPkg Package) error {
	return sendMessage(inputPkg.sourceMAC, protocolTypeToChar["PONG"], inputPkg.sourceIP, inputPkg.data)
}

func processArp(inputPkg Package) error {
	if inputPkg.Operation == "REQ" {
		if inputPkg.destinationIP == dhcp.IPAddress {
			return sendMessage(inputPkg.sourceMAC, protocolTypeToChar["ARP"], inputPkg.sourceIP, "RES"+inputPkg.data)
		}
	} else if inputPkg.Operation == "REQ" {
		return nil
	}
	return fmt.Errorf("неверная ARP операция")
}

func processDhcp(inputPkg Package) error {
	switch inputPkg.Operation {
	case "DIS":
		mutex.Lock()
		ipToOffer, ok := macToIpTable[inputPkg.sourceMAC]
		mutex.Unlock()

		if ok {
			return sendMessage(inputPkg.sourceMAC, protocolTypeToChar["DHCP"], ipToOffer, "OFF"+ipDns)
		}

		newIP, err := getFreeIP(inputPkg.data)
		fmt.Println("Предложил IP: ", newIP)
		if err != nil {
			return err
		}
		return sendMessage(inputPkg.sourceMAC, protocolTypeToChar["DHCP"], newIP, "OFF"+ipDns)

	case "REQ":
		mutex.Lock()
		macToIpTable[inputPkg.sourceMAC] = inputPkg.sourceIP
		mutex.Unlock()
		fmt.Println("IP ", inputPkg.sourceIP, " подтвержден.")
		return nil

	default:
		return fmt.Errorf("неверная операция: %s\n", inputPkg.Operation)
	}

}

func sendMessage(destinationMAC, requestType, destinationIP, data string) error {
	message := destinationMAC + dhcp.MACAddress + requestType + destinationIP + dhcp.IPAddress + data
	_, err := dhcp.conn.Write([]byte(message + "\n"))
	if err != nil {
		return fmt.Errorf("ошибка отправки пакета:%s\n%w", message, err)
	}
	return nil
}

func getFreeIP(desiredIP string) (string, error) {
	mutex.Lock()
	defer mutex.Unlock()

	isUsed := func(ip string) bool {
		for _, usedIP := range macToIpTable {
			if usedIP == ip {
				return true
			}
		}
		return false
	}

	if desiredIP != "" {
		if !isUsed(desiredIP) {
			inPool := false
			for _, ip := range IPPool {
				if ip == desiredIP {
					inPool = true
					break
				}
			}
			if !inPool {
				IPPool = append(IPPool, desiredIP)
			}
			return desiredIP, nil
		}
	}

	for _, ip := range IPPool {
		if !isUsed(ip) {
			return ip, nil
		}
	}

	return "", errors.New("no available IP in pool")
}

func main() {
	localAddr, _ := net.ResolveTCPAddr("tcp", "0.0.0.0:0067")
	routerAddress, _ := net.ResolveTCPAddr("tcp", "127.0.0.1:8080")
	err := connectToRouter(localAddr, routerAddress)
	if err != nil {
		fmt.Println(err)
		return
	}
	fmt.Println("DHCP сервер слушает на порту", localAddr)
	for {
		inputMessage, err := listen()
		if err != nil {
			fmt.Println(err)
			fmt.Println("Отключение...")
			break
		}
		if err := processInputPackage(inputMessage); err != nil {
			fmt.Println(err)
		}
	}
}
