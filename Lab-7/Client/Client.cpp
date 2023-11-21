#include <windows.h>


#pragma comment(lib, "ws2_32.lib") // �������� � ����������� ��� ������ � Winsock

#define IDC_EDIT   101
#define IDC_BUTTON 102

HWND hwndEdit;
HWND hwndButton;
SOCKET clientSocket;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // ������������� Winsock (���������� �������)
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        MessageBox(NULL, L"������ ��� ������������� Winsock.", L"������", MB_OK | MB_ICONERROR);
        return 1;
    }

    // �������� ������
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        MessageBox(NULL, L"������ ��� �������� ������.", L"������", MB_OK | MB_ICONERROR);
        WSACleanup();
        return 1;
    }

    // ������ �� ����������� � �������
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP-����� �������
    serverAddress.sin_port = htons(12345); // ���� �������

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        MessageBox(NULL, L"������ ��� ����������� � �������.", L"������", MB_OK | MB_ICONERROR);
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // �������� �������� ����
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = L"ChatApp";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, L"������ ��� ����������� ����!", L"������!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, L"ChatApp", L"Chat Client", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, L"������ ��� �������� ����!", L"������!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // �������� ���� ���������
    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    closesocket(clientSocket); // �������� ������ �������
    WSACleanup(); // ������� Winsock

    return Msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        hwndEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL, 10, 10, 350, 200, hwnd, (HMENU)IDC_EDIT, GetModuleHandle(NULL), NULL);
        hwndButton = CreateWindow(L"BUTTON", L"���������", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 220, 50, 25, hwnd, (HMENU)IDC_BUTTON, GetModuleHandle(NULL), NULL);
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_BUTTON) {
            char buffer[1024];
            GetWindowTextA(hwndEdit, buffer, sizeof(buffer));

            // �������� ������ �� ������
            send(clientSocket, buffer, strlen(buffer) * sizeof(char), 0);

            SetWindowText(hwndEdit, L""); // ������� ���������� ����
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}
