#include <Windows.h>
#include <string>
#include <thread>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define DEFAULT_PORT 11111
#define DEFAULT_IP "127.0.0.1"
#define ID_BUTTON_SEND 1
#define ID_BUTTON_CLEAR 2

HWND hNameEdit, hMessageEdit, hListBox;
SOCKET clientSocket;

void HandleMessages() {
    while (true) {
        char buffer[1000];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesRead <= 0) {
            MessageBox(NULL, L"Error reading messages or server disconnected.", L"Error", MB_OK | MB_ICONERROR);
            closesocket(clientSocket);
            exit(1);
        }

        buffer[bytesRead] = '\0';
        SendMessageA(hListBox, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(buffer));
    }
}

void ConnectToServer() {
    WSADATA wsaData;
    // Инициализация библиотеки Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        MessageBox(NULL, L"Failed to initialize Winsock.", L"Error", MB_OK | MB_ICONERROR);
        exit(1);
    }

    // Создание сокета
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        MessageBox(NULL, L"Failed to create socket.", L"Error", MB_OK | MB_ICONERROR);
        WSACleanup();
        exit(1);
    }

    // Настройка параметров сервера
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(DEFAULT_IP); // Преобразование IP-адреса из строки
    serverAddr.sin_port = htons(DEFAULT_PORT); // host to network short

    
    if (serverAddr.sin_addr.s_addr == INADDR_NONE) {
        MessageBox(NULL, L"Invalid server IP address.", L"Error", MB_OK | MB_ICONERROR);
        closesocket(clientSocket);
        WSACleanup();
        exit(1);
    }

    // Подключение к серверу
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        MessageBox(NULL, L"Failed to connect to the server.", L"Error", MB_OK | MB_ICONERROR);
        closesocket(clientSocket);
        WSACleanup();
        exit(1);
    }

    // Запуск потока для приема сообщений от сервера
    thread(HandleMessages).detach();
}


void SendButtonClicked() {
    char name[100], message[500];
    GetWindowTextA(hNameEdit, name, sizeof(name) / sizeof(name[0]));
    GetWindowTextA(hMessageEdit, message, sizeof(message) / sizeof(message[0]));

    string fullMessage = name;
    fullMessage += ": ";
    fullMessage += message;

    // Отправка сообщения на сервер
    if (send(clientSocket, fullMessage.c_str(), fullMessage.size(), 0) == SOCKET_ERROR)
        MessageBox(NULL, L"Failed to send message to the server.", L"Error", MB_OK | MB_ICONERROR);
    
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:

        CreateWindowEx(
            0, L"STATIC", L"Name:",
            WS_CHILD | WS_VISIBLE,
            10, 10, 70, 25,
            hwnd, NULL, NULL, NULL
        );
        hNameEdit = CreateWindowEx(
            0, L"EDIT", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            80, 10, 120, 25,
            hwnd, NULL, NULL, NULL
        );
        SendMessage(hNameEdit, EM_LIMITTEXT, 10, 0);

        CreateWindowEx(
            0, L"STATIC", L"Message:",
            WS_CHILD | WS_VISIBLE,
            10, 40, 70, 25,
            hwnd, NULL, NULL, NULL
        );
        hMessageEdit = CreateWindowEx(
            0, L"EDIT", NULL, 
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            80, 40, 250, 25, 
            hwnd, NULL, NULL, NULL
        );
        SendMessage(hMessageEdit, EM_LIMITTEXT, 40, 0);
        CreateWindowEx(
            0, L"BUTTON", L"Send", 
            WS_CHILD | WS_VISIBLE,
            340, 40, 80, 25,
            hwnd, reinterpret_cast<HMENU>(ID_BUTTON_SEND), NULL, NULL);


        hListBox = CreateWindowEx(
            0, L"LISTBOX", NULL,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL,
            10, 70, 410, 205,
            hwnd, NULL, NULL, NULL
        );

        CreateWindowEx(
            0, L"BUTTON", L"Clear",
            WS_CHILD | WS_VISIBLE,
            340, 280, 80, 25,
            hwnd, reinterpret_cast<HMENU>(ID_BUTTON_CLEAR), NULL, NULL
        );

        
        // Подключение к серверу
        ConnectToServer();
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == ID_BUTTON_SEND && HIWORD(wParam) == BN_CLICKED) {
            SendButtonClicked();
            SetWindowTextA(hMessageEdit, "");
        }
        else if (LOWORD(wParam) == ID_BUTTON_CLEAR && HIWORD(wParam) == BN_CLICKED) {
            SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
        }
        break;

    case WM_DESTROY:
        // Завершение работы с сокетом и Winsock
        closesocket(clientSocket);
        WSACleanup();
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR cmdline, int ss) {

    // Создание класса главного окна 
    WNDCLASS wc;
    wc.style = NULL;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = NULL;
    wc.cbWndExtra = NULL;
    wc.hInstance = hInst;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = L"MainWindowClass";
    if (!RegisterClass(&wc))
        return EXIT_FAILURE;

    // Создание главного окна
    HWND hWnd = CreateWindow(
        L"MainWindowClass", L"KonchikMessenger",
        WS_OVERLAPPEDWINDOW,
        0, 0, 450, 360,
        NULL, NULL, hInst, NULL
    );

    if (!hWnd)
        return EXIT_FAILURE;

    ShowWindow(hWnd, ss);

    // Цикл обработки сообщений
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
