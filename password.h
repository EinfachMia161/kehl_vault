#ifndef KEHL_VAULT_PASSWORD_H
#define KEHL_VAULT_PASSWORD_H

#ifdef __cplusplus
extern "C" {
#endif

int password_is_long_enough(const char* password, int minimum_length);

int password_contains_lower_case(const char* password);
int password_contains_upper_case(const char* password);
int password_contains_digit(const char* password);
int password_contains_special_character(const char* password);

int password_calculate_strength(
    const char* password,
    int minimum_length

);

#ifdef __cplusplus
}
#endif

#endif
