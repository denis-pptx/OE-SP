#include <windows.h>
#include <string>
#include <regex>

#include "Converters.h"
#include "Constants.h"

using namespace std;


int selected_index_combobox1 = 1;
int selected_index_combobox2 = 0;


void MainWindowAddComboBoxes(HWND hWnd);
void MainWindowAddEdits(HWND hWnd);
void MainWindowAddButtons(HWND hWnd);


LRESULT CALLBACK MainWindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


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
		L"MainWindowClass", L"Конвертер", 
		WS_OVERLAPPEDWINDOW, 
		0, 0, 370, 280, 
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


void MainWindowAddComboBoxes(HWND hWnd) {
    HWND hComboBox1 = CreateWindow(
        L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
        50, 50, 100, 200,
        hWnd,
        (HMENU)ID_COMBOBOX1,
        NULL,
        NULL
    );

    for (auto base : select_bases)
        SendMessage(hComboBox1, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(to_wstring(base).c_str()));

    SendMessage(hComboBox1, CB_SETCURSEL, selected_index_combobox1, 0);



    HWND hComboBox2 = CreateWindow(
        L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
        50, 100, 100, 200,
        hWnd,
        (HMENU)ID_COMBOBOX2,
        NULL,
        NULL
    );

    for (auto base : select_bases)
        SendMessage(hComboBox2, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(to_wstring(base).c_str()));

    SendMessage(hComboBox2, CB_SETCURSEL, selected_index_combobox2, 0);
}


void MainWindowAddEdits(HWND hWnd) {
    HWND hEdit1 = CreateWindow(
        L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        160, 50, 150, 25,
        hWnd,
        (HMENU)ID_EDIT1,
        NULL,
        NULL
    );

    HWND hEdit2 = CreateWindow(
        L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        160, 100, 150, 25,
        hWnd,
        (HMENU)ID_EDIT2,
        NULL,
        NULL
    );
}



void MainWindowAddButtons(HWND hWnd) {
    HWND hButton = CreateWindow(
        L"BUTTON", L"Конвертировать",
        WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
        160, 150, 150, 25,
        hWnd,
        (HMENU)ID_BUTTON_CONVERT,
        NULL,
        NULL
    );
}



// Функция обработки сообщений
LRESULT CALLBACK MainWindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg)
    {
    case WM_CREATE:
    {
        MainWindowAddComboBoxes(hWnd);
        MainWindowAddEdits(hWnd);
        MainWindowAddButtons(hWnd);
        break;
    }

    // LOWORD(wParam) - Идентификатор элемента управления
    // HIWORD(wParam) - Код уведомления, определяемый элементом управления
    // lParam - Дескриптор окна управления
    // https://learn.microsoft.com/ru-ru/windows/win32/menurc/wm-command
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case ID_COMBOBOX1:
            if (HIWORD(wParam) == CBN_SELCHANGE)
                selected_index_combobox1 = SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);

            break;

        case ID_COMBOBOX2:
            if (HIWORD(wParam) == CBN_SELCHANGE)
                selected_index_combobox2 = SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);

            break;

        case ID_BUTTON_CONVERT:
            if (HIWORD(wParam) == BN_CLICKED)
            {
                WCHAR readFirst[50];
                GetWindowText(GetDlgItem(hWnd, ID_EDIT1), readFirst, sizeof(readFirst) / sizeof(readFirst[0]));

                wstring w_inputFirst(readFirst);
                string inputFirst = std::string(w_inputFirst.begin(), w_inputFirst.end());

                int selected_base = select_bases[selected_index_combobox1];
                if (
                    (selected_base == 2 && !regex_match(inputFirst, binary_pattern)) ||
                    (selected_base == 10 && (!regex_match(inputFirst, decimal_pattern) || stoi(inputFirst) > 65535)) ||
                    (selected_base == 16 && !regex_match(inputFirst, hexadecimal_pattern))
                    )
                {
                    MessageBox(hWnd, L"1. Не допускаются некорректные символы.\n2. Только положительные числа.\n3. Ввод не может быть пустой. \n4. Максимальный размер: 16 бит.", L"Ошибка", MB_ICONERROR | MB_OK);
                    SetWindowText(GetDlgItem(hWnd, ID_EDIT2), L"Некорректный ввод.");
                }
                else {
                    try {
                        string temp = baseToDecimal(inputFirst, select_bases[selected_index_combobox1]);
                        string result = decimalToBase(temp, select_bases[selected_index_combobox2]);

                        SetWindowText(GetDlgItem(hWnd, ID_EDIT2), wstring(result.begin(), result.end()).c_str());
                    }
                    catch (exception e) {
                        MessageBox(hWnd, L"Ошибка конвертации.", L"Ошибка", MB_ICONERROR | MB_OK);
                        SetWindowText(GetDlgItem(hWnd, ID_EDIT2), L"Ошибка конвертации.");
                    }
                }
            }
            break;
        }
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}