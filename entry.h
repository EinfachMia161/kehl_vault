/**
 * @file entry.h
 * @brief Core in-memory data structures and dynamic array management for vault entries.
 *
 * Defines the fundamental Entry structure representing stored credentials (title, username, password)
 * and the EntryList dynamic container with automatic growth, bounds checking, and safe memory deallocation.
 */

#ifndef KEHL_VAULT_ENTRY_H
#define KEHL_VAULT_ENTRY_H

#ifdef __cplusplus
extern "C" {
#endif

#define ENTRY_TITLE_SIZE 64
#define ENTRY_USERNAME_SIZE 64
#define ENTRY_PASSWORD_SIZE 64

/**
 * @brief Represents a single credential record with fixed-size null-terminated buffers.
 */
typedef struct {
    char title[ENTRY_TITLE_SIZE];       /**< Descriptive service or website title */
    char username[ENTRY_USERNAME_SIZE]; /**< Account identifier or login username */
    char password[ENTRY_PASSWORD_SIZE]; /**< Account password or secret key */
} Entry;

/**
 * @brief Resizable dynamic array container holding Entry items on the heap.
 */
typedef struct {
    Entry* entries; /**< Pointer to dynamic heap-allocated entry buffer */
    int count;      /**< Current number of stored entries */
    int capacity;   /**< Total allocated capacity slots */
} EntryList;

/**
 * @brief Prints module status information for diagnostic confirmation.
 */
void entry_print_module_status(void);

/**
 * @brief Creates and initializes an Entry record with validated null-terminated fields.
 * @param entry Pointer to target Entry to populate.
 * @param title Title string.
 * @param username Username string.
 * @param password Password string.
 * @return 1 on success, 0 on invalid parameters.
 */
int entry_create(
        Entry* entry,
        const char* title,
        const char* username,
        const char* password
);

/**
 * @brief Appends an entry to a fixed-size array.
 * @param entries Array of Entry objects.
 * @param max_entries Maximum capacity of the array.
 * @param entry_count Pointer to current count of items.
 * @param title Service title.
 * @param username Account username.
 * @param password Account password.
 * @return 1 on success, 0 on failure.
 */
int entry_add(
        Entry entries[],
        int max_entries,
        int* entry_count,
        const char* title,
        const char* username,
        const char* password
);

/**
 * @brief Retrieves a pointer to an Entry by index from a fixed array.
 * @param entries Array of Entry objects.
 * @param entry_count Total number of valid entries.
 * @param index 0-based target index.
 * @return Pointer to Entry, or NULL if index is out of bounds.
 */
Entry* entry_get(
        Entry entries[],
        int entry_count,
        int index
);

/**
 * @brief Updates fields of an existing entry in a fixed array.
 * @param entries Array of Entry objects.
 * @param entry_count Total count of entries.
 * @param index Target index to update.
 * @param title New title (or NULL to keep unchanged).
 * @param username New username (or NULL to keep unchanged).
 * @param password New password (or NULL to keep unchanged).
 * @return 1 on success, 0 on failure.
 */
int entry_update(
        Entry entries[],
        int entry_count,
        int index,
        const char* title,
        const char* username,
        const char* password
);

/**
 * @brief Removes an entry at the specified index from a fixed array, shifting subsequent elements.
 * @param entries Array of Entry objects.
 * @param entry_count Pointer to count of items.
 * @param index Index of element to delete.
 * @return 1 on success, 0 if index is out of range.
 */
int entry_remove(
        Entry entries[],
        int* entry_count,
        int index
);

/**
 * @brief Prints an individual entry to standard output.
 * @param entry Pointer to Entry to display.
 */
void entry_print(const Entry* entry);

/**
 * @brief Prints a list of entries to standard output.
 * @param entries Array of Entry objects.
 * @param entry_count Number of entries in array.
 */
void entry_print_list(
        const Entry entries[],
        int entry_count
);

/**
 * @brief Initializes a dynamic EntryList with specified initial capacity.
 * @param list Pointer to EntryList to initialize.
 * @param initial_capacity Initial number of slots to allocate (minimum 1).
 * @return 1 on successful allocation, 0 on failure.
 */
int entry_list_init(
        EntryList* list,
        int initial_capacity
);

/**
 * @brief Appends an entry to dynamic EntryList, automatically resizing if full.
 * @param list Pointer to EntryList.
 * @param title Title string.
 * @param username Username string.
 * @param password Password string.
 * @return 1 on success, 0 on failure.
 */
int entry_list_add(
        EntryList* list,
        const char* title,
        const char* username,
        const char* password
);

/**
 * @brief Retrieves an entry by index from a dynamic EntryList.
 * @param list Pointer to EntryList.
 * @param index 0-based index.
 * @return Pointer to Entry, or NULL if out of bounds.
 */
Entry* entry_list_get(
        EntryList* list,
        int index
);

/**
 * @brief Removes an entry by index from a dynamic EntryList with compact shifting.
 * @param list Pointer to EntryList.
 * @param index 0-based index of item to remove.
 * @return 1 on success, 0 on invalid index.
 */
int entry_list_remove(
        EntryList* list,
        int index
);

/**
 * @brief Formats and displays all entries in dynamic EntryList.
 * @param list Pointer to EntryList.
 */
void entry_list_print(
        const EntryList* list
);

/**
 * @brief Deallocates heap memory associated with dynamic EntryList and resets pointers.
 * @param list Pointer to EntryList to free.
 */
void entry_list_destroy(
        EntryList* list
);

#ifdef __cplusplus
}
#endif

#endif /* KEHL_VAULT_ENTRY_H */