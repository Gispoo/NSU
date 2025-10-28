package main

import (
	"encoding/binary"
	"fmt"
	"io"
	"net"
	"os"
	"path/filepath"
	"time"
)

func main() {
	if len(os.Args) != 4 {
		fmt.Println("client <файл> <IP> <порт>")
		os.Exit(1)
	}

	filePath := os.Args[1]
	ip := os.Args[2]
	port := os.Args[3]

	err := sendFile(filePath, ip, port)
	if err != nil {
		fmt.Printf("Ошибка отправки файла: %v\n", err)
		os.Exit(1)
	}
}

func sendFile(filePath, ip, port string) error {
	conn, err := net.Dial("tcp", ip+":"+port)
	if err != nil {
		return err
	}
	defer conn.Close()

	fmt.Printf("Подключились к серверу %s\n", ip+":"+port)

	file, err := os.Open(filePath)
	if err != nil {
		return err
	}
	defer file.Close()

	fileInfo, err := file.Stat()
	if err != nil {
		return err
	}

	fileName := filepath.Base(filePath)
	fileSize := fileInfo.Size()

	fmt.Printf("Отправляем файл '%s' размером %d байт\n", fileName, fileSize)

	err = sendFileMetadata(conn, fileName, int(fileSize))
	if err != nil {
		return err
	}

	// Отправляем содержимое файла
	err = sendFileContent(conn, file)
	if err != nil {
		return err
	}

	// Получаем результат от сервера
	success, err := receiveResult(conn)
	if err != nil {
		return err
	}

	if success {
		fmt.Println("Файл успешно передан!")
	} else {
		fmt.Println("Ошибка: сервер не подтвердил получение файла")
	}

	return nil
}

func sendFileMetadata(conn net.Conn, fileName string, fileSize int) error {
	metadata := fmt.Sprintf("%s|%d\n", fileName, fileSize)
	_, err := conn.Write([]byte(metadata))
	return err
}

func sendFileContent(conn net.Conn, file *os.File) error {
	buffer := make([]byte, 32*1024)

	for {
		n, err := file.Read(buffer)
		if n > 0 {
			_, writeErr := conn.Write(buffer[:n])
			if writeErr != nil {
				return writeErr
			}
			time.Sleep(100 * time.Millisecond)
		}

		if err != nil {
			if err == io.EOF {
				break
			}
			return err
		}
	}

	return nil
}

func receiveResult(conn net.Conn) (bool, error) {
	var result byte
	err := binary.Read(conn, binary.BigEndian, &result)
	if err != nil {
		return false, err
	}

	return result == 1, nil
}
