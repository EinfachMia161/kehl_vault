/**
 * @file category.c
 * @brief Implementation of categorization and tag parsing functions.
 */

#include "category.h"
#include <string.h>
#include <ctype.h>

static const char* CATEGORY_NAMES[VAULT_CAT_COUNT] = {
    "Login",
    "Card",
    "Secure Note",
    "Identity",
    "Other"
};

const char* category_to_string(VaultCategory category) {
    if (category >= 0 && category < VAULT_CAT_COUNT) {
        return CATEGORY_NAMES[category];
    }
    return "Other";
}

static int strcasecmp_custom(const char* s1, const char* s2) {
    while (*s1 && *s2) {
        int diff = tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
        if (diff != 0) return diff;
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

VaultCategory category_from_string(const char* str) {
    if (!str) return VAULT_CAT_OTHER;

    for (int i = 0; i < VAULT_CAT_COUNT; ++i) {
        if (strcasecmp_custom(str, CATEGORY_NAMES[i]) == 0) {
            return (VaultCategory)i;
        }
    }

    if (strcasecmp_custom(str, "note") == 0 || strcasecmp_custom(str, "notes") == 0) {
        return VAULT_CAT_SECURE_NOTE;
    }
    if (strcasecmp_custom(str, "creditcard") == 0 || strcasecmp_custom(str, "cards") == 0) {
        return VAULT_CAT_CARD;
    }
    if (strcasecmp_custom(str, "id") == 0) {
        return VAULT_CAT_IDENTITY;
    }

    return VAULT_CAT_OTHER;
}

int category_parse_tags(const char* tag_string, char out_tags[][TAG_NAME_MAX], int max_tags) {
    if (!tag_string || !out_tags || max_tags <= 0) {
        return 0;
    }

    int count = 0;
    const char* ptr = tag_string;

    while (*ptr && count < max_tags) {
        /* Skip leading whitespace and commas */
        while (*ptr && (*ptr == ' ' || *ptr == '\t' || *ptr == ',')) {
            ptr++;
        }
        if (!*ptr) break;

        const char* start = ptr;
        while (*ptr && *ptr != ',') {
            ptr++;
        }

        const char* end = ptr - 1;
        while (end > start && (*end == ' ' || *end == '\t')) {
            end--;
        }

        size_t len = (size_t)(end - start + 1);
        if (len > 0) {
            if (len >= TAG_NAME_MAX) {
                len = TAG_NAME_MAX - 1;
            }
            memcpy(out_tags[count], start, len);
            out_tags[count][len] = '\0';
            count++;
        }

        if (*ptr == ',') {
            ptr++;
        }
    }

    return count;
}

int category_has_tag(const char* tag_string, const char* target_tag) {
    if (!tag_string || !target_tag || strlen(target_tag) == 0) {
        return 0;
    }

    char parsed_tags[MAX_TAGS_PER_ENTRY][TAG_NAME_MAX];
    int count = category_parse_tags(tag_string, parsed_tags, MAX_TAGS_PER_ENTRY);

    for (int i = 0; i < count; ++i) {
        if (strcasecmp_custom(parsed_tags[i], target_tag) == 0) {
            return 1;
        }
    }

    return 0;
}
