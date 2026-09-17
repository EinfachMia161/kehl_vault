/**
 * @file entry.c
 * @brief Implementation of Entry CRUD operations and dynamic EntryList memory management.
 */

#include "entry.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void entry_print_module_status(void) {
    printf("C Entry Module is operational.\n");
}

int entry_create(
        Entry* entry,
        const char* title,
        const char* username,
        const char* password
) {
    if (entry == NULL ||
        title == NULL ||
        username == NULL ||
        password == NULL) {
        return 0;
    }

    /* Bounded string copy with explicit null-termination guarantee */
    strncpy(entry->title, title, ENTRY_TITLE_SIZE - 1);
    entry->title[ENTRY_TITLE_SIZE - 1] = '\0';

    strncpy(entry->username, username, ENTRY_USERNAME_SIZE - 1);
    entry->username[ENTRY_USERNAME_SIZE - 1] = '\0';

    strncpy(entry->password, password, ENTRY_PASSWORD_SIZE - 1);
    entry->password[ENTRY_PASSWORD_SIZE - 1] = '\0';

    return 1;
}

int entry_add(
        Entry entries[],
        int max_entries,
        int* entry_count,
        const char* title,
        const char* username,
        const char* password
) {
    if (entries == NULL ||
        entry_count == NULL ||
        max_entries <= 0) {
        return 0;
    }

    if (*entry_count >= max_entries) {
        return 0;
    }

    int new_entry_index = *entry_count;

    int creation_successful =
            entry_create(
                    &entries[new_entry_index],
                    title,
                    username,
                    password
            );

    if (creation_successful == 0) {
        return 0;
    }

    (*entry_count)++;
    return 1;
}

Entry* entry_get(
        Entry entries[],
        int entry_count,
        int index
) {
    if (entries == NULL) {
        return NULL;
    }

    if (index < 0 || index >= entry_count) {
        return NULL;
    }

    return &entries[index];
}

int entry_update(
        Entry entries[],
        int entry_count,
        int index,
        const char* title,
        const char* username,
        const char* password
) {
    Entry* entry = entry_get(entries, entry_count, index);

    if (entry == NULL) {
        return 0;
    }

    return entry_create(
            entry,
            title,
            username,
            password
    );
}

int entry_remove(
        Entry entries[],
        int* entry_count,
        int index
) {
    if (entries == NULL ||
        entry_count == NULL) {
        return 0;
    }

    if (index < 0 || index >= *entry_count) {
        return 0;
    }

    /* Shift subsequent elements left to maintain compactness */
    for (int current_index = index;
         current_index < *entry_count - 1;
         current_index++) {
        entries[current_index] = entries[current_index + 1];
    }

    (*entry_count)--;
    return 1;
}

void entry_print(const Entry* entry) {
    if (entry == NULL) {
        printf("Entry is NULL\n");
        return;
    }

    printf("Title:    %s\n", entry->title);
    printf("Username: %s\n", entry->username);
    printf("Password: %s\n", entry->password);
}

void entry_print_list(
        const Entry entries[],
        int entry_count
) {
    if (entries == NULL || entry_count <= 0) {
        printf("The list is empty.\n");
        return;
    }

    for (int index = 0; index < entry_count; index++) {
        printf("----------------------------------------\n");
        printf("Entry [%d]\n", index);
        entry_print(&entries[index]);
    }

    printf("----------------------------------------\n");
}

int entry_list_init(
        EntryList* list,
        int initial_capacity
) {
    if (list == NULL || initial_capacity <= 0) {
        return 0;
    }

    list->entries = (Entry*)malloc(sizeof(Entry) * (size_t)initial_capacity);

    if (list->entries == NULL) {
        list->count = 0;
        list->capacity = 0;
        return 0;
    }

    list->count = 0;
    list->capacity = initial_capacity;

    return 1;
}

static int entry_list_grow(EntryList* list) {
    if (list == NULL) {
        return 0;
    }

    int new_capacity = list->capacity * 2;
    if (new_capacity <= 0) {
        new_capacity = 4;
    }

    Entry* new_entries = (Entry*)realloc(
            list->entries,
            sizeof(Entry) * (size_t)new_capacity
    );

    if (new_entries == NULL) {
        return 0;
    }

    list->entries = new_entries;
    list->capacity = new_capacity;

    return 1;
}

int entry_list_add(
        EntryList* list,
        const char* title,
        const char* username,
        const char* password
) {
    if (list == NULL ||
        title == NULL ||
        username == NULL ||
        password == NULL) {
        return 0;
    }

    if (list->count >= list->capacity) {
        int grow_successful = entry_list_grow(list);

        if (grow_successful == 0) {
            return 0;
        }
    }

    int new_index = list->count;

    int creation_successful =
            entry_create(
                    &list->entries[new_index],
                    title,
                    username,
                    password
            );

    if (creation_successful == 0) {
        return 0;
    }

    list->count++;
    return 1;
}

Entry* entry_list_get(
        EntryList* list,
        int index
) {
    if (list == NULL) {
        return NULL;
    }

    if (index < 0 || index >= list->count) {
        return NULL;
    }

    return &list->entries[index];
}

int entry_list_remove(
        EntryList* list,
        int index
) {
    if (list == NULL) {
        return 0;
    }

    if (index < 0 || index >= list->count) {
        return 0;
    }

    for (int current_index = index;
         current_index < list->count - 1;
         current_index++) {
        list->entries[current_index] = list->entries[current_index + 1];
    }

    list->count--;
    return 1;
}

void entry_list_print(
        const EntryList* list
) {
    if (list == NULL || list->count <= 0) {
        printf("The dynamic list is empty.\n");
        return;
    }

    entry_print_list(list->entries, list->count);
}

void entry_list_destroy(
        EntryList* list
) {
    if (list == NULL) {
        return;
    }

    if (list->entries != NULL) {
        free(list->entries);
        list->entries = NULL;
    }

    list->count = 0;
    list->capacity = 0;
}
