package main

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"strings"
	"sync"
	"time"
)

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

var charToProtocolType = map[string]string{
	"01": "PING",
	"02": "PONG",
	"03": "ARP",
	"04": "DHCP",
	"05": "UDP",
	"06": "DNS",
}
var protocolTypeToChar = map[string]string{
	"PING": "01",
	"PONG": "02",
	"ARP":  "03",
	"DHCP": "04",
	"UDP":  "05",
	"DNS":  "06",
}

type Package struct {
	destinationMAC string
	sourceMAC      string
	protocolType   string
	destinationIP  string
	sourceIP       string
	Operation      string
	data           string
}

type Client struct {
	pkg  Package
	conn net.Conn
	ms   string
	port string
}

type Router struct {
	tableCAM map[string]net.Conn // MAC -> conn
	tableARP map[string]string   // IP -> MAC
	mu       sync.RWMutex
	mac      string
	ip       string
	port     string
}

type natEntry struct {
	ClientIP  string
	ClientMAC string
	Conn      net.Conn
}

var router = &Router{
	tableCAM: make(map[string]net.Conn),
	tableARP: make(map[string]string),
	mac:      "MM:MM:MM:MM:MM:MM",
	ip:       "127.000.000.001",
	port:     ":9000",
}
var response = make(chan Package)
var natTable = struct {
	sync.RWMutex
	m map[string]natEntry
}{m: make(map[string]natEntry)}
var switchAddr = "127.0.0.1:8080"
var switcher Client

func connectToRouter(targetAddr string) (Client, error) {
	var client Client
	conn, err := net.Dial("tcp", targetAddr)
	if err != nil {
		return Client{}, fmt.Errorf("ошибка подключения к коммутатору: %w", err)
	}
	client.conn = conn

	client.port = getClientPort(client.conn)
	fmt.Println(router.port + ": Подключение на порту " + client.port)

	return client, nil
}

func listen() error {
	ln, err := net.Listen("tcp", router.port)
	if err != nil {
		return fmt.Errorf("ошибка запуска сервера по адресу %s:\n %w", router.port, err)
	}
	fmt.Println("Сервер запущен на ", router.port)
	for {
		var client Client
		var err error
		client.conn, err = ln.Accept()
		if err != nil {
			fmt.Println("Ошибка принятия соединения: ", err)
			continue
		}

		client.port = getClientPort(client.conn)
		fmt.Println(router.port + ": Подключение на порту " + client.port)

		router.tableCAM[client.port] = client.conn
		go handleClientConnection(&client)
	}
}

func getClientPort(conn net.Conn) string {
	addr := conn.RemoteAddr()
	_, port, _ := net.SplitHostPort(addr.String())
	return port
}

func handleClientConnection(client *Client) {
	defer (*client).conn.Close()

	readClientRequests(client)

	err := unregisterClient(client)
	if err != nil {
		fmt.Println(err)
	}
}

func readClientRequests(client *Client) {
	reader := bufio.NewReader(client.conn)
	addedToTable := false
	for {
		message, err := reader.ReadString('\n')
		//log.Printf("[DEBUG] read from %s: %q, err: %v\n", client.conn.RemoteAddr(), message, err)
		if err != nil {
			break
		}
		message = strings.TrimSpace(message)
		if message == "" {
			fmt.Println("Получено пустое сообщение")
			continue
		}
		client.pkg, err = getPackage(message)
		if err != nil {
			fmt.Printf("\n%s: %s: Ошибка обработки пакета: %v\n%s\n", router.port, client.port, err, message)
			continue
		}
		fmt.Printf("\n%s: %s: Получен пакет:\n%s\n", router.port, client.port, message)

		if !addedToTable {
			addedToTable = addInCAM(client.conn, client.port, client.pkg.sourceMAC)
		}

		if err = processRequest(client); err != nil {
			fmt.Println(err)
		}
	}
}

func unregisterClient(client *Client) error {
	router.mu.Lock()
	defer router.mu.Unlock()

	for clientMAC, clientConn := range router.tableCAM {
		if clientConn == client.conn {
			delete(router.tableCAM, clientMAC)
			fmt.Println(client.port, ": Удалён: ", clientMAC)
			return nil
		}
	}
	return fmt.Errorf("удаление из таблицы CAM завершилось ошибкой")
}

func getPackage(message string) (Package, error) {
	if len(message) < offsetOperation {
		return Package{}, fmt.Errorf("сообщение слишком короткое\n<%s>", message)
	}

	destMAC := message[OffsetDestMAC:OffsetSrcMAC]
	srcMAC := message[OffsetSrcMAC:OffsetProtocolType]

	typeChar := message[OffsetProtocolType:OffsetDestIP]
	protocolType, ok := charToProtocolType[typeChar]
	if !ok {
		return Package{}, fmt.Errorf("неизвестный тип пакета: %s\n", typeChar)
	}

	destIP := strings.TrimSpace(message[OffsetDestIP:OffsetSrcIP])
	srcIP := strings.TrimSpace(message[OffsetSrcIP:offsetOperation])

	var oper, data string
	if protocolType == "PING" || protocolType == "PONG" || protocolType == "UDP" {
		oper = ""
		data = message[offsetOperation:]
	} else {
		oper = message[offsetOperation:OffsetData]
		data = message[OffsetData:]
	}

	return Package{
		protocolType:   protocolType,
		destinationMAC: destMAC,
		sourceMAC:      srcMAC,
		destinationIP:  destIP,
		sourceIP:       srcIP,
		Operation:      oper,
		data:           data,
	}, nil
}

func addInCAM(senderConn net.Conn, senderPort, senderMac string) bool {
	if senderConn == switcher.conn {
		return true
	}
	router.mu.Lock()
	defer router.mu.Unlock()

	found := false
	if _, found = router.tableCAM[senderMac]; !found {
		for port, clientN := range router.tableCAM {
			if clientN == senderConn {
				delete(router.tableCAM, port)
				fmt.Println(senderPort+": Удалён:", port)
				router.tableCAM[senderMac] = senderConn
				fmt.Println(senderPort + ": Добавлен:" + senderMac + "\n")
				found = true
			}
		}
		if !found {
			fmt.Println(senderPort + " не был добавлен в САМ таблицу")
		}
	} else {
		fmt.Println(senderPort + " уже есть в САМ таблице")
	}
	return found
}

func isLocalIP(ip string) bool {
	return ip == "0.0.0.0" || ip == "255.255.255.255" || strings.HasPrefix(ip, "192.168.") || ip == router.ip
}

func processRequest(client *Client) error {
	pkg := client.pkg

	if pkg.protocolType == "ARP" {
		return processArp(client.conn, pkg)
	} else if pkg.destinationIP == router.ip {
		return toLAN(pkg)
	} else if !isLocalIP(client.pkg.destinationIP) {
		return toInternet(pkg)
	} else {
		return fmt.Errorf("ошибка обработки пакета маршрутизатором:\n" + packageToString(pkg) + "\n")
	}
}

func processArp(senderConn net.Conn, pkg Package) error {
	if pkg.Operation == "RES" {
		response <- pkg
	} else if pkg.Operation == "REQ" {
		if pkg.destinationIP == router.ip || (!isLocalIP(pkg.destinationIP) && isLocalIP(pkg.sourceIP)) || (isLocalIP(pkg.destinationIP) && !isLocalIP(pkg.sourceIP)) {
			return sendMessage(senderConn, pkg.sourceMAC, protocolTypeToChar["ARP"], pkg.sourceIP, "RES")
		}
	} else {
		return fmt.Errorf("неизвестная операци в ARP протоколе\n" + packageToString(pkg))
	}

	return nil
}

func toInternet(pkg Package) error {
	if isLocalIP(pkg.destinationIP) {
		return fmt.Errorf("целевой IP локальный\n" + packageToString(pkg))
	}

	if pkg.destinationMAC != router.mac {
		return fmt.Errorf("МАК получателя не совпадает с МАК маршрутизатора\n" + packageToString(pkg))
	}

	natTable.Lock()
	defer natTable.Unlock()

	var err error
	destMac, ok := router.tableARP[pkg.destinationIP]
	if !ok {
		destMac, err = getMacAddress(pkg.destinationIP)
		if err != nil {
			return err
		}
	}
	conn, ok := router.tableCAM[destMac]
	if !ok {
		return fmt.Errorf("собеседника с таким МАК нет(")
	}

	natTable.m[pkg.destinationIP] = natEntry{
		ClientIP:  pkg.sourceIP,
		ClientMAC: pkg.sourceMAC,
		Conn:      conn,
	}

	pkg.sourceIP = router.ip
	pkg.sourceMAC = router.mac
	pkg.destinationMAC = destMac

	err = sendPackage(conn, pkg)
	if err != nil {
		return fmt.Errorf("ошибка при отправке в интернет:", err)
	}

	return nil
}

func toLAN(pkg Package) error {
	if !isLocalIP(pkg.destinationIP) {
		return fmt.Errorf("целевой IP НЕ локальный\n" + packageToString(pkg))
	}

	if pkg.destinationMAC != router.mac {
		return fmt.Errorf("МАК получателя не совпадает с МАК маршрутизатора\n" + packageToString(pkg))
	}

	natTable.RLock()
	entry, ok := natTable.m[pkg.sourceIP]
	natTable.RUnlock()

	if !ok {
		return fmt.Errorf("собеседник для %s в этой локальной сети не найден\n", pkg.sourceIP)
	}

	pkg.destinationIP = entry.ClientIP
	pkg.destinationMAC = entry.ClientMAC
	pkg.sourceMAC = router.mac

	err := sendPackage(switcher.conn, pkg)
	if err != nil {
		return fmt.Errorf("ошибка при отправке в LAN:", err)
	}

	return nil
}

func getMacAddress(destinationIP string) (string, error) {
	if mac, ok := router.tableARP[destinationIP]; ok {
		return mac, nil
	}

	for _, conn := range router.tableCAM {
		if err := sendMessage(conn,
			"FF:FF:FF:FF:FF:FF",
			protocolTypeToChar["ARP"],
			destinationIP,
			"REQ"); err != nil {
			return "", err
		}
	}

	pkg, err := waitForResponse("ARP", router.ip, 2*time.Second)
	if err != nil {
		return "", err
	}
	if pkg.Operation != "RES" {
		return "", fmt.Errorf("получен не ARP RESPONSE" + packageToString(pkg))
	}

	router.tableARP[destinationIP] = pkg.sourceMAC
	return pkg.sourceMAC, nil
}

func waitForResponse(protocolType, sourceIp string, timeout time.Duration) (Package, error) {
	timer := time.NewTimer(timeout)
	defer timer.Stop()

	for {
		select {
		case pkg := <-response:
			if pkg.protocolType == protocolType && (sourceIp == "" || pkg.destinationIP == sourceIp) {
				return pkg, nil
			}
		case <-timer.C:
			return Package{}, fmt.Errorf("таймаут ожидания %s", protocolType)
		}
	}
}

func sendMessage(conn net.Conn, destinationMAC, protocolType, destinationIP, data string) error {
	if charProtocolType, ok := protocolTypeToChar[protocolType]; ok {
		protocolType = charProtocolType
	}
	message := destinationMAC + router.mac + protocolType + destinationIP + router.ip + data
	_, err := conn.Write([]byte(message + "\n"))
	if err != nil {
		return fmt.Errorf("sendMessage:\nошибка отправки пакета: %s\n%w", message, err)
	}
	log.Printf("SEND: %q", message)
	fmt.Println("Пакет отправлен " + getClientPort(conn))
	return nil
}

func sendPackage(conn net.Conn, pkg Package) error {
	if charProtocolType, ok := protocolTypeToChar[pkg.protocolType]; ok {
		pkg.protocolType = charProtocolType
	}
	message := packageToString(pkg)
	_, err := conn.Write([]byte(message + "\n"))
	if err != nil {
		return fmt.Errorf("sendPackage:\nошибка отправки пакета: %s\n%w", message, err)
	}
	log.Printf("SEND: %q", message)
	fmt.Println("Пакет отправлен " + getClientPort(conn))
	return nil
}

func packageToString(pkg Package) string {
	return pkg.destinationMAC + pkg.sourceMAC + pkg.protocolType + pkg.destinationIP + pkg.sourceIP + pkg.Operation + pkg.data
}

func main() {
	var err error
	switcher, err = connectToRouter(switchAddr)
	if err != nil {
		fmt.Println("Ошибка:", err)
	}

	go handleClientConnection(&switcher)

	err = listen()
	if err != nil {
		fmt.Println("Ошибка:", err)
	}

}
