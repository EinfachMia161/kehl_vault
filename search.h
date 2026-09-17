/**
 * @file search.h
 * @brief Multi-field fuzzy search and ranking engine for kehl-vault entries.
 *
 * Implements substring search, Levenshtein distance calculations, and multi-field
 * weighted relevance scoring across titles and usernames.
 */

#ifndef KEHL_VAULT_SEARCH_H
#define KEHL_VAULT_SEARCH_H

#include "entry.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_SEARCH_MATCHES 64

/**
 * @brief Represents a single search match result with relevance score.
 */
typedef struct {
    int entry_index;
    int relevance_score; /* Higher score = better match */
} SearchMatch;

/**
 * @brief Container for ranked search query results.
 */
typedef struct {
    SearchMatch matches[MAX_SEARCH_MATCHES];
    int count;
} SearchResultSet;

/**
 * @brief Computes Levenshtein edit distance between two strings.
 * @param s1 First null-terminated string.
 * @param s2 Second null-terminated string.
 * @return Minimum edit distance (insertions, deletions, substitutions).
 */
int search_levenshtein_distance(const char* s1, const char* s2);

/**
 * @brief Evaluates match score between a target text and a query term.
 * @param text Target text (e.g. entry title).
 * @param query Query search term.
 * @return Calculated score (0 means no match, higher means closer match).
 */
int search_calculate_score(const char* text, const char* query);

/**
 * @brief Executes a ranked multi-field search across an EntryList.
 * @param list Pointer to EntryList to search.
 * @param query Search query string.
 * @param results Pointer to SearchResultSet to receive sorted matches.
 * @return Total number of matching entries found.
 */
int search_vault(const EntryList* list, const char* query, SearchResultSet* results);

#ifdef __cplusplus
}
#endif

#endif /* KEHL_VAULT_SEARCH_H */
