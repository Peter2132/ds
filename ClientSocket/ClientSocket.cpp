#define WIN32_LEAN_AND_MEAN // Убирает ненужные заголовки из Windows.h для повышения производительности
#include <Windows.h> // Заголовок для WinAPI функций
#include <iostream> // Заголовок для работы с потоками ввода-вывода
#include <WinSock2.h> // Заголовок для Winsock 2, используемого для сетевого программирования
#include <WS2tcpip.h> // Заголовок для дополнительных функций Winsock 2
//Windows Sockets API 
using namespace std; 

int main() {
    WSADATA wsaData; // Структура для инициализации Winsock
    ADDRINFO hints; // Структура с параметрами для getaddrinfo
    ADDRINFO* addrResult; // Указатель на структуру с информацией об адресе
    SOCKET ConnectSocket = INVALID_SOCKET; // Дескриптор сокета, по умолчанию не валидный
    char recvBuffer[512]; // Буфер для приема данных

    const char* sendBuffer1 = "Hello from client 1"; // Текст первого сообщения
    const char* sendBuffer2 = "Hello from client 2"; // Текст второго сообщения

    // Создание Winsock для создания сетевых приложений
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); // Инициализируем Winsock с версией 2.2. Уведомляет операционную систему о том, что он собирается использовать сетевые возможности
    if (result != 0) {
        cout << "WSAStartup failed with result: " << result << endl;
        return 1; // Выход из программы с ошибкой
    }

    // Заполняем структуру hints для определения параметров поиска адресса getaddrinfo
    ZeroMemory(&hints, sizeof(hints)); // Обнуляем структуру hints 0
    hints.ai_family = AF_INET; // Семейство протоколов IPv4
    hints.ai_socktype = SOCK_STREAM; // Тип сокета TCP
    hints.ai_protocol = IPPROTO_TCP; // Протокол TCP

    // Получаем информацию об адресе
    result = getaddrinfo("localhost", "666", &hints, &addrResult); // Получаем адрес по имени узла "localhost" и порту 666
    if (result != 0) {
        cout << "getaddrinfo failed with error: " << result << endl; //Выводим сообщение об ошибке
        freeaddrinfo(addrResult); // Освобождаем память,которая выделенная для структуры адреса
        WSACleanup(); // Очищаем Winsock
        return 1; // Выход из программы
    }

    // Создаем сокет
    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol); // Создаем сокет с параметрами из структуры адреса
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Socket creation failed" << endl; //Выводим сообщение об ошибке
        freeaddrinfo(addrResult); // Освобождаем память,которая выделенная для структуры адреса
        WSACleanup(); // Очищаем Winsock
        return 1; // Выход из программы
    }

    // Подключаемся к серверу
    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen); // Подключаемся к серверу по адресу и длине адреса
    if (result == SOCKET_ERROR) {
        cout << "Unable to connect to server" << endl; //Выводим сообщение об ошибке
        closesocket(ConnectSocket); // Закрываем сокет
        ConnectSocket = INVALID_SOCKET; // Сбрасываем дескриптор сокета
        freeaddrinfo(addrResult); // Освобождаем память,которая выделенная для структуры адреса
        WSACleanup(); // Очищаем Winsock
        return 1; // Выход из программы
    }

    // Отправляем первое сообщение
    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0); // Отправляем сообщение на сервер
    if (result == SOCKET_ERROR) {
        cout << "Send failed, error: " << result << endl; //Выводим сообщение об ошибке
        closesocket(ConnectSocket); // Закрываем сокет
        freeaddrinfo(addrResult); // Освобождаем память,которая выделенная для структуры адреса
        WSACleanup(); // Очищаем Winsock
        return 1; // Выход из программы
    }
    cout << "Sent: " << result << " bytes" << endl; // Выводим количество отправленных байт

    // Отправляем второе сообщение
    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0); // Отправляем второе сообщение на сервер
    if (result == SOCKET_ERROR) {
        cout << "Send failed, error: " << result << endl; //Выводим сообщение об ошибке
        closesocket(ConnectSocket); // Закрываем сокет
        freeaddrinfo(addrResult); // Освобождаем память,которая выделенная для структуры адреса
        WSACleanup(); // Очищаем Winsock
        return 1; // Выход из программы
    }
    cout << "Sent: " << result << " bytes" << endl; // Выводим количество отправленных байт

    // Запрещаем отправку данных
    result = shutdown(ConnectSocket, SD_SEND); // Запрещаем отправку данных на сокет
    if (result == SOCKET_ERROR) {
        cout << "Shutdown failed, error: " << result << endl; //Выводим сообщение об ошибке
        closesocket(ConnectSocket); // Закрываем сокет
        freeaddrinfo(addrResult); // Освобождаем память,которая выделенная для структуры адреса
        WSACleanup(); // Очищаем Winsock
        return 1; // Выход из программы
    }

    // Принимаем данные
    do {
        ZeroMemory(recvBuffer, 512); // Обнуляем буфер приема
        result = recv(ConnectSocket, recvBuffer, 512, 0); // Принимаем данные с сервера
        if (result > 0) {
            cout << "Received " << result << " bytes" << endl; // Выводим количество полученных байт
            cout << "Received data: " << recvBuffer << endl; // Выводим полученные данные
        }
        else if (result == 0) {
            cout << "Connection closed" << endl; // Выводим сообщение о закрытии соединения
        }
        else {
            cout << "Recv failed, error: " << WSAGetLastError() << endl; // Выводим сообщение об ошибке
        }
    } while (result > 0); // Повторяем цикл, пока получаем данные

    
    closesocket(ConnectSocket); // Закрываем сокет
    freeaddrinfo(addrResult); // Освобождаем память,которая выделенная для структуры адреса
    WSACleanup(); // Очищаем Winsock
    return 0; // Выход из программы
}

