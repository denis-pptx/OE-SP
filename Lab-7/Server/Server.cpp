#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Линковка с библиотекой для работы с Winsock

#define PORT 12345
#define MAX_BUFFER_SIZE 1024

int main() {
    setlocale(LC_ALL, "Ru");
    // Инициализация Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Ошибка при инициализации Winsock.\n";
        return 1;
    }

    // Создание сокета
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Ошибка при создании сокета.\n";
        WSACleanup();
        return 1;
    }

    // Привязка сокета
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Ошибка при привязке сокета.\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Ожидание входящих подключений
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Ошибка при прослушивании сокета.\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Сервер слушает порт " << PORT << "...\n";

    // Принятие подключений от клиентов
    SOCKET clientSocket;
    sockaddr_in clientAddress;
    int clientAddressSize = sizeof(clientAddress);

    while (true) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Ошибка при принятии подключения клиента.\n";
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        std::cout << "Клиент подключен.\n";

        // Обработка обмена сообщениями с клиентом
        char buffer[MAX_BUFFER_SIZE];
        int bytesRead;

        do {
            bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesRead > 0) {
                buffer[bytesRead] = '\0'; // Добавляем нулевой символ в конец строки
                std::cout << "Получено от клиента: " << buffer << std::endl;

                // Реализуйте код для отправки ответа обратно клиенту
                send(clientSocket, buffer, bytesRead, 0);
            }
        } while (bytesRead > 0);

        std::cout << "Клиент отключен.\n";
        closesocket(clientSocket);
    }

    // Очистка ресурсов
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
