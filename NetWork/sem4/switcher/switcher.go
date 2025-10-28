package main

import (
	"bufio"
	"fmt"
	"net"
	"strings"
	"sync"
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
	ms   string
	conn net.Conn
	port string
}

type Switcher struct {
	// MAC -> conn
	tableCAM map[string]net.Conn
	mu       sync.RWMutex
	mac      string
	ip       string
	port     string
}

var switcher = &Switcher{
	tableCAM: make(map[string]net.Conn),
	mac:      "KK:KK:KK:KK:KK:KK",
	ip:       "192.168.000.000",
	port:     ":8080",
}

func listen() error {
	ln, err := net.Listen("tcp", switcher.port)
	if err != nil {
		return fmt.Errorf("ошибка запуска сервера по адресу %s:\n %w", switcher.port, err)
	}
	fmt.Println("Сервер запущен на ", switcher.port)
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
		fmt.Println(switcher.port + ": Подключение на порту " + client.port)

		switcher.tableCAM[client.port] = client.conn
		go handleClientConnection(&client)
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

func handleClientConnection(client *Client) {
	defer client.conn.Close()

	processClientRequests(client)

	err := unregisterClient(client)
	if err != nil {
		fmt.Println(err)
	}
}

func processClientRequests(client *Client) {
	reader := bufio.NewReader(client.conn)
	addedToTable := false
	for {
		message, err := reader.ReadString('\n')
		if err != nil {
			break
		}
		client.pkg, err = getPackage(message)
		if err != nil {
			fmt.Printf("\n%s: %s: Ошибка обработки пакета: %v\n%s\n", switcher.port, client.port, err, message)
			continue
		}
		fmt.Printf("\n%s: %s: Получен пакет:\n%s\n", switcher.port, client.port, message)

		if !addedToTable {
			addedToTable = addInCAM(client.conn, client.port, client.pkg.sourceMAC)
		}

		switchRequest(client)
	}
}

func unregisterClient(client *Client) error {
	switcher.mu.Lock()
	defer switcher.mu.Unlock()

	for clientMAC, clientConn := range switcher.tableCAM {
		if clientConn == client.conn {
			delete(switcher.tableCAM, clientMAC)
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
	switcher.mu.Lock()
	defer switcher.mu.Unlock()

	found := false
	if _, ok := switcher.tableCAM[senderMac]; !ok {
		for port, clientN := range switcher.tableCAM {
			if clientN == senderConn {
				delete(switcher.tableCAM, port)
				fmt.Println(senderPort+": Удалён:", port)
				switcher.tableCAM[senderMac] = senderConn
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

func switchRequest(clientSender *Client) {
	switcher.mu.Lock()

	pkg := clientSender.pkg

	message := packageToString(pkg)
	if pkg.destinationMAC == "FF:FF:FF:FF:FF:FF" {
		if err := processArp(clientSender.conn, pkg); err != nil {
			fmt.Println(err)
		}

	} else if clientRecipient, ok := switcher.tableCAM[pkg.destinationMAC]; ok {
		err := sendPackage(clientRecipient, pkg)
		if err != nil {
			fmt.Println(err)
		}

	} else {
		fmt.Println("\n" + clientSender.port + ": MAC-адрес " + pkg.destinationMAC + " не найден в сети.\n" + message)
		pkg.protocolType = "UDP"
		pkg.destinationIP = "MAC-адрес " + pkg.destinationMAC + " не найден в сети."
		err := sendPackage(clientSender.conn, pkg)
		if err != nil {
			fmt.Println(err)
		}
	}

	switcher.mu.Unlock()
}

func processArp(senderConn net.Conn, pkg Package) error {
	for _, clientN := range switcher.tableCAM {
		if clientN != senderConn {
			err := sendPackage(clientN, pkg)
			if err != nil {
				return err
			}
		}
	}

	return nil
}

func sendPackage(conn net.Conn, pkg Package) error {
	if charProtocolType, ok := protocolTypeToChar[pkg.protocolType]; ok {
		pkg.protocolType = charProtocolType
	}
	message := packageToString(pkg)
	n, err := conn.Write([]byte(message + "\n"))
	if err != nil {
		return fmt.Errorf("sendMessage:\nошибка отправки пакета: %s\n%w", message, err)
	}
	if n < offsetOperation {
		return fmt.Errorf("слишком короткое сообщение:\n<%s>\n", message)
	}
	//log.Printf("SEND: %q", message)
	port, _ := getClientPort(conn)
	fmt.Println("Пакет перенаправлен клиенту " + port)
	return nil
}

func packageToString(pkg Package) string {
	return pkg.destinationMAC + pkg.sourceMAC + pkg.protocolType + pkg.destinationIP + pkg.sourceIP + pkg.Operation + pkg.data
}

func main() {
	errChan := make(chan error)

	go func() {
		errChan <- listen()
	}()

	err := <-errChan
	if err != nil {
		fmt.Println("Ошибка:", err)
	}
}
