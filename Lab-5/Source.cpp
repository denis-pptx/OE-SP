#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <string>
#include <string>
#include <ctime>
#include <map>
#include <thread>

#include "Registry.h"
#include "Constants.h"
#include "FileMapping.h"
using namespace std;


int utcOffset = DEFAULT_TIMEZONE;
HHOOK keyboardHook;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MainWindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void PlaySoundAsync(wstring soundFile);

RECT reduceRect(RECT rect, const double alpha);
void DrawClock(HDC hdc, RECT rect, int hour, int minute, int second);
void DisplayTimeZone(HWND hWnd);
void UpdateClock(HWND hWnd);


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR cmdline, int ss) {
    InitMapping();
    
    utcOffset = LoadTimeZoneFromRegistry();

    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInst, 0);
  
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
    HWND hWnd = CreateWindow(
        L"MainWindowClass", L"Clock",
        WS_OVERLAPPEDWINDOW,
        0, 0, 500, 500,
        NULL, NULL, hInst, NULL
    );

    if (!hWnd)
        return EXIT_FAILURE;

    ShowWindow(hWnd, ss);
    SetTimer(hWnd, NULL, 1000, NULL);

    // Цикл обработки сообщений
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(keyboardHook);
    CloseMapping();

    return msg.wParam;
}


LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
     if (nCode >= 0) {
        if (wParam == WM_KEYDOWN) {
            KBDLLHOOKSTRUCT* pKeyInfo = (KBDLLHOOKSTRUCT*)(lParam);
            if (pKeyInfo->vkCode == VK_SPACE) {
                thread soundThread(PlaySoundAsync, L"audio/kukushka.wav");
                soundThread.detach();
                WriteLog("Кукушка");
            }
        }
    }
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}


// Функция обработки сообщений
LRESULT CALLBACK MainWindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg)
    {
    case WM_CREATE:
    {
        thread soundThread(PlaySoundAsync, L"audio/start-windows.wav");
        soundThread.detach();
        break;
    }
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
    case WM_TIMER: {
        UpdateClock(hWnd);
        break;
    }
    case WM_KEYDOWN:
        switch (wParam) {
        case VK_LEFT:
            if (utcOffset <= -11) {
                MessageBox(NULL, L"Minimum time zone: UTC-11", L"Warning", MB_ICONWARNING | MB_OK);
            }
            else {
                utcOffset -= 1;
                UpdateClock(hWnd);
                
                SaveTimeZoneToRegistry(utcOffset);

                string log = string("Timezone decreased from UTC") + to_string(utcOffset) + " to UTC" + to_string(utcOffset - 1);
                thread t(WriteLog, log);
                t.detach();
            }
            break;

        case VK_RIGHT:
            if (utcOffset >= 12) {
                MessageBox(NULL, L"Maximum time zone: UTC+12", L"Warning", MB_ICONWARNING | MB_OK);
            }
            else {
                utcOffset += 1;
                UpdateClock(hWnd);
                
                SaveTimeZoneToRegistry(utcOffset);

                string log = string("Timezone increased from UTC") + to_string(utcOffset - 1) + " to UTC" + to_string(utcOffset);
                thread t(WriteLog, log);
                t.detach();
            }
            break;
        }
        break;
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void PlaySoundAsync(wstring soundFile) {
    PlaySound(soundFile.c_str(), NULL, SND_FILENAME);
}

// Уменьшить прямоугольик отрисовки
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

// Отрисовка часов
void DrawClock(HDC hdc, RECT rect, int hour, int minute, int second) {
    // Вычисление в соответствии с размером окна
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    int diameter = min(width, height);
    int radius = diameter / 2;
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

        int textX = centerX + (radius * sin(radian));
        int textY = centerY - (radius * cos(radian));

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
        SetTextColor(hdc, RGB((i * 25) % 256, (i * 45) % 256, (i * 65) % 256));
        TextOutW(hdc, textX - textSize.cx / 2, textY - textSize.cy / 2, digit.c_str(), digit.length());

        SetBkMode(hdc, OPAQUE);
    }

    // Восстановление предыдущего пера
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);

    // Восстановление предыдущего шрифта
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);

    // Отрисовка часовой стрелки
    int hourAngle = (hour % 12) * 30 + (minute / 2);
    int hourLength = radius * 0.25;
    int hx = centerX + hourLength * sin(hourAngle * M_PI / 180.0);
    int hy = centerY - hourLength * cos(hourAngle * M_PI / 180.0);

    int hourPenWidth = diameter / 100;
    HPEN hHourPen = CreatePen(PS_SOLID, hourPenWidth, RGB(0, 0, 0));
    hOldPen = (HPEN)SelectObject(hdc, hHourPen);

    MoveToEx(hdc, centerX, centerY, NULL);
    LineTo(hdc, hx, hy);

    SelectObject(hdc, hOldPen);
    DeleteObject(hHourPen);


    // Отрисовка минутной стрелки
    int minuteAngle = minute * 6 + (second / 10);
    int minuteLength = radius * 0.45;
    int mx = centerX + minuteLength * sin(minuteAngle * M_PI / 180.0);
    int my = centerY - minuteLength * cos(minuteAngle * M_PI / 180.0);

    int minutePenWidth = diameter / 150;
    HPEN hMinutePen = CreatePen(PS_SOLID, minutePenWidth, RGB(128, 128, 128));
    hOldPen = (HPEN)SelectObject(hdc, hMinutePen);

    MoveToEx(hdc, centerX, centerY, NULL);
    LineTo(hdc, mx, my);

    SelectObject(hdc, hOldPen);
    DeleteObject(hMinutePen);


    // Отрисовка секунднрй стрелки
    int secondAngle = second * 6;
    int secondLength = radius * 0.55;
    int sx = centerX + secondLength * sin(secondAngle * M_PI / 180.0);
    int sy = centerY - secondLength * cos(secondAngle * M_PI / 180.0);

    int secondPenWidth = diameter / 200;
    HPEN hSecondPen = CreatePen(PS_SOLID, secondPenWidth, RGB(255, 0, 0));
    hOldPen = (HPEN)SelectObject(hdc, hSecondPen);

    MoveToEx(hdc, centerX, centerY, NULL);
    LineTo(hdc, sx, sy);

    SelectObject(hdc, hOldPen);
    DeleteObject(hSecondPen);


    // Отрисовка круга в центре часов
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    int r = radius / 30;
    Ellipse(hdc, centerX - r, centerY - r, centerX + r, centerY + r);

    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);
}


// Отображение тайм зоны (в title окна)
void DisplayTimeZone(HWND hWnd) {

    wstring new_title = BASE_TITLE;
    if (utcOffset < 0)
        new_title += to_wstring(utcOffset);
    else if (utcOffset > 0)
        new_title += L"+" + to_wstring(utcOffset);

    new_title += L" | " + TIME_ZONE_MAP[utcOffset];
    SetWindowText(hWnd, new_title.c_str());
}


// Обновить часы
void UpdateClock(HWND hWnd) {
    HDC hdc = GetDC(hWnd);

    RECT clientRect;
    GetClientRect(hWnd, &clientRect);

    // Закрашиваем всю область окна фоновым цветом
    HBRUSH bgBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
    FillRect(hdc, &clientRect, bgBrush);
    DeleteObject(bgBrush);

    // Текущее UTC время
    time_t now = time(0);
    tm* gmtm = gmtime(&now);

    // Отрисовка часов
    DrawClock(hdc, reduceRect(clientRect, 0.8), gmtm->tm_hour + utcOffset, gmtm->tm_min, gmtm->tm_sec);

    // Вывод пояса в заголовок окна
    DisplayTimeZone(hWnd);

    ReleaseDC(hWnd, hdc);
}
