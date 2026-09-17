/**
 * @file password.h
 * @brief Password strength calculation, entropy acquisition, and Diceware passphrase generation.
 *
 * Provides cryptographic random generation, character-set policy validation,
 * score calculation, and multi-word passphrase generation.
 */

#ifndef KEHL_VAULT_PASSWORD_H
#define KEHL_VAULT_PASSWORD_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Checks if password length meets the minimum threshold.
 * @param password Password string.
 * @param minimum_length Minimum length in characters.
 * @return 1 if long enough, 0 otherwise.
 */
int password_is_long_enough(const char* password, int minimum_length);

/**
 * @brief Checks if password contains at least one lowercase ASCII letter.
 * @param password Password string.
 * @return 1 if found, 0 otherwise.
 */
int password_contains_lower_case(const char* password);

/**
 * @brief Checks if password contains at least one uppercase ASCII letter.
 * @param password Password string.
 * @return 1 if found, 0 otherwise.
 */
int password_contains_upper_case(const char* password);

/**
 * @brief Checks if password contains at least one decimal digit.
 * @param password Password string.
 * @return 1 if found, 0 otherwise.
 */
int password_contains_digit(const char* password);

/**
 * @brief Checks if password contains at least one special/symbol character.
 * @param password Password string.
 * @return 1 if found, 0 otherwise.
 */
int password_contains_special_character(const char* password);

/**
 * @brief Evaluates composite password strength score (range 0 to 5).
 * @param password Password string to grade.
 * @param minimum_length Minimum length threshold (default 8).
 * @return Strength score from 0 (very weak) to 5 (very strong).
 */
int password_calculate_strength(
    const char* password,
    int minimum_length
);

/**
 * @brief Converts numeric strength score into descriptive text.
 * @param score Score value (0 to 5).
 * @return String description ("Weak", "Medium", "Strong", etc.).
 */
const char* password_strength_to_string(int score);

/**
 * @brief Acquires cryptographically secure random bytes from OS entropy source.
 * @param buffer Output byte buffer.
 * @param length Number of random bytes requested.
 * @return 1 on success, 0 on failure.
 */
int password_get_secure_random_bytes(
    unsigned char* buffer,
    int length
);

/**
 * @brief Generates a cryptographically strong password with customized character sets.
 * @param length Desired length of password.
 * @param include_lower Include lowercase [a-z].
 * @param include_upper Include uppercase [A-Z].
 * @param include_digits Include digits [0-9].
 * @param include_special Include special symbols.
 * @param output Destination buffer for null-terminated password.
 * @param output_size Size of destination buffer.
 * @return 1 on success, 0 on failure.
 */
int password_generate(
    int length,
    int include_lower,
    int include_upper,
    int include_digits,
    int include_special,
    char* output,
    int output_size
);

/**
 * @brief Generates a standard default password with all character sets enabled.
 * @param length Desired length (minimum 8).
 * @param output Destination buffer.
 * @param output_size Size of destination buffer.
 * @return 1 on success, 0 on failure.
 */
int password_generate_default(
    int length,
    char* output,
    int output_size
);

/**
 * @brief Generates a human-memorable multi-word passphrase using Diceware wordlists.
 * @param word_count Number of words to combine (e.g. 4, 5, 6).
 * @param separator Separator string (e.g. "-", ".", " ").
 * @param capitalize 1 to capitalize each word, 0 for lowercase.
 * @param output Destination buffer.
 * @param output_size Size of destination buffer.
 * @return 1 on success, 0 on failure.
 */
int password_generate_passphrase(
    int word_count,
    const char* separator,
    int capitalize,
    char* output,
    int output_size
);

/**
 * @brief Generates a password mapped from raw random entropy bytes into a character set.
 * @param random_bytes Buffer containing random bytes.
 * @param random_bytes_count Count of random bytes.
 * @param character_set String of allowed characters.
 * @param password_length Output password length.
 * @param output Destination string buffer.
 * @param output_size Destination buffer capacity.
 * @return 1 on success, 0 on failure.
 */
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

#endif /* KEHL_VAULT_PASSWORD_H */
