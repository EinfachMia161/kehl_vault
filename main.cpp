#include "password.h"

#include <iostream>

int main() {
    const char* short_password = "abc";
    const char* long_password = "abc12345";

    const int minimum_length = 8;

    int short_password_is_valid =
            password_is_long_enough(short_password, minimum_length);

    int long_password_is_valid =
            password_is_long_enough(long_password, minimum_length);

    std::cout << "Test 1: ";
    if (short_password_is_valid == 1) {
        std::cout << "Passwort ist lang genug.\n";
    } else {
        std::cout << "Passwort ist zu kurz.\n";
    }

    std::cout << "Test 2: ";
    if (long_password_is_valid == 1) {
        std::cout << "Passwort ist lang genug.\n";
    } else {
        std::cout << "Passwort ist zu kurz.\n";
    }

    return 0;
}