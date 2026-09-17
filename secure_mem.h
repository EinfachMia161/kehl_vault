/**
 * @file secure_mem.h
 * @brief Volatile memory wiping and secure zeroization primitives for kehl-vault.
 *
 * Guarantees that sensitive credentials, derived encryption keys, and plaintext
 * buffers are thoroughly scrubbed from process RAM without compiler optimization dead-store removal.
 */

#ifndef KEHL_VAULT_SECURE_MEM_H
#define KEHL_VAULT_SECURE_MEM_H

#include "entry.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Cryptographically wipes a raw block of memory to zeros.
 * @param ptr Pointer to memory buffer.
 * @param size Number of bytes to zeroize.
 */
void secure_memzero(void* ptr, size_t size);

/**
 * @brief Wipes and zeroes out a null-terminated string buffer.
 * @param str Null-terminated string buffer.
 */
void secure_wipe_string(char* str);

/**
 * @brief Securely zeroes all sensitive text fields of an Entry structure.
 * @param entry Pointer to Entry to scrub.
 */
void secure_wipe_entry(Entry* entry);

/**
 * @brief Securely zeroes all entries inside an EntryList.
 * @param list Pointer to EntryList.
 */
void secure_wipe_list(EntryList* list);

#ifdef __cplusplus
}
#endif

#endif /* KEHL_VAULT_SECURE_MEM_H */
