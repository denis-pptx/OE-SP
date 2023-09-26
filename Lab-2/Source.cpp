#define _USE_MATH_DEFINES
#include <windows.h>
#include <string>
#include <string>
using namespace std;




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

void DrawClock(HDC hdc, RECT rect, int hour, int minute, int second) {
    // Вычисление в соответствии с размером окна
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    int diameter = min(width, height);
    int centerX = rect.left + diameter / 2;
    int centerY = rect.top + diameter / 2;


    // Отрисовка большого круга
    int penWidth = diameter / 100;
    HPEN hPen = CreatePen(PS_SOLID, penWidth, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    Ellipse(hdc, rect.left, rect.top, rect.left + diameter, rect.top + diameter);


    // Установка размера шрифта в зависимости от диаметра
    int fontSize = diameter / 10; 
    HFONT hFont = CreateFont(fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Arial");
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    // Рисование цифр
    for (int i = 1; i <= 12; i++) {
        // Расчет координаты (x, y) для цифры
        double angle = i * (360.0 / 12.0);
        double radian = angle * M_PI / 180.0;

        int textX = centerX + ((diameter / 2) * sin(radian));
        int textY = centerY - ((diameter / 2) * cos(radian));

        // Создание кисти для заливки окружности
        HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush); 

        // Отрисовка окружности для цифры
        int radius = diameter * 0.08; 
        Ellipse(hdc, textX - radius, textY - radius, textX + radius, textY + radius); 

        // Восстановка предыдущей кисти
        SelectObject(hdc, hOldBrush); 
        DeleteObject(hBrush); 

        // Отрисовка цифры
        SetBkMode(hdc, TRANSPARENT);

        wstring digit = to_wstring(i);

        SIZE textSize;
        GetTextExtentPoint32W(hdc, digit.c_str(), digit.length(), &textSize);

        TextOutW(hdc, textX - textSize.cx / 2, textY - textSize.cy / 2, digit.c_str(), digit.length());

        SetBkMode(hdc, OPAQUE);
    }

    // Восстановление предыдущего пера
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);

    // Восстановление предыдущего шрифта
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);


}

void UpdateClock(HWND hWnd) {
    HDC hdc = GetDC(hWnd);

    RECT clientRect;
    GetClientRect(hWnd, &clientRect);

    // Закрашиваем всю область окна фоновым цветом
    HBRUSH bgBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
    FillRect(hdc, &clientRect, bgBrush);
    DeleteObject(bgBrush);

    DrawClock(hdc, reduceRect(clientRect, 0.8), 13, 5, 15);

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
    case WM_GETMINMAXINFO: {
        MINMAXINFO* minsize = (MINMAXINFO*)lParam;
        minsize->ptMinTrackSize.x = 400;
        minsize->ptMinTrackSize.y = 400;
        break;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}