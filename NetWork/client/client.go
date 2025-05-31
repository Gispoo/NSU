package main

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"os"
	"strconv"
	"strings"
	"time"
)

type Client struct {
	MACAddress string
	IPAddress  string
	conn       net.Conn
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

// IP -> MAC
var tableARP = make(map[string]string)
var client = Client{
	MACAddress: "AA:BB:CC:DD:EE:01",
	IPAddress:  "000.000.000.000",
}
var responseChan = make(chan Package)
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

var routerAddress *net.TCPAddr
var routerIP = "127.000.000.001"
var dnsIP = ""
var reader = bufio.NewReader(os.Stdin)

func initClient(Args []string) {
	var err error
	routerAddress, err = net.ResolveTCPAddr("tcp", "127.0.0.1:8080")
	if len(Args) > 1 {
		client.MACAddress = Args[1]
	}

	if len(Args) > 2 {
		client.IPAddress = Args[2]
		dnsIP = "192.168.001.000"

		routerAddress, err = net.ResolveTCPAddr("tcp", "127.0.0.1:9000")
		if err != nil {
			log.Printf(err.Error())
		}
	}
}

func promptUserPort() {
	for {
		log.Print("Введите желаемый исходный порт: ")
		input, _ := reader.ReadString('\n')
		input = strings.TrimSpace(input)

		port, err := strconv.Atoi(input)
		if err != nil || port < 1024 || port > 65535 {
			log.Println("Недопустимый порт. Укажите число от 1024 до 65535.")
			continue
		}

		localAddr, err := net.ResolveTCPAddr("tcp", fmt.Sprintf("0.0.0.0:%d", port))
		if err != nil {
			log.Println("Ошибка при разборе адреса:", err)
			continue
		}

		err = connectToRouter(localAddr, routerAddress)
		if err != nil {
			log.Println("Порт занят. Попробуйте другой.")
			continue
		}

		return
	}
}

func connectToRouter(localAddr, routerAddress *net.TCPAddr) error {
	conn, err := net.DialTCP("tcp", localAddr, routerAddress)
	if err != nil {
		return fmt.Errorf("ошибка подключения к маршрутизатору: %w", err)
	}
	client.conn = conn
	return nil
}

func handleConnection() error {
	defer client.conn.Close()

	ipReady := make(chan struct{})

	go func() {
		<-ipReady
		for {
			sendMessage(protocolTypeToChar["PING"], "", client.MACAddress, "", client.IPAddress, "")
		}
	}()

	go func() {
		for {
			inputMessage, err := listen()
			if err != nil {
				log.Printf(err.Error())
				log.Printf("Отключение...")
				break
			}
			if err := processInputPackage(inputMessage); err != nil {
				log.Printf(err.Error())
			}
		}
	}()

	if client.IPAddress == "000.000.000.000" {
		err := getIP()
		if err != nil {
			log.Printf(err.Error())
			return err
		}

		err = getDomainName()
		if err != nil {
			log.Printf(err.Error())
			return err
		}

		log.Printf("Получен IP: " + client.IPAddress)
	}

	close(ipReady)

	select {}
}

func getIP() error {
	log.Printf("Введите желаемый IP формата 192.168.ххх.ххх: ")
	desiredIP, err := reader.ReadString('\n')
	if err != nil {
		return fmt.Errorf("ошибка чтения желаемого IP: %w", err)
	}
	desiredIP = strings.TrimSpace(desiredIP)
	if err := sendMessage(
		protocolTypeToChar["DHCP"],
		"FF:FF:FF:FF:FF:FF",
		client.MACAddress,
		"255.255.255.255",
		client.IPAddress,
		"DIS"+desiredIP,
	); err != nil {
		return fmt.Errorf("не удалось отправить DISCOVER: %w", err)
	}

	pkg, err := waitForResponse("DHCP", "", 2*time.Second)
	if err != nil {
		return err
	}
	if pkg.Operation != "OFF" {
		return fmt.Errorf("получен не OFFER пакет")
	}

	client.IPAddress = pkg.destinationIP
	dnsIP = strings.TrimSpace(pkg.data)

	if err := sendMessage(
		protocolTypeToChar["DHCP"],
		pkg.sourceMAC,
		client.MACAddress,
		pkg.sourceIP,
		pkg.destinationIP,
		"REQ",
	); err != nil {
		return fmt.Errorf("не удалось отправить REQUEST: %w", err)
	}
	return nil
}

func getDomainName() error {
	log.Printf("Введите желаемое имя: ")
	desiredAddress, err := reader.ReadString('\n')
	if err != nil {
		return fmt.Errorf("ошибка чтения желаемого имени: %w", err)
	}
	desiredAddress = strings.TrimSpace(desiredAddress)
	if err := sendMessage(
		protocolTypeToChar["DNS"],
		"",
		client.MACAddress,
		dnsIP,
		client.IPAddress,
		"REG"+desiredAddress,
	); err != nil {
		return fmt.Errorf("не удалось отправить DNS REGISTER REQ: %w", err)
	}

	pkg, err := waitForResponse("DNS", client.IPAddress, 2*time.Second)

	log.Printf(pkg.data)

	return err
}

func sendMessage(requestType, destinationMAC, sourceMAC, destinationIP, sourceIP, data string) error {
	message := ""
	var err error
	if data == "" {
		data, err = appLayer()
		if err != nil {
			return err
		}
	}
	message = data + message

	if sourceIP == "" {
		sourceIP = client.IPAddress
	}
	message = sourceIP + message

	if destinationIP == "" {
		destinationIP, err = internetLayer()
		if err != nil {
			return err
		}
	}
	message = destinationIP + message

	message = requestType + message

	if sourceMAC == "" {
		sourceMAC = client.MACAddress
	}
	message = sourceMAC + message

	if destinationMAC == "" {
		if !isLocalIP(destinationIP) {
			destinationMAC, err = getMacAddress(routerIP)
			if err != nil {
				return err
			}
		} else {
			destinationMAC, err = getMacAddress(destinationIP)
			if err != nil {
				return err
			}
		}
	}
	message = destinationMAC + message

	_, err = client.conn.Write([]byte(strings.TrimSpace(message) + "\n"))
	if err != nil {
		return err
	}
	log.Printf("Отправлен пакет:%s", message)
	return nil
}

func appLayer() (string, error) {
	log.Printf("Введите текст запроса: ")
	data, err := reader.ReadString('\n')
	if err != nil {
		return "", err
	}

	return strings.TrimSpace(data), nil
}

func internetLayer() (string, error) {
	log.Printf("Введите адрес или имя получателя: ")
	destAddress, err := reader.ReadString('\n')
	if err != nil {
		return "", err
	}
	destAddress = strings.TrimSpace(destAddress)
	if isValidIP(destAddress) {
		return destAddress, nil
	}
	log.Printf("Запрашиваем IP у DNS по имени %s\n", destAddress)
	if err = sendMessage(protocolTypeToChar["DNS"],
		"",
		client.MACAddress,
		dnsIP,
		client.IPAddress,
		"REQ"+destAddress); err != nil {
		return "", err
	}

	pkg, err := waitForResponse("DNS", client.IPAddress, 2*time.Second)
	if err != nil {
		return "", err
	}
	if pkg.Operation != "RES" {
		return "", fmt.Errorf("получен не DNS RESPONSE")
	}

	return strings.TrimSpace(pkg.data), nil
}

func getMacAddress(destinationIP string) (string, error) {
	if mac, ok := tableARP[destinationIP]; ok {
		return mac, nil
	}

	if err := sendMessage(protocolTypeToChar["ARP"],
		"FF:FF:FF:FF:FF:FF",
		client.MACAddress,
		destinationIP,
		client.IPAddress,
		"REQ"); err != nil {
		return "", err
	}

	pkg, err := waitForResponse("ARP", client.IPAddress, 2*time.Second)
	if err != nil {
		return "", err
	}
	if pkg.Operation != "RES" {
		return "", fmt.Errorf("получен не ARP RESPONSE")
	}

	tableARP[destinationIP] = pkg.sourceMAC
	return pkg.sourceMAC, nil
}

func waitForResponse(requestType, destIP string, timeout time.Duration) (Package, error) {
	timer := time.NewTimer(timeout)
	defer timer.Stop()

	for {
		select {
		case pkg := <-responseChan:
			if pkg.requestType == requestType && (destIP == "" || pkg.destinationIP == destIP) {
				return pkg, nil
			}
		case <-timer.C:
			return Package{}, fmt.Errorf("таймаут ожидания %s", requestType)
		}
	}
}

func listen() (string, error) {
	reader := bufio.NewReader(client.conn)
	inputMessage, err := reader.ReadString('\n')
	if err != nil {
		return "", fmt.Errorf("ошибка чтения ответа: %w\nвероятно, роутер отключен. Завершение работы...", err)
	}

	return strings.TrimSpace(inputMessage), nil
}

func processInputPackage(message string) error {
	pkg, err := getPackage(message)
	if err != nil {
		log.Printf("Ошибка обработки пакета: %v", err)
		return err
	}
	addMac(pkg)

	switch pkg.requestType {
	case "PING":
		log.Printf("получен PING пакет: " + message)
		return processPing(pkg)

	case "PONG":
		log.Printf("получен PONG пакет: " + message)

	case "ARP":
		log.Printf("получен ARP пакет: " + message)
		return processArp(pkg)

	case "UDP":
		log.Printf("получен UDP пакет: " + message)

	case "DHCP":
		log.Printf("получен DHCP пакет: " + message)
		if pkg.Operation == "OFF" {
			responseChan <- pkg
		}

	case "DNS":
		log.Printf("получен DNS пакет: " + message)
		if pkg.Operation == "RES" || pkg.Operation == "ACK" {
			responseChan <- pkg
		}

	default:
		log.Printf("Неизвестный тип протокола: " + message)
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

func addMac(pkg Package) {
	if _, ok := tableARP[pkg.sourceIP]; !ok && pkg.requestType != "DHCP" && pkg.sourceIP != "000.000.000.000" {
		log.Printf("Добавляем MAC-адрес от %s в ARP таблицу", pkg.sourceIP)
		tableARP[pkg.sourceIP] = pkg.sourceMAC
	}
}

func processPing(pkg Package) error {
	return sendMessage(protocolTypeToChar["PONG"], pkg.sourceMAC, client.MACAddress, pkg.sourceIP, client.IPAddress, pkg.data)
}

func processArp(pkg Package) error {
	if pkg.Operation == "REQ" {
		if pkg.destinationIP == client.IPAddress {
			return sendMessage(protocolTypeToChar["ARP"], pkg.sourceMAC, client.MACAddress, pkg.sourceIP, client.IPAddress, "RES")
		}
	} else if pkg.Operation == "RES" {
		responseChan <- pkg
	}

	return nil
}

func isLocalIP(ip string) bool {
	return ip == "0.0.0.0" || ip == "255.255.255.255" || strings.HasPrefix(ip, "192.168.") || ip == "127.000.000.001"
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

func main() {
	initClient(os.Args)

	f, err := os.OpenFile(client.MACAddress[15:]+".log", os.O_CREATE|os.O_TRUNC|os.O_WRONLY, 0644)
	if err != nil {
		log.Fatalf("Не удалось открыть лог-файл: %v", err)
	}
	log.SetOutput(f)
	log.SetFlags(log.LstdFlags)
	defer f.Close()

	promptUserPort()

	if err := handleConnection(); err != nil {
		log.Printf(err.Error())
		return
	}
}
