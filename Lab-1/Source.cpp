#include <windows.h>


// ������� ��������� ���������
LRESULT CALLBACK MainWindowProcedure(HWND hw, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hw, msg, wp, lp);
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
	HWND hw = CreateWindow(L"MainWindowClass", L"���������", WS_OVERLAPPEDWINDOW, 0, 0, 640, 480, NULL, NULL, hInst, NULL);
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

