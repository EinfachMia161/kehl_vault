/**
 * @file search.c
 * @brief Implementation of fuzzy string comparison and ranked multi-field search.
 */

#include "search.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static void to_lowercase(const char* src, char* dst, size_t dst_size) {
    size_t i = 0;
    while (src[i] && i < dst_size - 1) {
        dst[i] = (char)tolower((unsigned char)src[i]);
        i++;
    }
    dst[i] = '\0';
}

int search_levenshtein_distance(const char* s1, const char* s2) {
    if (!s1 || !s2) return 999;

    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);

    if (len1 == 0) return (int)len2;
    if (len2 == 0) return (int)len1;

    int* column = (int*)malloc((len1 + 1) * sizeof(int));
    if (!column) return 999;

    for (size_t y = 0; y <= len1; y++) {
        column[y] = (int)y;
    }

    for (size_t x = 1; x <= len2; x++) {
        column[0] = (int)x;
        int last_diag = (int)(x - 1);

        for (size_t y = 1; y <= len1; y++) {
            int old_diag = column[y];
            int cost = (tolower((unsigned char)s1[y - 1]) == tolower((unsigned char)s2[x - 1])) ? 0 : 1;

            int min_val = column[y] + 1; /* deletion */
            if (column[y - 1] + 1 < min_val) min_val = column[y - 1] + 1; /* insertion */
            if (last_diag + cost < min_val) min_val = last_diag + cost; /* substitution */

            column[y] = min_val;
            last_diag = old_diag;
        }
    }

    int result = column[len1];
    free(column);
    return result;
}

int search_calculate_score(const char* text, const char* query) {
    if (!text || !query || strlen(query) == 0) return 0;

    char lower_text[256];
    char lower_query[256];
    to_lowercase(text, lower_text, sizeof(lower_text));
    to_lowercase(query, lower_query, sizeof(lower_query));

    /* Exact match */
    if (strcmp(lower_text, lower_query) == 0) {
        return 100;
    }

    /* Prefix match */
    if (strncmp(lower_text, lower_query, strlen(lower_query)) == 0) {
        return 80;
    }

    /* Substring match */
    if (strstr(lower_text, lower_query) != NULL) {
        return 60;
    }

    /* Fuzzy Levenshtein match for queries >= 3 chars */
    size_t q_len = strlen(lower_query);
    if (q_len >= 3) {
        int dist = search_levenshtein_distance(lower_text, lower_query);
        if (dist <= 2) {
            return 40 - (dist * 10);
        }
    }

    return 0;
}

int search_vault(const EntryList* list, const char* query, SearchResultSet* results) {
    if (!list || !query || !results || list->count <= 0 || strlen(query) == 0) {
        if (results) results->count = 0;
        return 0;
    }

    results->count = 0;

    for (int i = 0; i < list->count; ++i) {
        const Entry* entry = &list->entries[i];

        int title_score = search_calculate_score(entry->title, query);
        int user_score = search_calculate_score(entry->username, query);

        int total_score = (title_score * 3) + (user_score * 2);

        if (total_score > 0 && results->count < MAX_SEARCH_MATCHES) {
            results->matches[results->count].entry_index = i;
            results->matches[results->count].relevance_score = total_score;
            results->count++;
        }
    }

    /* Sort matches descending by score (simple selection sort) */
    for (int i = 0; i < results->count - 1; ++i) {
        int max_idx = i;
        for (int j = i + 1; j < results->count; ++j) {
            if (results->matches[j].relevance_score > results->matches[max_idx].relevance_score) {
                max_idx = j;
            }
        }
        if (max_idx != i) {
            SearchMatch temp = results->matches[i];
            results->matches[i] = results->matches[max_idx];
            results->matches[max_idx] = temp;
        }
    }

    return results->count;
}
