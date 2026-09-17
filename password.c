/**
 * @file password.c
 * @brief Implementation of password scoring, cryptographically secure RNG, and passphrase generation.
 */

#include "password.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <bcrypt.h>
#ifndef BCRYPT_SUCCESS
#define BCRYPT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif
#else
#include <stdio.h>
#endif

#define CHARSET_LOWER "abcdefghijklmnopqrstuvwxyz"
#define CHARSET_UPPER "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define CHARSET_DIGITS "0123456789"
#define CHARSET_SPECIAL "!@#$%^&*()-_=+[]{}|;:,.<>?"

int password_is_long_enough(const char* password, int minimum_length) {
    if (!password) return 0;
    int password_length = (int)strlen(password);
    return (password_length >= minimum_length) ? 1 : 0;
}

int password_contains_lower_case(const char* password) {
    if (!password) return 0;
    for (int index = 0; password[index] != '\0'; index++) {
        if (password[index] >= 'a' && password[index] <= 'z') {
            return 1;
        }
    }
    return 0;
}

int password_contains_upper_case(const char* password) {
    if (!password) return 0;
    for (int index = 0; password[index] != '\0'; index++) {
        if (password[index] >= 'A' && password[index] <= 'Z') {
            return 1;
        }
    }
    return 0;
}

int password_contains_digit(const char* password) {
    if (!password) return 0;
    for (int index = 0; password[index] != '\0'; index++) {
        if (password[index] >= '0' && password[index] <= '9') {
            return 1;
        }
    }
    return 0;
}

int password_contains_special_character(const char* password) {
    if (!password) return 0;
    for (int index = 0; password[index] != '\0'; index++) {
        char character = password[index];
        int is_lower = (character >= 'a' && character <= 'z');
        int is_upper = (character >= 'A' && character <= 'Z');
        int is_digit = (character >= '0' && character <= '9');

        if (!is_lower && !is_upper && !is_digit) {
            return 1;
        }
    }
    return 0;
}

int password_calculate_strength(
        const char* password,
        int minimum_length
) {
    if (!password) return 0;
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
    if (random_bytes == NULL ||
        character_set == NULL ||
        output == NULL) {
        return 0;
    }

    int character_set_length = (int)strlen(character_set);
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

const char* password_strength_to_string(int score) {
    if (score <= 2) {
        return "Weak";
    } else if (score <= 4) {
        return "Medium";
    } else {
        return "Strong";
    }
}

int password_get_secure_random_bytes(unsigned char* buffer, int length) {
    if (buffer == NULL || length <= 0) {
        return 0;
    }
#if defined(_WIN32) || defined(_WIN64)
    NTSTATUS status = BCryptGenRandom(NULL, (PUCHAR)buffer, (ULONG)length, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    return BCRYPT_SUCCESS(status) ? 1 : 0;
#else
    FILE* urandom = fopen("/dev/urandom", "rb");
    if (urandom == NULL) {
        return 0;
    }
    size_t read_bytes = fread(buffer, 1, (size_t)length, urandom);
    fclose(urandom);
    return read_bytes == (size_t)length ? 1 : 0;
#endif
}

int password_generate(
    int length,
    int include_lower,
    int include_upper,
    int include_digits,
    int include_special,
    char* output,
    int output_size
) {
    if (output == NULL || length <= 0 || output_size <= length) {
        return 0;
    }

    char charset[256];
    charset[0] = '\0';

    if (include_lower) {
        strcat(charset, CHARSET_LOWER);
    }
    if (include_upper) {
        strcat(charset, CHARSET_UPPER);
    }
    if (include_digits) {
        strcat(charset, CHARSET_DIGITS);
    }
    if (include_special) {
        strcat(charset, CHARSET_SPECIAL);
    }

    if (strlen(charset) == 0) {
        return 0;
    }

    unsigned char* random_bytes = (unsigned char*)malloc((size_t)length);
    if (random_bytes == NULL) {
        return 0;
    }

    if (!password_get_secure_random_bytes(random_bytes, length)) {
        free(random_bytes);
        return 0;
    }

    int result = password_generate_from_bytes(
        random_bytes,
        length,
        charset,
        length,
        output,
        output_size
    );

    free(random_bytes);
    return result;
}

int password_generate_default(int length, char* output, int output_size) {
    return password_generate(length, 1, 1, 1, 1, output, output_size);
}

static const char* WORDLIST[] = {
    "ability", "able", "about", "above", "accept", "account", "across", "action",
    "activity", "actor", "admit", "adult", "affect", "after", "again", "against",
    "agent", "agree", "ahead", "airline", "airport", "album", "alcohol", "alive",
    "allow", "almost", "alone", "along", "already", "also", "always", "among",
    "amount", "animal", "another", "answer", "anyone", "appear", "apple", "approach",
    "area", "argue", "army", "around", "arrive", "artist", "assume", "attack",
    "author", "balance", "banana", "beacon", "beauty", "become", "before", "begin",
    "behind", "belief", "belong", "beside", "better", "beyond", "billion", "binary",
    "blanket", "block", "breeze", "bridge", "bright", "brother", "budget", "buffer",
    "builder", "cabin", "cable", "camera", "campus", "candle", "canvas", "canyon",
    "capital", "captain", "capture", "carbon", "carrier", "castle", "casual", "catalog",
    "catch", "category", "cattle", "cause", "caution", "ceiling", "cell", "center",
    "chain", "chair", "chance", "change", "channel", "chapter", "charge", "chart",
    "cheese", "choice", "church", "circle", "citizen", "city", "civil", "classic",
    "clean", "clear", "client", "climate", "clock", "cloud", "coach", "coast",
    "coffee", "column", "combat", "comfort", "command", "common", "company", "complex",
    "concept", "concern", "concert", "conduct", "confirm", "connect", "contact", "contain",
    "control", "cookie", "corner", "correct", "cotton", "couch", "counter", "country",
    "couple", "courage", "course", "cousin", "cover", "craft", "crash", "credit",
    "crisis", "critic", "cross", "crowd", "crystal", "culture", "cupboard", "current",
    "custom", "damage", "dance", "danger", "darkness", "database", "daughter", "daylight",
    "dealer", "debate", "decade", "decide", "decision", "declare", "defense", "degree",
    "delivery", "demand", "dentist", "deposit", "deputy", "derive", "desert", "design",
    "desire", "detail", "detect", "develop", "device", "dialog", "diamond", "digital",
    "dinner", "direct", "discover", "display", "distance", "divide", "doctor", "domain",
    "dolphin", "donate", "doorway", "double", "dragon", "drawer", "dream", "driver",
    "dynamic", "eager", "eagle", "early", "earth", "easily", "echo", "economy",
    "edition", "educate", "effect", "effort", "elastic", "elder", "element", "elite",
    "embark", "embrace", "emerge", "emotion", "empire", "employ", "empower", "empty",
    "enable", "enact", "endless", "endorse", "energy", "enforce", "engage", "engine",
    "enhance", "enjoy", "enlist", "enough", "enrich", "enroll", "ensure", "enter",
    "entire", "entry", "envelope", "episode", "equal", "equip", "erase", "erosion",
    "escape", "essay", "essence", "estate", "eternal", "ethics", "evidence", "evil",
    "evolve", "exact", "example", "excess", "exchange", "excite", "exclude", "excuse"
};

#define WORDLIST_SIZE ((int)(sizeof(WORDLIST) / sizeof(WORDLIST[0])))

int password_generate_passphrase(
    int word_count,
    const char* separator,
    int capitalize,
    char* output,
    int output_size
) {
    if (output == NULL || word_count <= 0 || output_size <= 0) {
        return 0;
    }

    if (separator == NULL) {
        separator = "-";
    }

    output[0] = '\0';
    unsigned char random_indices[64];

    if (word_count > (int)sizeof(random_indices)) {
        word_count = (int)sizeof(random_indices);
    }

    if (!password_get_secure_random_bytes(random_indices, word_count)) {
        return 0;
    }

    size_t current_len = 0;

    for (int i = 0; i < word_count; ++i) {
        int word_idx = random_indices[i] % WORDLIST_SIZE;
        const char* word = WORDLIST[word_idx];
        size_t word_len = strlen(word);

        if (current_len + word_len + strlen(separator) + 1 >= (size_t)output_size) {
            return 0;
        }

        if (i > 0) {
            strcat(output, separator);
            current_len += strlen(separator);
        }

        if (capitalize && word_len > 0) {
            char cap_word[64];
            strncpy(cap_word, word, sizeof(cap_word) - 1);
            cap_word[sizeof(cap_word) - 1] = '\0';
            cap_word[0] = (char)toupper((unsigned char)cap_word[0]);
            strcat(output, cap_word);
        } else {
            strcat(output, word);
        }

        current_len += word_len;
    }

    return 1;
}
