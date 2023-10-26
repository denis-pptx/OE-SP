#include <Windows.h>
#include <string>
#include "Constants.h"

// Функция для сохранения часового пояса в реестре
void SaveTimeZoneToRegistry(int timeZone) {
    HKEY hKey;

    // Создать/получить ключ
    LONG result = RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\KonchikClock", 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);
    if (result != ERROR_SUCCESS) {
        MessageBox(NULL, L"Ошибка при создании ключа реестра", L"Ошибка", MB_ICONERROR | MB_OK);
        RegCloseKey(hKey);
        return;
    }

    // Получить значение
    result = RegSetValueEx(hKey, L"TimeZone", 0, REG_DWORD, reinterpret_cast<BYTE*>(&timeZone), sizeof(timeZone));
    if (result != ERROR_SUCCESS) {
        MessageBox(NULL, L"Ошибка при записи значения в реестр", L"Ошибка", MB_ICONERROR | MB_OK);
    }

    RegCloseKey(hKey);
}

// Функция для загрузки часового пояса из реестра
int LoadTimeZoneFromRegistry() {
    HKEY hKey;

    // Открыть ключ
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\KonchikClock", 0, KEY_READ, &hKey);

    // Успешно
    if (result == ERROR_SUCCESS) {
        DWORD timeZone = 0;
        DWORD dataSize = sizeof(timeZone);

        // Получить значение 
        result = RegQueryValueEx(hKey, L"TimeZone", NULL, NULL, reinterpret_cast<BYTE*>(&timeZone), &dataSize);
        if (result != ERROR_SUCCESS) {
            MessageBox(NULL, L"Ошибка при чтении значения из реестра", L"Ошибка", MB_ICONERROR | MB_OK);
            RegCloseKey(hKey);
            return 0;
        }

        RegCloseKey(hKey);
        return static_cast<int>(timeZone);
    }
    // Ключа не существует
    else if (result == ERROR_FILE_NOT_FOUND) {
        RegCloseKey(hKey);
        SaveTimeZoneToRegistry(DEFAULT_TIMEZONE);
        return DEFAULT_TIMEZONE;
    }
    // Другая ошибка
    else {
        MessageBox(NULL, L"Ошибка при открытии ключа реестра", L"Ошибка", MB_ICONERROR | MB_OK);
        RegCloseKey(hKey);
        return 0;
    }
}

