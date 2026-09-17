/**
 * @file history.h
 * @brief Vault change history, entry revisioning, and soft-delete trash bin management.
 *
 * Maintains historical audit logs of entry updates and deletions, allowing users
 * to inspect previous credentials and restore accidentally deleted entries.
 */

#ifndef KEHL_VAULT_HISTORY_H
#define KEHL_VAULT_HISTORY_H

#include "entry.h"
#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_HISTORY_RECORDS 256

/**
 * @brief History action event type.
 */
typedef enum {
    HIST_ACTION_MODIFIED = 0,
    HIST_ACTION_DELETED = 1
} HistoryAction;

/**
 * @brief Represents a single historical snapshot of an entry state.
 */
typedef struct {
    Entry entry;
    uint64_t timestamp;
    HistoryAction action;
    int is_purged;
} HistoryRecord;

/**
 * @brief Collection of history records.
 */
typedef struct {
    HistoryRecord records[MAX_HISTORY_RECORDS];
    int count;
} HistoryList;

/**
 * @brief Initializes an empty history list.
 * @param list Pointer to HistoryList.
 */
void history_list_init(HistoryList* list);

/**
 * @brief Logs an entry modification or deletion event into the history log.
 * @param list Pointer to HistoryList.
 * @param old_entry Snapshot of the entry prior to modification or deletion.
 * @param action Action type (HIST_ACTION_MODIFIED or HIST_ACTION_DELETED).
 * @return 1 on success, 0 on failure.
 */
int history_log_event(HistoryList* list, const Entry* old_entry, HistoryAction action);

/**
 * @brief Counts the number of active deleted entries in the trash bin.
 * @param list Pointer to HistoryList.
 * @return Number of restorable deleted items.
 */
int history_trash_count(const HistoryList* list);

/**
 * @brief Restores a deleted entry from the trash bin back into an active EntryList.
 * @param list Pointer to HistoryList.
 * @param history_index Index in the history list.
 * @param target_vault Target active EntryList to restore the entry to.
 * @return 1 on successful restore, 0 on failure.
 */
int history_restore_entry(HistoryList* list, int history_index, EntryList* target_vault);

/**
 * @brief Permanently purges an entry from the trash bin.
 * @param list Pointer to HistoryList.
 * @param history_index Index in the history list.
 * @return 1 on success, 0 on failure.
 */
int history_purge_entry(HistoryList* list, int history_index);

/**
 * @brief Purges all deleted entries currently in the trash bin.
 * @param list Pointer to HistoryList.
 * @return Number of purged items.
 */
int history_empty_trash(HistoryList* list);

#ifdef __cplusplus
}
#endif

#endif /* KEHL_VAULT_HISTORY_H */
