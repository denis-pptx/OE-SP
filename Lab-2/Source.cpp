#include <windows.h>
#include <string>
using namespace std;


#include <string>

LRESULT CALLBACK MainWindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

RECT reduceRect(RECT rect, const double alpha) {
    RECT result;

    // Исходные размеры
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    // Новые размеры
    int newWidth = (int)(width * alpha);
    int newHeight = (int)(height * alpha);

    // Смещение по X и Y для центрирования уменьшенного прямоугольника
    int xOffset = (width - newWidth) / 2;
    int yOffset = (height - newHeight) / 2;

    // Новые координаты всех границ прямоугольника
    result.left = rect.left + xOffset;
    result.top = rect.top + yOffset;
    result.right = result.left + newWidth;
    result.bottom = result.top + newHeight;

    return result;
}

void DrawClock(HDC hdc, RECT clientRect, int hour, int minute, int second) {
    // Вычисление в соответствии с размером окна
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;

    int diameter = min(width, height);

    // Отрисовка эллписа
    int penWidth = diameter / 100;
    HPEN hPen = CreatePen(PS_SOLID, penWidth, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    Ellipse(hdc, clientRect.left, clientRect.top, clientRect.left + diameter, clientRect.top + diameter);

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

void UpdateClock(HWND hWnd) {
    HDC hdc = GetDC(hWnd);

    RECT clientRect;
    GetClientRect(hWnd, &clientRect);

    // Закрашиваем всю область окна фоновым цветом
    HBRUSH bgBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
    FillRect(hdc, &clientRect, bgBrush);
    DeleteObject(bgBrush);

    DrawClock(hdc, reduceRect(clientRect, 0.9), 0, 0, 0);

    ReleaseDC(hWnd, hdc);
}


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR cmdline, int ss) {

    // Создание класса главного окна
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

    // Создание главного окна
    HWND hw = CreateWindow(
        L"MainWindowClass", L"Clock",
        WS_OVERLAPPEDWINDOW,
        0, 0, 500, 500,
        NULL, NULL, hInst, NULL
    );

    if (!hw)
        return EXIT_FAILURE;

    ShowWindow(hw, ss);

    // Цикл обработки сообщений
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}






// Функция обработки сообщений
LRESULT CALLBACK MainWindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg)
    {
    case WM_SIZE:
    {
        UpdateClock(hWnd);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}