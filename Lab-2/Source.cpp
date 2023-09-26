#include <windows.h>
#include <string>
using namespace std;



LRESULT CALLBACK MainWindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int ss) {

    // �������� ������ �������� ����
    WNDCLASS wc;
    wc.style = NULL;
    wc.lpfnWndProc = MainWindowProcedure;
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

    // �������� �������� ����
    HWND hw = CreateWindow(
        L"MainWindowClass", L"����",
        WS_OVERLAPPEDWINDOW,
        0, 0, 370, 280,
        NULL, NULL, hInst, NULL
    );

    if (!hw)
        return EXIT_FAILURE;

    ShowWindow(hw, ss);

    // ���� ��������� ���������
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}






// ������� ��������� ���������
LRESULT CALLBACK MainWindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg)
    {
    case WM_CREATE:
    {
        
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}