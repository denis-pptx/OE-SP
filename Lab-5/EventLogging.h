#pragma once

#include <string>
#include <Windows.h>
#include <ctime>
using namespace std;


wstring NumberToStringWithLeadingZero(int number) {
    if (number < 10)
        return L"0" + to_wstring(number);
    else
        return to_wstring(number);
}

void WriteToEventLog(wstring message) {
    HANDLE hEventLog = RegisterEventSource(NULL, L"KonchikClock");

    if (hEventLog) {
        // Текущее UTC время
        time_t now = time(0);
        tm* gmtm = gmtime(&now);

        // Добавить в сообщение время
        message = NumberToStringWithLeadingZero(gmtm->tm_hour) + L":" +
                  NumberToStringWithLeadingZero(gmtm->tm_min) + L":" +
                  NumberToStringWithLeadingZero(gmtm->tm_sec) + L" UTC | " +
                  message;

        const wchar_t* messageStrings[1];
        messageStrings[0] = message.c_str();

        ReportEvent(hEventLog, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, messageStrings, NULL);

        DeregisterEventSource(hEventLog);
    }
}