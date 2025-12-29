package main

import (
	"encoding/binary"
	"fmt"
	"io"
	"net"
	"os"
	"path/filepath"
	"strconv"
	"sync"
	"time"
)

//docker-compose up -d server
//docker-compose run client /app/client /app/files/test1.txt localhost 8080

// docker stop lab2_server, docker-compose down

//docker ps          # показать запущенные контейнеры
//docker-compose ps  # показать контейнеры этого проекта

//docker-compose restart server

type ClientConnection struct {
	conn     net.Conn
	clientID int
}

// Статистика загрузки.
type UploadStats struct {
	startTime    time.Time
	lastUpdate   time.Time
	lastBytes    int
	totalBytes   int
	instantSpeed float64
	averageSpeed float64
	mutex        sync.Mutex
}

func monitorSpeed(stats *UploadStats, clientID int, done chan bool) {
	ticker := time.NewTicker(3 * time.Second)
	defer ticker.Stop()

	for {
		select {
		case <-ticker.C:
			stats.mutex.Lock()

			now := time.Now()
			timeDiff := now.Sub(stats.lastUpdate).Seconds()

			if timeDiff > 0 {
				bytesDiff := stats.totalBytes - stats.lastBytes
				stats.instantSpeed = float64(bytesDiff) / timeDiff

				totalTime := now.Sub(stats.startTime).Seconds()
				if totalTime > 0 {
					stats.averageSpeed = float64(stats.totalBytes) / totalTime
				}

				fmt.Printf("Клиент #%d: мгновенная %.2f KB/s, средняя %.2f KB/s, получено %d байт\n",
					clientID, stats.instantSpeed/1024, stats.averageSpeed/1024, stats.totalBytes)

				stats.lastUpdate = now
				stats.lastBytes = stats.totalBytes
			}

			stats.mutex.Unlock()
		case <-done:
			fmt.Printf("Клиент #%d: мониторинг скорости остановлен\n", clientID)
			return
		}
	}
}

func main() {
	if len(os.Args) != 2 {
		fmt.Println("server.go <порт>")
		os.Exit(1)
	}

	port := os.Args[1]

	// директория для загрузок
	err := os.MkdirAll("uploads", 0755)
	if err != nil {
		fmt.Printf("Ошибка создания директории uploads: %v\n", err)
		os.Exit(1)
	}

	startServer(port)
}

func startServer(port string) {
	listener, err := net.Listen("tcp", ":"+port)
	if err != nil {
		fmt.Printf("Ошибка запуска сервера: %v\n", err)
		os.Exit(1)
	}
	defer listener.Close()

	fmt.Printf("Сервер запущен на порту %s\n", port)
	fmt.Println("Ожидание подключений...")

	clientCounter := 0
	var mutex sync.Mutex

	for {
		conn, err := listener.Accept()
		if err != nil {
			fmt.Printf("Ошибка принятия соединения: %v\n", err)
			continue
		}

		mutex.Lock()
		clientCounter++
		clientID := clientCounter
		mutex.Unlock()

		fmt.Printf("Клиент #%d подключен: %s\n", clientID, conn.RemoteAddr())

		go handleClient(ClientConnection{
			conn:     conn,
			clientID: clientID,
		})
	}
}

func handleClient(client ClientConnection) {
	defer client.conn.Close()

	stats := &UploadStats{
		startTime:  time.Now(),
		lastUpdate: time.Now(),
	}

	done := make(chan bool)
	defer close(done)
	go monitorSpeed(stats, client.clientID, done)

	fileName, fileSize, err := receiveFileMetadata(client.conn)
	if err != nil {
		fmt.Printf("Клиент #%d: ошибка получения метаданных: %v\n", client.clientID, err)
		done <- true
		return
	}

	fmt.Printf("Клиент #%d: получаем файл '%s' размером %d байт\n",
		client.clientID, fileName, fileSize)

	receivedSize, err := receiveFileContent(client.conn, fileName, fileSize, stats)
	if err != nil {
		fmt.Printf("Клиент #%d: ошибка получения файла: %v\n", client.clientID, err)
		sendResult(client.conn, false)
		done <- true
		return
	}

	success := receivedSize == fileSize
	if success {
		fmt.Printf("Клиент #%d: файл успешно получен (%d байт)\n", client.clientID, receivedSize)
	} else {
		fmt.Printf("Клиент #%d: ошибка - получено %d байт, ожидалось %d байт\n",
			client.clientID, receivedSize, fileSize)
	}

	sendResult(client.conn, success)
	done <- true
}

func receiveFileMetadata(conn net.Conn) (string, int, error) {
	var fileNameBytes []byte
	buffer := make([]byte, 1)

	for {
		_, err := conn.Read(buffer)
		if err != nil {
			return "", 0, err
		}

		if buffer[0] == '|' {
			break
		}
		fileNameBytes = append(fileNameBytes, buffer[0])
	}

	fileName := string(fileNameBytes)

	var fileSizeBytes []byte
	for {
		_, err := conn.Read(buffer)
		if err != nil {
			return "", 0, err
		}

		if buffer[0] == '\n' || buffer[0] == ' ' {
			break
		}
		fileSizeBytes = append(fileSizeBytes, buffer[0])
	}

	fileSize, err := strconv.Atoi(string(fileSizeBytes))
	if err != nil {
		return "", 0, err
	}

	return fileName, fileSize, nil
}

func receiveFileContent(conn net.Conn, fileName string, expectedSize int, stats *UploadStats) (int, error) {
	filePath := filepath.Join("uploads", fileName)
	file, err := os.Create(filePath)
	if err != nil {
		return 0, err
	}
	defer file.Close()

	buffer := make([]byte, 32*1024)
	var totalReceived int

	for {
		n, err := conn.Read(buffer)
		if n > 0 {
			_, writeErr := file.Write(buffer[:n])
			if writeErr != nil {
				return totalReceived, writeErr
			}

			totalReceived += n

			stats.mutex.Lock()
			stats.totalBytes = totalReceived
			stats.mutex.Unlock()
		}

		if err != nil {
			if err == io.EOF {
				break
			}
			return totalReceived, err
		}

		if totalReceived >= expectedSize {
			break
		}
	}

	return totalReceived, nil
}

func sendResult(conn net.Conn, success bool) error {
	// 1 = успех, 0 = неудача
	var result byte = 0
	if success {
		result = 1
	}
	return binary.Write(conn, binary.BigEndian, result)
}
