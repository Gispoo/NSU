package main

import (
	"bufio"
	"fmt"
	"net"
	"strconv"
	"strings"
	"sync"
)

type DNS struct {
	MACAddress string
	IPAddress  string
	conn       net.Conn
}

var dns = DNS{
	MACAddress: "DN:DN:DN:DN:DN:DN",
	IPAddress:  "192.168.001.000",
}

type Package struct {
	destinationMAC string
	sourceMAC      string
	requestType    string
	destinationIP  string
	sourceIP       string
	Operation      string
	data           string
}

// Domain -> IPv4
var records = make(map[string]string)
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

func connectToRouter(localAddr, routerAddress *net.TCPAddr) error {
	conn, err := net.DialTCP("tcp", localAddr, routerAddress)
	if err != nil {
		return fmt.Errorf("ошибка подключения к маршрутизатору: %w", err)
	}
	dns.conn = conn
	return nil
}

func listen() (string, error) {
	reader := bufio.NewReader(dns.conn)
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

	case "DNS":
		fmt.Printf("\nполучен DNS пакет:\n%s\n", message)
		return processDns(pkg)

	default:
		fmt.Println("\nНеизвестный тип протокола:\n" + message)
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

func processPing(pkg Package) error {
	return sendMessage(pkg.sourceMAC, protocolTypeToChar["PONG"], pkg.sourceIP, pkg.data)
}

func processArp(pkg Package) error {
	if pkg.Operation == "REQ" {
		if pkg.destinationIP == dns.IPAddress {
			return sendMessage(pkg.sourceMAC, protocolTypeToChar["ARP"], pkg.sourceIP, "RES")
		}
	}
	return nil
}

func processDns(inputPkg Package) error {
	mutex.Lock()
	defer mutex.Unlock()

	switch inputPkg.Operation {
	case "REQ":
		if isValidIP(inputPkg.data) {
			return sendMessage(inputPkg.sourceMAC, protocolTypeToChar["DNS"], inputPkg.sourceIP, "RES"+inputPkg.data)
		}
		ip, ok := records[inputPkg.data]
		if !ok {
			return sendMessage(inputPkg.sourceMAC, protocolTypeToChar["DNS"], inputPkg.sourceIP, "RES Нет такого")
		} else {
			return sendMessage(inputPkg.sourceMAC, protocolTypeToChar["DNS"], inputPkg.sourceIP, "RES"+ip)
		}
	case "REG":
		records[inputPkg.data] = inputPkg.sourceIP
		return sendMessage(inputPkg.sourceMAC, protocolTypeToChar["DNS"], inputPkg.sourceIP, "ACKИмя зарегистрировано")
	default:
		return fmt.Errorf("")
	}
}

func isValidIP(ip string) bool {
	parts := strings.Split(ip, ".")
	if len(parts) != 4 {
		return false
	}

	for _, part := range parts {
		if len(part) != 3 {
			return false // строго 3 цифры
		}
		n, err := strconv.Atoi(part)
		if err != nil || n < 0 || n > 255 {
			return false
		}
	}
	return true
}

func sendMessage(destinationMAC, requestType, destinationIP, data string) error {
	message := destinationMAC + dns.MACAddress + requestType + destinationIP + dns.IPAddress + data
	_, err := dns.conn.Write([]byte(message + "\n"))
	if err != nil {
		return fmt.Errorf("ошибка отправки пакета:%s\n%w", message, err)
	}
	return nil
}

func main() {
	localAddr, _ := net.ResolveTCPAddr("tcp", "0.0.0.0:0053")
	routerAddress, _ := net.ResolveTCPAddr("tcp", "127.0.0.1:8080")
	err := connectToRouter(localAddr, routerAddress)
	if err != nil {
		fmt.Println(err)
		return
	}
	fmt.Println("DNS сервер слушает на порту", localAddr)
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
