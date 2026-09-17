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

const char* password_strength_to_string(int score);

int password_get_secure_random_bytes(
    unsigned char* buffer,
    int length
);

int password_generate(
    int length,
    int include_lower,
    int include_upper,
    int include_digits,
    int include_special,
    char* output,
    int output_size
);

int password_generate_default(
    int length,
    char* output,
    int output_size
);

int password_generate_passphrase(
    int word_count,
    const char* separator,
    int capitalize,
    char* output,
    int output_size
);

int password_generate_from_bytes(
    const unsigned char* random_bytes,
    int random_bytes_count,
    const char* character_set,
    int password_length,
    char* output,
    int output_size

);

#ifdef __cplusplus
}
#endif

#endif
