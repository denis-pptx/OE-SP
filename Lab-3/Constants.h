#pragma once

#include <Windows.h>
#include <string>
#include <map>
using namespace std;

wstring BASE_TITLE = L"Clock | UTC";

map<int, wstring> TIME_ZONE_MAP = {
       {-11, L"Midway Island"},
       {-10, L"Honolulu"},
       {-9, L"Anchorage"},
       {-8, L"Los Angeles"},
       {-7, L"Denver"},
       {-6, L"Chicago"},
       {-5, L"New York"},
       {-4, L"Caracas"},
       {-3, L"Buenos Aires"},
       {-2, L"South Georgia"},
       {-1, L"Azores"},
       {0, L"London"},
       {1, L"Paris"},
       {2, L"Athens"},
       {3, L"Minsk"},
       {4, L"Kabul"},
       {5, L"Islamabad"},
       {6, L"Almaty"},
       {7, L"Bangkok"},
       {8, L"Beijing"},
       {9, L"Tokyo"},
       {10, L"Sydney"},
       {11, L"Solomon Islands"},
       {12, L"Suva"}
};


