#include "password.h"

#include <string.h>

int password_is_long_enough(const char* password, int minimum_length) {
    int password_length = (int)strlen(password);

    if (password_length >= minimum_length) {
        return 1;
    }

    return 0;
}

int password_contains_lower_case(const char* password) {
    for (int index = 0; password[index] != '\0'; index++) {
        if (password[index] >= 'a' && password[index] <= 'z') {
            return 1;
        }
    }

    return 0;
}

int password_contains_upper_case(const char* password) {
    for (int index = 0; password[index] != '\0'; index++) {
        if (password[index] >= 'A' && password[index] <= 'Z') {
            return 1;
        }
    }

    return 0;
}

int password_contains_digit(const char* password) {
    for (int index = 0; password[index] != '\0'; index++) {
        if (password[index] >= '0' && password[index] <= '9') {
            return 1;
        }
    }

    return 0;
}

int password_contains_special_character(const char* password) {
    for (int index = 0; password[index] != '\0'; index++) {
        char character = password[index];

        int is_lower_case =
                character >= 'a' && character <= 'z';

        int is_upper_case =
                character >= 'A' && character <= 'Z';

        int is_digit =
                character >= '0' && character <= '9';

        if (!is_lower_case && !is_upper_case && !is_digit) {
            return 1;
        }
    }

    return 0;
}

int password_calculate_strength(
        const char* password,
        int minimum_length
) {
    int score = 0;

    if (password_is_long_enough(password, minimum_length) == 1) {
        score++;
    }

    if (password_contains_lower_case(password) == 1) {
        score++;
    }

    if (password_contains_upper_case(password) == 1) {
        score++;
    }

    if (password_contains_digit(password) == 1) {
        score++;
    }

    if (password_contains_special_character(password) == 1) {
        score++;
    }

    return score;
}