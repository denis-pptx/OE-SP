#include <Windows.h>

HWND hwndLeftEdit;
HWND hwndRightEdit;
HWND hwndStepEdit;
HWND hwndCancelButton;
HWND hwndProgressLabel;
HWND hwndResultLabel;


const int ID_CALCULATE_BUTTON = 101;
const int ID_CANCEL_BUTTON = 102;
const int ID_RESULT_LABEL = 103;
const int ID_PROGRESS_LABEL = 104;
const int ID_LEFT_EDIT = 105;
const int ID_RIGHT_EDIT = 106;
const int ID_STEP_EDIT = 107;


LRESULT CALLBACK MainWindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
		// Левая граница.
        CreateWindow(L"STATIC", L"Left:", WS_VISIBLE | WS_CHILD, 20, 20, 50, 20, hwnd, NULL, NULL, NULL);
        hwndLeftEdit = CreateWindow(L"EDIT", L"0", WS_VISIBLE | WS_CHILD | WS_BORDER, 80, 20, 100, 20, hwnd, (HMENU)ID_LEFT_EDIT, NULL, NULL);

		// Правая граница.
        CreateWindow(L"STATIC", L"Right:", WS_VISIBLE | WS_CHILD, 20, 60, 50, 20, hwnd, NULL, NULL, NULL);
        hwndRightEdit = CreateWindow(L"EDIT", L"1", WS_VISIBLE | WS_CHILD | WS_BORDER, 80, 60, 100, 20, hwnd, (HMENU)ID_RIGHT_EDIT, NULL, NULL);

		// Шаг.
        CreateWindow(L"STATIC", L"Step:", WS_VISIBLE | WS_CHILD, 20, 100, 50, 20, hwnd, NULL, NULL, NULL);
        hwndStepEdit = CreateWindow(L"EDIT", L"0.0001", WS_VISIBLE | WS_CHILD | WS_BORDER, 80, 100, 100, 20, hwnd, (HMENU)ID_STEP_EDIT, NULL, NULL);

		// Вычислить.
        CreateWindow(L"BUTTON", L"Calculate", WS_VISIBLE | WS_CHILD, 20, 140, 80, 30, hwnd, (HMENU)ID_CALCULATE_BUTTON, NULL, NULL);

		// Отменить.
        hwndCancelButton = CreateWindow(L"BUTTON", L"Cancel", WS_VISIBLE | WS_CHILD, 120, 140, 80, 30, hwnd, (HMENU)ID_CANCEL_BUTTON, NULL, NULL);

		// Проценты.
        hwndProgressLabel = CreateWindow(L"STATIC", L"", WS_VISIBLE | WS_CHILD, 20, 180, 250, 20, hwnd, (HMENU)ID_PROGRESS_LABEL, NULL, NULL);

		// Результат.
        hwndResultLabel = CreateWindow(L"STATIC", L"", WS_VISIBLE | WS_CHILD, 20, 210, 250, 20, hwnd, (HMENU)ID_RESULT_LABEL, NULL, NULL);

        break;
    }

    case WM_DESTROY: {
        PostQuitMessage(0);
        break;
    }
    default: 
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int ss) {

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
		L"MainWindowClass", L"Интеграл y = sin(x)",
		WS_OVERLAPPEDWINDOW,
		0, 0, 300, 300,
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