#pragma once
#include <Windows.h>

HANDLE hFile;
HANDLE hMapFile;
LPVOID pData;

LPCSTR fileName = "data.txt";
CONST INT fileSize = 1;

bool OpenFile() {
    hFile = CreateFileA(fileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    return hFile != INVALID_HANDLE_VALUE;
}

bool CreateMap() {
    hMapFile = CreateFileMappingA(hFile, NULL, PAGE_READWRITE, 0, fileSize, NULL);
    return hMapFile != NULL;
}

bool MapToMemory() {
    pData = MapViewOfFile(hMapFile, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, fileSize);
    if (pData == NULL) {
        CloseHandle(hMapFile);
        CloseHandle(hFile);
        return false;
    }
    return true;
}

void CloseResources() {
    CloseHandle(hMapFile);
    CloseHandle(hFile);
}