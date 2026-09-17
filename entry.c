#include "entry.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void entry_print_module_status(void) {
    printf("Das C-Entry-Modul funktioniert.\n");
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

    for (int current_index = index;
         current_index < *entry_count - 1;
         current_index++) {
        entries[current_index] = entries[current_index + 1];
    }

    (*entry_count)--;

    Entry empty_entry = {0};
    entries[*entry_count] = empty_entry;

    return 1;
}

void entry_print(const Entry* entry) {
    if (entry == NULL) {
        return;
    }

    printf("Titel: %s\n", entry->title);
    printf("Benutzername: %s\n", entry->username);
}

void entry_print_list(
        const Entry entries[],
        int entry_count
) {
    if (entries == NULL || entry_count <= 0) {
        printf("Keine Entries vorhanden.\n");
        return;
    }

    for (int index = 0; index < entry_count; index++) {
        printf("Entry %d\n", index + 1);
        printf("-------\n");

        entry_print(&entries[index]);

        printf("\n");
    }
}

int entry_list_init(
        EntryList* list,
        int initial_capacity
) {
    if (list == NULL || initial_capacity <= 0) {
        return 0;
    }

    list->entries =
            malloc(sizeof(Entry) * initial_capacity);

    if (list->entries == NULL) {
        list->count = 0;
        list->capacity = 0;
        return 0;
    }

    list->count = 0;
    list->capacity = initial_capacity;

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

    if (list->entries == NULL ||
        list->capacity <= 0) {
        return 0;
    }

    if (list->count >= list->capacity) {
        int new_capacity = list->capacity * 2;

        Entry* resized_entries =
                realloc(
                        list->entries,
                        sizeof(Entry) * new_capacity
                );

        if (resized_entries == NULL) {
            return 0;
        }

        list->entries = resized_entries;
        list->capacity = new_capacity;
    }

    int creation_successful =
            entry_create(
                    &list->entries[list->count],
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
    if (list == NULL ||
        list->entries == NULL) {
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
    if (list == NULL ||
        list->entries == NULL) {
        return 0;
    }

    if (index < 0 || index >= list->count) {
        return 0;
    }

    for (int current_index = index;
         current_index < list->count - 1;
         current_index++) {
        list->entries[current_index] =
                list->entries[current_index + 1];
    }

    list->count--;

    Entry empty_entry = {0};
    list->entries[list->count] = empty_entry;

    return 1;
}

void entry_list_print(
        const EntryList* list
) {
    if (list == NULL ||
        list->entries == NULL ||
        list->count <= 0) {
        printf("Keine Entries vorhanden.\n");
        return;
    }

    printf(
            "Entries: %d/%d\n\n",
            list->count,
            list->capacity
    );

    for (int index = 0; index < list->count; index++) {
        printf("Entry %d\n", index + 1);
        printf("-------\n");

        entry_print(&list->entries[index]);

        printf("\n");
    }
}

void entry_list_destroy(
        EntryList* list
) {
    if (list == NULL) {
        return;
    }

    free(list->entries);

    list->entries = NULL;
    list->count = 0;
    list->capacity = 0;
}