#pragma once

#include <string>
#include <cmath>
using namespace std;

// Функция для перевода из десятичной системы в другие системы счисления
string decimalToBase(const string& numStr, int base) {
    int num = stoi(numStr);

    if (num == 0)
        return "0";

    string result = "";
    while (num > 0) {
        int remainder = num % base;
        if (remainder < 10) {
            result = char('0' + remainder) + result;
        }
        else {
            result = char('A' + remainder - 10) + result;
        }
        num /= base;
    }
    return result;
}

// Функция для перевода из других систем счисления в десятичную
string baseToDecimal(const string& numStr, int base) {
    int result = 0;
    int power = 0;
    for (int i = numStr.length() - 1; i >= 0; i--) {
        int digit;
        if (numStr[i] >= '0' && numStr[i] <= '9') {
            digit = numStr[i] - '0';
        }
        else if (numStr[i] >= 'A' && numStr[i] <= 'F') {
            digit = numStr[i] - 'A' + 10;
        }
        else {
            return "Неверный символ в символе: " + numStr[i];
        }
        result += digit * pow(base, power);
        power++;
    }
    return to_string(result);
}
