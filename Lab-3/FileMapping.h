#pragma once
#include <Windows.h>
#include "Constants.h"

HANDLE hTimeZoneFile;
HANDLE hTimeZoneMapFile;
LPVOID pTimeZoneData;


void InitMapping() {

    if (hTimeZoneFile = CreateFileA(timeZoneFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL),
        hTimeZoneFile == INVALID_HANDLE_VALUE) {
        MessageBox(NULL, L"hTimeZoneFile error", L"Warning", MB_ICONWARNING | MB_OK);
        return;
    }

    if (hTimeZoneMapFile = CreateFileMappingA(hTimeZoneFile, NULL, PAGE_READWRITE, 0, timeZonefileSize, NULL),
        hTimeZoneMapFile == NULL) {
        MessageBox(NULL, L"hTimeZoneMapFile error", L"Warning", MB_ICONWARNING | MB_OK);
        return;
    }
   
    if (pTimeZoneData = MapViewOfFile(hTimeZoneMapFile, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, timeZonefileSize),
        pTimeZoneData == NULL) {
        MessageBox(NULL, L"pTimeZoneData error", L"Warning", MB_ICONWARNING | MB_OK);
        return;
    }
}


void CloseMapping() {
    UnmapViewOfFile(pTimeZoneData);
    CloseHandle(hTimeZoneMapFile);
    CloseHandle(hTimeZoneFile);
}
