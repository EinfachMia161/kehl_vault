/**
 * @file storage.h
 * @brief Encrypted and atomic disk persistence layer for kehl-vault databases.
 *
 * Implements binary vault serialization, header integrity checks, and Authenticated
 * Encryption (PBKDF2-HMAC-SHA256 key derivation + ChaCha20 encryption + HMAC-SHA256 authentication).
 */

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

/**
 * @brief Binary file header structure stored at the beginning of every vault file.
 */
typedef struct {
    uint32_t magic;                            /**< Magic identifier (0x564B4548) */
    uint32_t version;                          /**< File format version (1 or 2) */
    uint32_t entry_count;                      /**< Total number of records stored */
    uint8_t  salt[CRYPTO_SALT_SIZE];           /**< Cryptographic salt for PBKDF2 */
    uint8_t  nonce[CRYPTO_NONCE_SIZE];         /**< Unique 96-bit nonce for ChaCha20 */
    uint8_t  auth_tag[CRYPTO_SHA256_HASH_SIZE];/**< HMAC-SHA256 ciphertext auth tag */
} VaultHeader;

/**
 * @brief Storage operation return status codes.
 */
typedef enum {
    VAULT_SUCCESS = 1,
    VAULT_ERR_INVALID_PARAM = -1,
    VAULT_ERR_FILE_IO = -2,
    VAULT_ERR_INVALID_HEADER = -3,
    VAULT_ERR_INVALID_PASSWORD = -4,
    VAULT_ERR_CORRUPT_DATA = -5,
    VAULT_ERR_ALLOCATION = -6
} VaultStorageStatus;

/**
 * @brief Atomically writes vault entries to disk with encryption and HMAC tag.
 * @param list Pointer to EntryList containing credentials to persist.
 * @param filepath Destination file path.
 * @param master_password Master encryption password (or NULL/empty for legacy plain mode).
 * @return VAULT_SUCCESS on success, or a negative VaultStorageStatus error code.
 */
int vault_save_to_file(const EntryList* list, const char* filepath, const char* master_password);

/**
 * @brief Loads, verifies authentication tag, decrypts, and deserializes entries from disk.
 * @param list Pointer to destination EntryList.
 * @param filepath Source file path.
 * @param master_password Master password to decrypt.
 * @return VAULT_SUCCESS on success, or a negative VaultStorageStatus error code.
 */
int vault_load_from_file(EntryList* list, const char* filepath, const char* master_password);

/**
 * @brief Inspects a vault file header to check if it requires a master password.
 * @param filepath Path to vault file.
 * @param is_encrypted Pointer to int receiving 1 if encrypted, 0 if plain.
 * @return 1 on successful header read, 0 if file could not be read or is invalid.
 */
int vault_is_encrypted_file(const char* filepath, int* is_encrypted);

#ifdef __cplusplus
}
#endif

#endif /* KEHL_VAULT_STORAGE_H */
