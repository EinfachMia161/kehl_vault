#include "password.h"

#include <string.h>
#include <stdlib.h>

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
    if (password == NULL || minimum_length < 0) {
        return 0;
    }

    int password_length = (int)strlen(password);

    if (password_length >= minimum_length) {
        return 1;
    }

    return 0;
}

int password_contains_lower_case(const char* password) {
    if (password == NULL) {
        return 0;
    }

    for (int index = 0; password[index] != '\0'; index++) {
        if (password[index] >= 'a' && password[index] <= 'z') {
            return 1;
        }
    }

    return 0;
}

int password_contains_upper_case(const char* password) {
    if (password == NULL) {
        return 0;
    }

    for (int index = 0; password[index] != '\0'; index++) {
        if (password[index] >= 'A' && password[index] <= 'Z') {
            return 1;
        }
    }

    return 0;
}

int password_contains_digit(const char* password) {
    if (password == NULL) {
        return 0;
    }

    for (int index = 0; password[index] != '\0'; index++) {
        if (password[index] >= '0' && password[index] <= '9') {
            return 1;
        }
    }

    return 0;
}

int password_contains_special_character(const char* password) {
    if (password == NULL) {
        return 0;
    }

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
    if (password == NULL || minimum_length < 0) {
        return 0;
    }

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
        return "Schwach";
    } else if (score <= 4) {
        return "Mittel";
    } else {
        return "Stark";
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
    "century", "ceremony", "chain", "chair", "chamber", "champion", "chance", "change",
    "channel", "chapter", "charge", "charity", "chart", "chase", "cheese", "cherry",
    "chest", "chicken", "chief", "child", "choice", "choose", "chronic", "chunk",
    "circle", "circuit", "citizen", "civic", "civil", "claim", "classic", "climate",
    "clinic", "clock", "closet", "cloud", "clover", "cluster", "coach", "coastal",
    "coconut", "coffee", "cohort", "colleague", "college", "colony", "column", "combine",
    "comfort", "command", "comment", "common", "company", "compare", "complex", "compose",
    "concept", "concern", "concert", "conduct", "confirm", "connect", "consent", "control",
    "cookie", "copper", "coral", "corner", "correct", "cotton", "couch", "counter",
    "country", "courage", "cousin", "cradle", "craft", "crater", "credit", "creek",
    "cricket", "crisis", "crisp", "critic", "cross", "crowd", "crystal", "cube",
    "culture", "cupboard", "curtain", "cushion", "custom", "cyber", "cycle", "canyon",
    "daily", "damage", "dance", "danger", "daring", "darwin", "database", "daughter",
    "dawn", "daylight", "dealer", "debate", "debris", "decade", "decimal", "declare",
    "decor", "decree", "defense", "degree", "delay", "deliver", "demand", "demise",
    "denial", "density", "deposit", "deputy", "derive", "desert", "design", "desk",
    "detail", "detect", "develop", "device", "devote", "dialog", "diamond", "diary",
    "diesel", "dietary", "differ", "digital", "dilemma", "dinner", "diploma", "direct"
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
    size_t current_len = 0;

    for (int i = 0; i < word_count; ++i) {
        unsigned short random_index_val = 0;
        if (!password_get_secure_random_bytes((unsigned char*)&random_index_val, sizeof(random_index_val))) {
            return 0;
        }

        int word_idx = (int)(random_index_val % WORDLIST_SIZE);
        const char* word = WORDLIST[word_idx];
        size_t word_len = strlen(word);

        if (i > 0) {
            size_t sep_len = strlen(separator);
            if (current_len + sep_len + 1 >= (size_t)output_size) {
                return 0;
            }
            strcat(output, separator);
            current_len += sep_len;
        }

        if (current_len + word_len + 1 >= (size_t)output_size) {
            return 0;
        }

        size_t start_pos = current_len;
        strcat(output, word);
        if (capitalize && word_len > 0) {
            if (output[start_pos] >= 'a' && output[start_pos] <= 'z') {
                output[start_pos] = (char)(output[start_pos] - 'a' + 'A');
            }
        }
        current_len += word_len;
    }

    return 1;
}
