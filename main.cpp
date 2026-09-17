#include "password.h"

#include <iostream>

int main() {
    const unsigned char test_random_bytes[] = {
        12, 55, 201, 7, 88, 143, 29, 250,
        64, 111, 3, 190
    };

    const char* character_set =
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "0123456789"
            "!@#$%";

    const int password_length = 12;

    char generated_password[13];

    int generation_successful =
            password_generate_from_bytes(
                    test_random_bytes,
                    12,
                    character_set,
                    password_length,
                    generated_password,
                    13
            );

    if (generation_successful == 1) {
        std::cout << "Passwort wurde erfolgreich erzeugt.\n";
        std::cout << "Laenge: " << password_length << "\n";
    } else {
        std::cout << "Passwort konnte nicht erzeugt werden.\n";
    }

    return 0;
}