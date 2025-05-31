package main

import (
	"bufio"
	"fmt"
	"net"
	"strings"
	"sync"
	"time"
)

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
	ms   string
	conn net.Conn
	port string
}

type Server interface {
	processRequest(client *Client)
	getTableCAM() map[string]net.Conn
	getInboundClient() chan *Client
	getMutex() *sync.RWMutex
	getMAC() string
	getIP() string
	getPort() string
}

type BaseServer struct {
	tableCAM      map[string]net.Conn
	inboundClient chan *Client
	mu            sync.RWMutex
	mac           string
	ip            string
	port          string
}

func (b *BaseServer) getTableCAM() map[string]net.Conn {
	return b.tableCAM
}
func (b *BaseServer) getInboundClient() chan *Client {
	return b.inboundClient
}
func (b *BaseServer) getMutex() *sync.RWMutex {
	return &b.mu
}
func (b *BaseServer) getMAC() string {
	return b.mac
}
func (b *BaseServer) getIP() string {
	return b.ip
}
func (b *BaseServer) getPort() string { return b.port }

type Switcher struct {
	BaseServer
}

var switcher Server = &Switcher{
	BaseServer: BaseServer{
		// MAC -> conn
		tableCAM:      make(map[string]net.Conn),
		inboundClient: make(chan *Client, 100),
		mac:           "KK:KK:KK:KK:KK:KK",
		ip:            "192.168.000.000",
		port:          ":8080",
	},
}

type Router struct {
	BaseServer
}

var router Server = &Router{
	BaseServer: BaseServer{
		// MAC -> conn
		tableCAM:      make(map[string]net.Conn),
		inboundClient: make(chan *Client, 100),
		mac:           "MM:MM:MM:MM:MM:MM",
		ip:            "127.000.000.001",
		port:          ":9000",
	},
}

// IP -> MAC
var tableARP = make(map[string]string)
var response = make(chan Package)

type natEntry struct {
	ClientIP string
	Conn     net.Conn
}

var natTable = struct {
	sync.RWMutex
	m map[string]natEntry
}{m: make(map[string]natEntry)}

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

func listen(s Server) error {
	ln, err := net.Listen("tcp", s.getPort())
	if err != nil {
		return fmt.Errorf("ошибка запуска сервера по адресу %s:\n %w", s.getPort(), err)
	}
	fmt.Println("Сервер запущен на ", s.getPort())
	for {
		var client Client
		var err error
		client.conn, err = ln.Accept()
		if err != nil {
			fmt.Println("Ошибка принятия соединения: ", err)
			continue
		}

		client.port, err = getClientPort(client.conn)
		if err != nil {
			fmt.Println(err)
			continue
		}
		fmt.Println(s.getPort() + ": Подключение на порту " + client.port)

		s.getTableCAM()[client.port] = client.conn
		go handleClientConnection(&client, s)
	}
}

func getClientPort(conn net.Conn) (string, error) {
	addr := conn.RemoteAddr()
	_, port, err := net.SplitHostPort(addr.String())
	if err != nil {
		return "", fmt.Errorf("ошибка разделения адреса:%w", err)
	}
	return port, nil
}

func handleClientConnection(client *Client, s Server) {
	defer (*client).conn.Close()

	processClientRequests(client, s)

	err := unregisterClient(client, s)
	if err != nil {
		fmt.Println(err)
	}
}

func processClientRequests(client *Client, s Server) {
	reader := bufio.NewReader(client.conn)

	for {
		message, err := reader.ReadString('\n')
		if err != nil {
			break
		}
		client.pkg, err = getPackage(message)
		if err != nil {
			fmt.Printf("\n%s: %s: Ошибка обработки пакета: %v\n%s\n", s.getPort(), client.port, err, message)
			continue
		}
		fmt.Printf("\n%s: %s: Получен пакет:\n%s\n", s.getPort(), client.port, message)

		addInCAM(client.conn, s, client.port, client.pkg.sourceMAC)

		s.processRequest(client)
	}
}

func unregisterClient(client *Client, s Server) error {
	s.getMutex().Lock()

	for clientMAC, clientConn := range s.getTableCAM() {
		if clientConn == client.conn {
			delete(s.getTableCAM(), clientMAC)
			fmt.Println(client.port, ": Удалён: ", clientMAC)
			s.getMutex().Unlock()
			return nil
		}
	}
	s.getMutex().Unlock()
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

func addInCAM(senderConn net.Conn, s Server, senderPort, senderMac string) {
	s.getMutex().Lock()
	defer s.getMutex().Unlock()

	if _, ok := s.getTableCAM()[senderMac]; !ok {
		found := false
		for port, clientN := range s.getTableCAM() {
			if clientN == senderConn {
				delete(s.getTableCAM(), port)
				fmt.Println(senderPort+": Удалён:", port)
				s.getTableCAM()[senderMac] = senderConn
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
}

func isLocalIP(ip string) bool {
	return ip == "0.0.0.0" || strings.HasPrefix(ip, "192.168.")
}

func (s *Switcher) processRequest(client *Client) {
	s.inboundClient <- client
}

func (s *Router) processRequest(client *Client) {
	if !isLocalIP(client.pkg.destinationIP) {
		return
	}

	var recipientClient Client
	var err error
	recipientClient.conn, err = reverseNAT(&client.pkg)
	if err != nil {
		fmt.Println(err)
		return
	}
	recipientClient.pkg = client.pkg
	recipientClient.port, err = getClientPort(recipientClient.conn)
	if err != nil {
		fmt.Println(err)
	}

	switcher.getInboundClient() <- &recipientClient
}

func switchRequest() {
	for {
		clientSender := <-switcher.getInboundClient()

		if clientSender.pkg.destinationMAC != router.getMAC() {
			router.getInboundClient() <- clientSender
			fmt.Printf("Пакет \n%s\n перенаправлен маршрутизатору", packageToString(clientSender.pkg))
			continue
		}

		switcher.getMutex().Lock()

		pkg := clientSender.pkg

		message := packageToString(pkg)
		if pkg.destinationMAC == "FF:FF:FF:FF:FF:FF" {
			if err := processArp(clientSender.conn, pkg); err != nil {
				fmt.Println(err)
			}

		} else if clientRecipient, ok := switcher.getTableCAM()[pkg.destinationMAC]; ok {
			if err := switchPkg(clientRecipient, pkg); err != nil {
				fmt.Println(err)
			}

		} else {
			fmt.Println("\n" + clientSender.port + ": MAC-адрес " + pkg.destinationMAC + " не найден в сети.\n" + message)
			err := sendMessage(clientSender.conn, pkg.sourceMAC, protocolTypeToChar["UDP"], pkg.sourceIP, "\nMAC-адрес "+pkg.destinationMAC+" не найден в сети.")
			if err != nil {
				fmt.Println(err)
			}
		}

		switcher.getMutex().Unlock()
	}
}

func processArp(senderConn net.Conn, pkg Package) error {
	for _, clientN := range switcher.getTableCAM() {
		if clientN != senderConn {
			err := sendPackage(clientN, pkg)
			if err != nil {
				return err
			}
			portRecipient, err := getClientPort(clientN)
			if err != nil {
				return err
			}
			fmt.Println("Пакет перенаправлен клиенту " + portRecipient)
		}
	}
	router.getInboundClient() <- &Client{pkg: pkg, conn: senderConn, port: "", ms: ""}
	fmt.Println("Пакет перенаправлен маршрутизатору")
	return nil
}

func switchPkg(recipientConn net.Conn, pkg Package) error {
	err := sendPackage(recipientConn, pkg)
	if err != nil {
		return err
	}
	port, _ := getClientPort(recipientConn)
	fmt.Println("Пакет перенаправлен клиенту " + port)
	return nil
}

func routerRequest() {
	for {
		client := <-router.getInboundClient()
		router.getMutex().Lock()

		pkg := client.pkg

		if pkg.protocolType == "ARP" {
			if pkg.Operation == "RES" {
				response <- client.pkg
			} else if pkg.Operation == "REQ" {
				if pkg.destinationIP == router.getIP() {
					fmt.Printf(sendMessage(protocolTypeToChar["ARP"], pkg.sourceMAC, client.MACAddress, pkg.sourceIP, client.IPAddress, "RES"))
				}
			}

			return
		}

		var err error
		destMac, ok := tableARP[pkg.destinationIP]
		if !ok {
			destMac, err = getMacAddress(pkg.destinationIP)
			if err != nil {
				fmt.Println(err)
				continue
			}
		}
		conn, ok := router.getTableCAM()[destMac]
		if !ok {
			fmt.Println("Собеседника с таким МАК нет(")
			continue
		}

		applyNAT(pkg, conn, destMac)
		err = sendPackage(conn, pkg)
		if err != nil {
			fmt.Println("Ошибка при отправке в интернет:", err)
		}

		router.getMutex().Unlock()
	}
}

func getMacAddress(destinationIP string) (string, error) {
	if mac, ok := tableARP[destinationIP]; ok {
		return mac, nil
	}

	for _, conn := range router.getTableCAM() {
		if err := sendMessage(conn,
			"FF:FF:FF:FF:FF:FF",
			protocolTypeToChar["ARP"],
			destinationIP,
			"REQ"); err != nil {
			return "", err
		}
	}

	pkg, err := waitForResponse("ARP", router.getIP(), 2*time.Second)
	if err != nil {
		return "", err
	}
	if pkg.Operation != "RES" {
		return "", fmt.Errorf("получен не ARP RESPONSE")
	}

	tableARP[destinationIP] = pkg.sourceMAC
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

func applyNAT(pkg Package, conn net.Conn, destinationMAC string) {
	if isLocalIP(pkg.sourceIP) && !isLocalIP(pkg.destinationIP) {
		natTable.Lock()
		defer natTable.Unlock()
		natTable.m[pkg.destinationIP] = natEntry{
			ClientIP: pkg.sourceIP,
			Conn:     conn,
		}

		pkg.sourceIP = router.getIP()
		pkg.sourceMAC = router.getMAC()
		pkg.destinationMAC = destinationMAC
	}
}

func reverseNAT(pkg *Package) (net.Conn, error) {
	if pkg.destinationMAC != router.getMAC() {
		return nil, fmt.Errorf("МАК получателя не совпадает с МАК маршрутизатора\n")
	}

	natTable.RLock()
	entry, ok := natTable.m[pkg.sourceIP]
	natTable.RUnlock()

	if !ok {
		return nil, fmt.Errorf("собеседник для %s в этой локальной сети не найден\n", pkg.sourceIP)
	}

	var err error
	pkg.destinationIP = entry.ClientIP
	pkg.destinationMAC, err = getMacAddress(pkg.destinationIP)
	pkg.sourceMAC = router.getMAC()
	return entry.Conn, err
}

func sendMessage(conn net.Conn, destinationMAC, protocolType, destinationIP, data string) error {
	if charProtocolType, ok := protocolTypeToChar[protocolType]; ok {
		protocolType = charProtocolType
	}
	message := destinationMAC + router.getMAC() + protocolType + destinationIP + router.getIP() + data
	_, err := conn.Write([]byte(message + "\n"))
	if err != nil {
		return fmt.Errorf("sendMessage:\nошибка отправки пакета: %s\n%w", message, err)
	}
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
	return nil
}

func packageToString(pkg Package) string {
	return pkg.destinationMAC + pkg.sourceMAC + pkg.protocolType + pkg.destinationIP + pkg.sourceIP + pkg.Operation + pkg.data
}

func main() {
	errChan := make(chan error)

	go func() {
		errChan <- listen(switcher)
	}()
	go func() {
		errChan <- listen(router)
	}()

	go func() {
		switchRequest()
	}()
	go func() {
		routerRequest()
	}()

	err := <-errChan
	if err != nil {
		fmt.Println("Ошибка:", err)
	}
}
