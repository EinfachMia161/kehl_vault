#include "password.h"

#include <string.h>

int password_is_long_enough(const char* password, int minimum_length) {
    int password_length = (int)strlen(password);

    if (password_length >= minimum_length) {
        return 1;
    }

    return 0;
}
