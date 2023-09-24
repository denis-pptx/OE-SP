#include <windows.h>

#define ID_COMBOBOX1 1
#define ID_COMBOBOX2 2

HWND hComboBox1;
HWND hComboBox2;

int selected_base_combobox1 = 1;
int selected_base_combobox2 = 0;


int combobox_index_to_base(int index) {
    if (index == 0)
        return 2;

    if (index == 1)
        return 10;

    if (index == 2)
        return 16;
}

void MainWindowAddComboBoxes(HWND hWnd) {
    hComboBox1 = CreateWindow(
        L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
        50, 50, 100, 200,
        hWnd,
        (HMENU)ID_COMBOBOX1,
        NULL,
        NULL
    );
    SendMessage(hComboBox1, CB_ADDSTRING, 0, (LPARAM)L"2");
    SendMessage(hComboBox1, CB_ADDSTRING, 0, (LPARAM)L"10");
    SendMessage(hComboBox1, CB_ADDSTRING, 0, (LPARAM)L"16");
    SendMessage(hComboBox1, CB_SETCURSEL, selected_base_combobox1, 0);


    hComboBox2 = CreateWindow(
        L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
        50, 100, 100, 200,
        hWnd,
        (HMENU)ID_COMBOBOX2,
        NULL,
        NULL
    );
    SendMessage(hComboBox2, CB_ADDSTRING, 0, (LPARAM)L"2");
    SendMessage(hComboBox2, CB_ADDSTRING, 0, (LPARAM)L"10");
    SendMessage(hComboBox2, CB_ADDSTRING, 0, (LPARAM)L"16");
    SendMessage(hComboBox2, CB_SETCURSEL, selected_base_combobox2, 0);
}


// ������� ��������� ���������
LRESULT CALLBACK MainWindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg)
    {
    case WM_CREATE:
    {
        MainWindowAddComboBoxes(hWnd);
        break;
    }

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case ID_COMBOBOX1:
            if (HIWORD(wParam) == CBN_SELCHANGE)
                selected_base_combobox1 = combobox_index_to_base(SendMessage(hComboBox1, CB_GETCURSEL, 0, 0));
            
            break;

        case ID_COMBOBOX2:
            if (HIWORD(wParam) == CBN_SELCHANGE)
                selected_base_combobox2 = combobox_index_to_base(SendMessage(hComboBox2, CB_GETCURSEL, 0, 0));
            
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
		L"MainWindowClass", L"���������", 
		WS_OVERLAPPEDWINDOW, 
		0, 0, 640, 480, 
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

