#include <WinSock2.h>
#include <iostream>
#include <vector>
#include <thread>
#include <string>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define DEFAULT_PORT 12345

vector<SOCKET> clients;

int getUniqueNumber() {
    static int number = 1;
    return number++;
}

// Обработка сообщений от клиента
void HandleClient(const SOCKET& clientSocket) {
    int clientNumber = getUniqueNumber();

    cout << "Client #" << clientNumber << " connected." << endl;
    cout << "Total clients: " << clients.size() << endl;

    while (true) {

        char buffer[1000];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesRead <= 0) {
            cout << "Client #" << clientNumber << " disconnected.\n";

            // Удалить сокет из вектора
            auto it = find(clients.begin(), clients.end(), clientSocket);
            if (it != clients.end()) 
                clients.erase(it);
            
            // Закрыть сокет
            closesocket(clientSocket);

            return;
        }

        buffer[bytesRead] = '\0';
        cout << "Message from client #" << clientNumber << ": " << buffer << endl;

        // Рассылка сообщения всем клиентам
        for (size_t i = 0; i < clients.size(); i++) {
            const SOCKET& otherClient = clients[i];

            int bytesSent = send(otherClient, buffer, bytesRead, 0);

            if (bytesSent == SOCKET_ERROR) 
                cout << "Error sending message to client #" << i << ".\n";
        }

    }
}


int main() {
    setlocale(LC_ALL, "Ru");

    WSADATA wsaData;
    // Инициализация библиотеки Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "Failed to initialize Winsock." << endl;
        return 1;
    }

    // Создание сокета
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET) {
        cout << "Failed to create socket." << endl;
        WSACleanup();
        return 1;
    }

    // Настройка параметров сервера
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(DEFAULT_PORT);

    // Привязка сокета
    if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Failed to bind socket." << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Ожидание подключений
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        cout << "Error while listening for connections." << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    cout << "Server started. Listening on port " << DEFAULT_PORT << endl;

    while (true) {
        // Принятие подключения
        SOCKET clientSocket = accept(listenSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            cout << "Failed to accept client connection." << endl;
            continue;
        }

        // Добавление клиента в вектор
        clients.push_back(clientSocket);

        // Создание нового потока для обработки клиента
        thread(HandleClient, clientSocket).detach();
    }

    // Закрытие сокета и очистка Winsock
    closesocket(listenSocket);
    WSACleanup();

    return 0;
}
