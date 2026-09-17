/**
 * @file category.h
 * @brief Entry categorization and tagging subsystem for kehl-vault.
 *
 * Defines standard vault categories (Logins, Cards, Secure Notes, Identity, Other)
 * and utilities for managing, parsing, and filtering entries by tags and categories.
 */

#ifndef KEHL_VAULT_CATEGORY_H
#define KEHL_VAULT_CATEGORY_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_TAGS_PER_ENTRY 8
#define TAG_NAME_MAX 32
#define CATEGORY_NAME_MAX 32

/**
 * @brief Standard vault entry categories.
 */
typedef enum {
    VAULT_CAT_LOGIN = 0,
    VAULT_CAT_CARD = 1,
    VAULT_CAT_SECURE_NOTE = 2,
    VAULT_CAT_IDENTITY = 3,
    VAULT_CAT_OTHER = 4,
    VAULT_CAT_COUNT = 5
} VaultCategory;

/**
 * @brief Converts a category enum to its human-readable string representation.
 * @param category Category enum value.
 * @return Constant string name of the category.
 */
const char* category_to_string(VaultCategory category);

/**
 * @brief Parses a string into a VaultCategory enum.
 * @param str String containing category name (case-insensitive).
 * @return Matching VaultCategory enum, or VAULT_CAT_OTHER if unrecognized.
 */
VaultCategory category_from_string(const char* str);

/**
 * @brief Splits a comma-separated tag string into an array of individual tag strings.
 * @param tag_string Input string (e.g., "work, personal, finance").
 * @param out_tags 2D array buffer to receive parsed tags.
 * @param max_tags Maximum number of tags allowed.
 * @return Number of tags parsed.
 */
int category_parse_tags(const char* tag_string, char out_tags[][TAG_NAME_MAX], int max_tags);

/**
 * @brief Checks whether a formatted tag list contains a specific tag (case-insensitive).
 * @param tag_string Comma-separated or whitespace-separated tag list.
 * @param target_tag Tag to look for.
 * @return 1 if found, 0 otherwise.
 */
int category_has_tag(const char* tag_string, const char* target_tag);

#ifdef __cplusplus
}
#endif

#endif /* KEHL_VAULT_CATEGORY_H */
