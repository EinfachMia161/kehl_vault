/**
 * @file history.c
 * @brief Implementation of change history and trash bin operations.
 */

#include "history.h"
#include <string.h>

void history_list_init(HistoryList* list) {
    if (!list) return;
    memset(list, 0, sizeof(HistoryList));
}

int history_log_event(HistoryList* list, const Entry* old_entry, HistoryAction action) {
    if (!list || !old_entry) {
        return 0;
    }

    /* Shift older records if array capacity is reached */
    if (list->count >= MAX_HISTORY_RECORDS) {
        memmove(&list->records[0], &list->records[1], sizeof(HistoryRecord) * (MAX_HISTORY_RECORDS - 1));
        list->count = MAX_HISTORY_RECORDS - 1;
    }

    HistoryRecord* rec = &list->records[list->count];
    rec->entry = *old_entry;
    rec->timestamp = (uint64_t)time(NULL);
    rec->action = action;
    rec->is_purged = 0;

    list->count++;
    return 1;
}

int history_trash_count(const HistoryList* list) {
    if (!list) return 0;
    int count = 0;
    for (int i = 0; i < list->count; ++i) {
        if (list->records[i].action == HIST_ACTION_DELETED && !list->records[i].is_purged) {
            count++;
        }
    }
    return count;
}

int history_restore_entry(HistoryList* list, int history_index, EntryList* target_vault) {
    if (!list || !target_vault || history_index < 0 || history_index >= list->count) {
        return 0;
    }

    HistoryRecord* rec = &list->records[history_index];
    if (rec->action != HIST_ACTION_DELETED || rec->is_purged) {
        return 0;
    }

    if (!entry_list_add(target_vault, rec->entry.title, rec->entry.username, rec->entry.password)) {
        return 0;
    }

    rec->is_purged = 1;
    return 1;
}

int history_purge_entry(HistoryList* list, int history_index) {
    if (!list || history_index < 0 || history_index >= list->count) {
        return 0;
    }
    list->records[history_index].is_purged = 1;
    return 1;
}

int history_empty_trash(HistoryList* list) {
    if (!list) return 0;
    int purged = 0;
    for (int i = 0; i < list->count; ++i) {
        if (list->records[i].action == HIST_ACTION_DELETED && !list->records[i].is_purged) {
            list->records[i].is_purged = 1;
            purged++;
        }
    }
    return purged;
}
