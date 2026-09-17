#include "entry.h"

#include <stdio.h>
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

int entry_update(
        Entry entries[],
        int entry_count,
        int index,
        const char* title,
        const char* username,
        const char* password
) {
    if (entries == NULL ||
        title == NULL ||
        username == NULL ||
        password == NULL) {
        return 0;
    }

    if (index < 0 || index >= entry_count) {
        return 0;
    }

    return entry_create(
            &entries[index],
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