#pragma once
#include <Windows.h>
#include "Constants.h"

HANDLE hTimeZoneFile;
HANDLE hTimeZoneMapFile;
LPVOID pTimeZoneData;

HANDLE hLogFile;
HANDLE hLogMapFile;
LPVOID pLogData;


void InitMapping() {

    if (hTimeZoneFile = CreateFileA(timeZoneFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL),
        hTimeZoneFile == INVALID_HANDLE_VALUE) {
        MessageBox(NULL, L"hTimeZoneFile error", L"Warning", MB_ICONWARNING | MB_OK);
        return;
    }

    if (hTimeZoneMapFile = CreateFileMappingA(hTimeZoneFile, NULL, PAGE_READWRITE, 0, timeZoneFileSize, NULL),
        hTimeZoneMapFile == NULL) {
        MessageBox(NULL, L"hTimeZoneMapFile error", L"Warning", MB_ICONWARNING | MB_OK);
        return;
    }
   
    if (pTimeZoneData = MapViewOfFile(hTimeZoneMapFile, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, timeZoneFileSize),
        pTimeZoneData == NULL) {
        MessageBox(NULL, L"pTimeZoneData error", L"Warning", MB_ICONWARNING | MB_OK);
        return;
    }




    if (hLogFile = CreateFileA(logFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL),
        hLogFile == INVALID_HANDLE_VALUE) {
        MessageBox(NULL, L"hLogFile error", L"Warning", MB_ICONWARNING | MB_OK);
        return;
    }

    if (hLogMapFile = CreateFileMappingA(hLogFile, NULL, PAGE_READWRITE, 0, logFileSize, NULL),
        hLogMapFile == NULL) {
        MessageBox(NULL, L"hLogMapFile error", L"Warning", MB_ICONWARNING | MB_OK);
        return;
    }

    if (pLogData = MapViewOfFile(hLogMapFile, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, logFileSize),
        pLogData == NULL) {
        MessageBox(NULL, L"pLogData error", L"Warning", MB_ICONWARNING | MB_OK);
        return;
    }
}


void WriteLog(LPCSTR str) {
    
}

void CloseMapping() {
    UnmapViewOfFile(pTimeZoneData);
    CloseHandle(hTimeZoneMapFile);
    CloseHandle(hTimeZoneFile);

    UnmapViewOfFile(pLogData);
    CloseHandle(hLogMapFile);
    CloseHandle(hLogFile);
}
