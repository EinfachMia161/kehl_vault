#include "password.h"

#include <iostream>

int main() {
    const char* test_password = "Abc12345!";

    const int minimum_length = 8;

    int password_score =
            password_calculate_strength(test_password, minimum_length);

    std::cout << "Passwortanalyse\n";
    std::cout << "---------------\n";

    std::cout << "Punkte: "
              << password_score
              << " von 5\n";

    std::cout << "Staerke: ";

    if (password_score <= 2) {
        std::cout << "schwach\n";
    } else if (password_score <= 4) {
        std::cout << "mittel\n";
    } else {
        std::cout << "stark\n";
    }

    return 0;
}