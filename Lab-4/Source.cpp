#include <Windows.h>
#include <cmath>
#include <string>
#include <sstream>
#include <iomanip>

HWND hwnd;
HWND hwndLeftEdit;
HWND hwndRightEdit;
HWND hwndStepEdit;
HWND hwndCancelButton;
HWND hwndProgressLabel;
HWND hwndResultLabel;
HWND hwndLowestPriority;
HWND hwndNormalPriority;
HWND hwndHighestPriority;

const int ID_CALCULATE_BUTTON = 1;
const int ID_CANCEL_BUTTON = 2;
const int ID_RESULT_LABEL = 3;
const int ID_PROGRESS_LABEL = 4;
const int ID_LEFT_EDIT = 5;
const int ID_RIGHT_EDIT = 6;
const int ID_STEP_EDIT = 7;
const int ID_LOWEST_PRIORITY = 8;
const int ID_NORMAL_PRIORITY = 9;
const int ID_HIGHEST_PRIORITY = 10;

double leftValue = 0;
double rightValue = 0;
double step = 0.0001;

bool isCalculating = false;
bool isCancelRequested = false;

bool GetDoubleFromEdit(HWND hwndEdit, double& result) {
    WCHAR buffer[32];
    GetWindowText(hwndEdit, buffer, sizeof(buffer) / sizeof(WCHAR));
    try {
        result = std::stod(buffer);
        return true;
    }
    catch (...) {
        return false;
    }
}

DWORD WINAPI CalculateInBackground(LPVOID lpParam) {
    double result = 0.0;
    for (double x = leftValue; x <= rightValue; x += step) {
        if (isCancelRequested) {
            SendMessage(hwnd, WM_USER + 3, 0, 0);
            break;
        }
        result += step * std::sin(x);
        double progress = ((x - leftValue) / (rightValue - leftValue) * 100);
        SendMessage(hwnd, WM_USER + 1, *reinterpret_cast<WPARAM*>(&progress), 0);
    }
    if (isCancelRequested)
        isCancelRequested = false;
    else
        SendMessage(hwnd, WM_USER + 2, *reinterpret_cast<WPARAM*>(&result), 0);
    isCalculating = false;
    return 0;
}

LRESULT CALLBACK MainWindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        // Левая граница.
        CreateWindow(L"STATIC", L"Левая граница:", WS_VISIBLE | WS_CHILD, 20, 20, 100, 20, hwnd, NULL, NULL, NULL);
        hwndLeftEdit = CreateWindow(L"EDIT", L"0", WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 20, 100, 20, hwnd, (HMENU)ID_LEFT_EDIT, NULL, NULL);

        // Правая граница.
        CreateWindow(L"STATIC", L"Правая граница:", WS_VISIBLE | WS_CHILD, 20, 60, 100, 20, hwnd, NULL, NULL, NULL);
        hwndRightEdit = CreateWindow(L"EDIT", L"1", WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 60, 100, 20, hwnd, (HMENU)ID_RIGHT_EDIT, NULL, NULL);

        // Шаг.
        CreateWindow(L"STATIC", L"Шаг:", WS_VISIBLE | WS_CHILD, 20, 100, 100, 20, hwnd, NULL, NULL, NULL);
        hwndStepEdit = CreateWindow(L"EDIT", L"0.0001", WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 100, 100, 20, hwnd, (HMENU)ID_STEP_EDIT, NULL, NULL);

        // Вычислить.
        CreateWindow(L"BUTTON", L"Вычислить", WS_VISIBLE | WS_CHILD, 20, 140, 80, 30, hwnd, (HMENU)ID_CALCULATE_BUTTON, NULL, NULL);

        // Отменить.
        hwndCancelButton = CreateWindow(L"BUTTON", L"Отменить", WS_VISIBLE | WS_CHILD, 120, 140, 80, 30, hwnd, (HMENU)ID_CANCEL_BUTTON, NULL, NULL);

        // Проценты.
        hwndProgressLabel = CreateWindow(L"STATIC", L"", WS_VISIBLE | WS_CHILD, 20, 180, 250, 20, hwnd, (HMENU)ID_PROGRESS_LABEL, NULL, NULL);

        // Результат.
        hwndResultLabel = CreateWindow(L"STATIC", L"", WS_VISIBLE | WS_CHILD, 20, 210, 250, 20, hwnd, (HMENU)ID_RESULT_LABEL, NULL, NULL);

        // Приоритет потока.
        CreateWindow(L"STATIC", L"Приоритет:", WS_VISIBLE | WS_CHILD, 20, 250, 100, 20, hwnd, NULL, NULL, NULL);
        hwndLowestPriority = CreateWindow(L"BUTTON", L"Низкий", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 20, 280, 100, 20, hwnd, (HMENU)ID_LOWEST_PRIORITY, NULL, NULL);
        hwndNormalPriority = CreateWindow(L"BUTTON", L"Обычный", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 20, 310, 80, 20, hwnd, (HMENU)ID_NORMAL_PRIORITY, NULL, NULL);
        hwndHighestPriority = CreateWindow(L"BUTTON", L"Высокий", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 20, 340, 100, 20, hwnd, (HMENU)ID_HIGHEST_PRIORITY, NULL, NULL);
        SendMessage(hwndNormalPriority, BM_SETCHECK, BST_CHECKED, 0); 

        break;
    }
    case WM_COMMAND: {
        if (LOWORD(wParam) == ID_CALCULATE_BUTTON) {
            if (isCalculating) {
                MessageBox(hwnd, L"Идет вычисление!", L"Предупреждение", MB_ICONWARNING);
            }
            else {
                if (GetDoubleFromEdit(hwndLeftEdit, leftValue) &&
                    GetDoubleFromEdit(hwndRightEdit, rightValue) &&
                    GetDoubleFromEdit(hwndStepEdit, step)) {
                    if (leftValue > rightValue) {
                        MessageBox(hwnd, L"Левое значение не может быть больше правого!", L"Предупреждение", MB_ICONWARNING);
                    }
                    else if (step <= 0) {
                        MessageBox(hwnd, L"Некорректное значение шага!", L"Предупреждение", MB_ICONWARNING);
                    }
                    else {
                        isCalculating = true;
                        SetWindowText(hwndResultLabel, L"");
                        SetWindowText(hwndProgressLabel, L"Идет вычисление...");
                        int threadPriority = THREAD_PRIORITY_NORMAL;
                        if (SendMessage(hwndLowestPriority, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                            threadPriority = THREAD_PRIORITY_LOWEST;
                        }
                        else if (SendMessage(hwndHighestPriority, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                            threadPriority = THREAD_PRIORITY_HIGHEST;
                        }

                        HANDLE hThread = CreateThread(NULL, 0, CalculateInBackground, NULL, 0, NULL);
                        SetThreadPriority(hThread, threadPriority);
                    }
                }
                else {
                    MessageBox(hwnd, L"Неверный ввод. Пожалуйста, введите корректное число с плавающей запятой.", L"Ошибка", MB_ICONERROR);
                }
            }
        }
        else if (LOWORD(wParam) == ID_CANCEL_BUTTON) {
            if (isCalculating) {
                SendMessage(hwnd, WM_USER + 3, 0, 0); // Запрос на отмену
            }
        }
        break;
    }
    case WM_USER + 1: {
        double percent = *reinterpret_cast<double*>(&wParam);
        std::wstringstream ss;
        ss << L"Прогресс: " << std::fixed << std::setprecision(3) << percent << L"%";
        SetWindowText(hwndProgressLabel, ss.str().c_str());
        break;
    }
    case WM_USER + 2: {
        double result = *reinterpret_cast<double*>(&wParam);
        std::wstring resultText = L"Результат: " + std::to_wstring(result);
        SetWindowText(hwndResultLabel, resultText.c_str());
        SetWindowText(hwndProgressLabel, L"Вычисление завершено");
        break;
    }
    case WM_USER + 3: {
        isCancelRequested = true;
        SetWindowText(hwndProgressLabel, L"Вычисление отменено");
        break;
    }
    case WM_DESTROY: {
        PostQuitMessage(0);
        break;
    }
    default: {
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASSEX wc = {
        sizeof(WNDCLASSEX),
        CS_CLASSDC,
        MainWindowProcedure,
        0L,
        0L,
        GetModuleHandle(NULL),
        NULL,
        NULL,
        NULL,
        NULL,
        L"CalculatorClassName",
        NULL
    };
    RegisterClassEx(&wc);
    hwnd = CreateWindow(wc.lpszClassName,
        L"Калькулятор синусоиды",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        0, 0, 350, 420,
        NULL,
        NULL,
        wc.hInstance,
        NULL);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
