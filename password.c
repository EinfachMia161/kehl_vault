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

int password_generate_from_bytes(
        const unsigned char* random_bytes,
        int random_bytes_count,
        const char* character_set,
        int password_length,
        char* output,
        int output_size
) {
    int character_set_length = (int)strlen(character_set);

    if (random_bytes == NULL ||
        character_set == NULL ||
        output == NULL) {
        return 0;
    }

    if (random_bytes_count <= 0 ||
        character_set_length <= 0 ||
        password_length <= 0) {
        return 0;
    }

    if (output_size <= password_length) {
        return 0;
    }

    for (int index = 0; index < password_length; index++) {
        int character_index =
                random_bytes[index % random_bytes_count]
                % character_set_length;

        output[index] = character_set[character_index];
    }

    output[password_length] = '\0';

    return 1;
}