#ifndef KEHL_VAULT_STORAGE_H
#define KEHL_VAULT_STORAGE_H

#include "entry.h"
#include "crypto.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VAULT_MAGIC 0x564B4548 /* "KEHV" */
#define VAULT_VERSION_LEGACY 1
#define VAULT_VERSION_ENCRYPTED 2
#define VAULT_CURRENT_VERSION 2

#define VAULT_PBKDF2_ITERATIONS 100000

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t entry_count;
    uint8_t salt[CRYPTO_SALT_SIZE];
    uint8_t nonce[CRYPTO_NONCE_SIZE];
    uint8_t auth_tag[CRYPTO_SHA256_HASH_SIZE];
} VaultHeader;

typedef enum {
    VAULT_SUCCESS = 1,
    VAULT_ERR_INVALID_PARAM = -1,
    VAULT_ERR_FILE_IO = -2,
    VAULT_ERR_INVALID_HEADER = -3,
    VAULT_ERR_INVALID_PASSWORD = -4,
    VAULT_ERR_CORRUPT_DATA = -5,
    VAULT_ERR_ALLOCATION = -6
} VaultStorageStatus;

int vault_save_to_file(const EntryList* list, const char* filepath, const char* master_password);
int vault_load_from_file(EntryList* list, const char* filepath, const char* master_password);
int vault_is_encrypted_file(const char* filepath, int* is_encrypted);

#ifdef __cplusplus
}
#endif

#endif
