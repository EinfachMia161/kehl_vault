/**
 * @file secure_mem.c
 * @brief Implementation of compiler-safe memory scrubbing functions.
 */

#include "secure_mem.h"
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

void secure_memzero(void* ptr, size_t size) {
    if (!ptr || size == 0) return;

#if defined(_WIN32) || defined(_WIN64)
    SecureZeroMemory(ptr, size);
#else
    volatile unsigned char* p = (volatile unsigned char*)ptr;
    while (size--) {
        *p++ = 0;
    }
#endif
}

void secure_wipe_string(char* str) {
    if (!str) return;
    size_t len = strlen(str);
    secure_memzero(str, len);
    str[0] = '\0';
}

void secure_wipe_entry(Entry* entry) {
    if (!entry) return;
    secure_memzero(entry->title, sizeof(entry->title));
    secure_memzero(entry->username, sizeof(entry->username));
    secure_memzero(entry->password, sizeof(entry->password));
}

void secure_wipe_list(EntryList* list) {
    if (!list || !list->entries) return;
    for (int i = 0; i < list->count; ++i) {
        secure_wipe_entry(&list->entries[i]);
    }
}
