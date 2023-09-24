#pragma once

#include <map>
#include <string>
#include <regex>
using namespace std;

#define ID_COMBOBOX1 1
#define ID_COMBOBOX2 2

#define ID_EDIT1 3
#define ID_EDIT2 4

#define ID_BUTTON_CONVERT 5


map<int, int> select_bases{
    {0, 2},
    {1, 10},
    {2, 16}
};

regex binary_pattern("^[01]{1,16}$");
regex decimal_pattern("^[0-9]{1,5}$");
regex hexadecimal_pattern("^[0-9A-F]{1,4}$");