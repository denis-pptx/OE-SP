#pragma once
#include <string>
#include <Windows.h>
#include <mutex>
#include "Constants.h"

std::mutex mtx;

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

string NumberToStringWithLeadingZero(int number) {
    if (number < 10) 
        return "0" + to_string(number);
    else 
        return to_string(number);
    
}

void WriteLog(string str) {
    mtx.lock();

    if (pLogData != NULL) {

        // Текущее UTC время
        time_t now = time(0);
        tm* gmtm = gmtime(&now);

        str = NumberToStringWithLeadingZero(gmtm->tm_hour) + ":" +
              NumberToStringWithLeadingZero(gmtm->tm_min) + ":" +
              NumberToStringWithLeadingZero(gmtm->tm_sec) + " UTC | " +
              str + "\n";

        // Теущая длина в памяти
        int currentLength = strlen((char*)pLogData);

        if (currentLength + str.length() > logFileSize) {
            memset(pLogData, 0, logFileSize);
            MessageBox(NULL, L"Недостаточно места в файле лога. Файл очищен.", L"Warning", MB_ICONWARNING | MB_OK);
            currentLength = 0;
        }
 
        CopyMemory((char*)pLogData + currentLength, str.c_str(), str.length());
    }
    else {
        MessageBox(NULL, L"Память не инициализирована", L"Ошибка", MB_ICONERROR | MB_OK);
    }

    mtx.unlock();
}


void CloseMapping() {
    UnmapViewOfFile(pTimeZoneData);
    CloseHandle(hTimeZoneMapFile);
    CloseHandle(hTimeZoneFile);

    UnmapViewOfFile(pLogData);
    CloseHandle(hLogMapFile);
    CloseHandle(hLogFile);
}
