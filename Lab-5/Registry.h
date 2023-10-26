#include <Windows.h>
#include <string>
#include "Constants.h"

// ������� ��� ���������� �������� ����� � �������
void SaveTimeZoneToRegistry(int timeZone) {
    HKEY hKey;
    LONG result = RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\KonchikClock", 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);
    if (result != ERROR_SUCCESS) {
        MessageBox(NULL, L"������ ��� �������� ����� �������", L"������", MB_ICONERROR | MB_OK);
        RegCloseKey(hKey);
        return;
    }

    result = RegSetValueEx(hKey, L"TimeZone", 0, REG_DWORD, reinterpret_cast<BYTE*>(&timeZone), sizeof(timeZone));
    if (result != ERROR_SUCCESS) {
        MessageBox(NULL, L"������ ��� ������ �������� � ������", L"������", MB_ICONERROR | MB_OK);
    }

    RegCloseKey(hKey);
}

// ������� ��� �������� �������� ����� �� �������
int LoadTimeZoneFromRegistry() {
    HKEY hKey;
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\KonchikClock", 0, KEY_READ, &hKey);
    if (result != ERROR_SUCCESS) {
        MessageBox(NULL, L"������ ��� �������� ����� �������", L"������", MB_ICONERROR | MB_OK);
        RegCloseKey(hKey);
        return 0;
    }

    DWORD timeZone = 0;
    DWORD dataSize = sizeof(timeZone);
    result = RegQueryValueEx(hKey, L"TimeZone", NULL, NULL, reinterpret_cast<BYTE*>(&timeZone), &dataSize);
    if (result != ERROR_SUCCESS) {
        MessageBox(NULL, L"������ ��� ������ �������� �� �������", L"������", MB_ICONERROR | MB_OK);
        RegCloseKey(hKey);
        return 0;
    }

    RegCloseKey(hKey);
    return static_cast<int>(timeZone);
}

