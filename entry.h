#ifndef KEHL_VAULT_ENTRY_H
#define KEHL_VAULT_ENTRY_H

#ifdef __cplusplus
extern "C" {
#endif

#define ENTRY_TITLE_SIZE 64
#define ENTRY_USERNAME_SIZE 64
#define ENTRY_PASSWORD_SIZE 64

typedef struct {
    char title[ENTRY_TITLE_SIZE];
    char username[ENTRY_USERNAME_SIZE];
    char password[ENTRY_PASSWORD_SIZE];
} Entry;

typedef struct {
    Entry* entries;
    int count;
    int capacity;
} EntryList;

void entry_print_module_status(void);

int entry_create(
        Entry* entry,
        const char* title,
        const char* username,
        const char* password
);

int entry_add(
        Entry entries[],
        int max_entries,
        int* entry_count,
        const char* title,
        const char* username,
        const char* password
);

Entry* entry_get(
        Entry entries[],
        int entry_count,
        int index
);

int entry_update(
        Entry entries[],
        int entry_count,
        int index,
        const char* title,
        const char* username,
        const char* password
);

int entry_remove(
        Entry entries[],
        int* entry_count,
        int index
);

void entry_print(const Entry* entry);

void entry_print_list(
        const Entry entries[],
        int entry_count
);

int entry_list_init(
        EntryList* list,
        int initial_capacity
);

int entry_list_add(
        EntryList* list,
        const char* title,
        const char* username,
        const char* password
);

Entry* entry_list_get(
        EntryList* list,
        int index
);

int entry_list_remove(
        EntryList* list,
        int index
);

void entry_list_print(
        const EntryList* list
);

void entry_list_destroy(
        EntryList* list
);

#ifdef __cplusplus
}
#endif

#endif