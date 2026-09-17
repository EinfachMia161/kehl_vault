/**
 * @file storage.c
 * @brief Implementation of encrypted binary persistence and atomic file replacement.
 */

#include "storage.h"
#include "password.h"
#include "secure_mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ALLOWED_ENTRIES 1000000

int vault_is_encrypted_file(const char* filepath, int* is_encrypted) {
    if (filepath == NULL || is_encrypted == NULL) {
        return 0;
    }

    FILE* file = fopen(filepath, "rb");
    if (file == NULL) {
        return 0;
    }

    VaultHeader header;
    if (fread(&header, sizeof(VaultHeader), 1, file) != 1) {
        fclose(file);
        return 0;
    }
    fclose(file);

    if (header.magic != VAULT_MAGIC) {
        return 0;
    }

    *is_encrypted = (header.version >= VAULT_VERSION_ENCRYPTED);
    return 1;
}

int vault_save_to_file(const EntryList* list, const char* filepath, const char* master_password) {
    if (list == NULL || filepath == NULL || list->count < 0) {
        return VAULT_ERR_INVALID_PARAM;
    }

    if (list->count > 0 && list->entries == NULL) {
        return VAULT_ERR_INVALID_PARAM;
    }

    char tmp_filepath[1024];
    int written = snprintf(tmp_filepath, sizeof(tmp_filepath), "%s.tmp", filepath);
    if (written < 0 || (size_t)written >= sizeof(tmp_filepath)) {
        return VAULT_ERR_FILE_IO;
    }

    FILE* file = fopen(tmp_filepath, "wb");
    if (file == NULL) {
        return VAULT_ERR_FILE_IO;
    }

    VaultHeader header;
    memset(&header, 0, sizeof(header));
    header.magic = VAULT_MAGIC;
    header.entry_count = (uint32_t)list->count;

    int use_encryption = (master_password != NULL && strlen(master_password) > 0);
    size_t payload_size = sizeof(Entry) * (size_t)list->count;
    uint8_t* payload_to_write = NULL;

    if (use_encryption) {
        header.version = VAULT_VERSION_ENCRYPTED;

        if (!password_get_secure_random_bytes(header.salt, CRYPTO_SALT_SIZE) ||
            !password_get_secure_random_bytes(header.nonce, CRYPTO_NONCE_SIZE)) {
            fclose(file);
            remove(tmp_filepath);
            return VAULT_ERR_ALLOCATION;
        }

        uint8_t derived_keys[64]; /* 32 bytes enc_key + 32 bytes auth_key */
        if (!crypto_pbkdf2_sha256(
                master_password,
                strlen(master_password),
                header.salt,
                CRYPTO_SALT_SIZE,
                VAULT_PBKDF2_ITERATIONS,
                derived_keys,
                sizeof(derived_keys))) {
            secure_memzero(derived_keys, sizeof(derived_keys));
            fclose(file);
            remove(tmp_filepath);
            return VAULT_ERR_ALLOCATION;
        }

        uint8_t* enc_key = derived_keys;
        uint8_t* auth_key = derived_keys + 32;

        if (list->count > 0) {
            payload_to_write = (uint8_t*)malloc(payload_size);
            if (payload_to_write == NULL) {
                secure_memzero(derived_keys, sizeof(derived_keys));
                fclose(file);
                remove(tmp_filepath);
                return VAULT_ERR_ALLOCATION;
            }

            crypto_chacha20_xor(
                enc_key,
                header.nonce,
                1,
                (const uint8_t*)list->entries,
                payload_to_write,
                payload_size
            );

            crypto_hmac_sha256(auth_key, 32, payload_to_write, payload_size, header.auth_tag);
        } else {
            /* Empty list tag calculation */
            crypto_hmac_sha256(auth_key, 32, (const uint8_t*)"", 0, header.auth_tag);
        }

        secure_memzero(derived_keys, sizeof(derived_keys));
    } else {
        header.version = VAULT_VERSION_LEGACY;
    }

    if (fwrite(&header, sizeof(VaultHeader), 1, file) != 1) {
        if (payload_to_write) free(payload_to_write);
        fclose(file);
        remove(tmp_filepath);
        return VAULT_ERR_FILE_IO;
    }

    if (list->count > 0) {
        const void* data_ptr = use_encryption ? (const void*)payload_to_write : (const void*)list->entries;
        size_t items_written = fwrite(data_ptr, 1, payload_size, file);
        if (items_written != payload_size) {
            if (payload_to_write) free(payload_to_write);
            fclose(file);
            remove(tmp_filepath);
            return VAULT_ERR_FILE_IO;
        }
    }

    if (payload_to_write) {
        secure_memzero(payload_to_write, payload_size);
        free(payload_to_write);
    }

    if (fflush(file) != 0 || fclose(file) != 0) {
        remove(tmp_filepath);
        return VAULT_ERR_FILE_IO;
    }

    remove(filepath);
    if (rename(tmp_filepath, filepath) != 0) {
        remove(tmp_filepath);
        return VAULT_ERR_FILE_IO;
    }

    return VAULT_SUCCESS;
}

int vault_load_from_file(EntryList* list, const char* filepath, const char* master_password) {
    if (list == NULL || filepath == NULL) {
        return VAULT_ERR_INVALID_PARAM;
    }

    FILE* file = fopen(filepath, "rb");
    if (file == NULL) {
        return VAULT_ERR_FILE_IO;
    }

    VaultHeader header;
    if (fread(&header, sizeof(VaultHeader), 1, file) != 1) {
        fclose(file);
        return VAULT_ERR_INVALID_HEADER;
    }

    if (header.magic != VAULT_MAGIC) {
        fclose(file);
        return VAULT_ERR_INVALID_HEADER;
    }

    if (header.version != VAULT_VERSION_LEGACY && header.version != VAULT_VERSION_ENCRYPTED) {
        fclose(file);
        return VAULT_ERR_INVALID_HEADER;
    }

    if (header.entry_count > MAX_ALLOWED_ENTRIES) {
        fclose(file);
        return VAULT_ERR_INVALID_HEADER;
    }

    int initial_cap = (int)header.entry_count;
    if (initial_cap <= 0) {
        initial_cap = 4;
    }

    EntryList new_list;
    if (!entry_list_init(&new_list, initial_cap)) {
        fclose(file);
        return VAULT_ERR_ALLOCATION;
    }

    size_t payload_size = sizeof(Entry) * (size_t)header.entry_count;

    if (header.version == VAULT_VERSION_ENCRYPTED) {
        if (master_password == NULL || strlen(master_password) == 0) {
            entry_list_destroy(&new_list);
            fclose(file);
            return VAULT_ERR_INVALID_PASSWORD;
        }

        uint8_t derived_keys[64];
        if (!crypto_pbkdf2_sha256(
                master_password,
                strlen(master_password),
                header.salt,
                CRYPTO_SALT_SIZE,
                VAULT_PBKDF2_ITERATIONS,
                derived_keys,
                sizeof(derived_keys))) {
            secure_memzero(derived_keys, sizeof(derived_keys));
            entry_list_destroy(&new_list);
            fclose(file);
            return VAULT_ERR_ALLOCATION;
        }

        uint8_t* enc_key = derived_keys;
        uint8_t* auth_key = derived_keys + 32;

        if (header.entry_count > 0) {
            uint8_t* encrypted_payload = (uint8_t*)malloc(payload_size);
            if (encrypted_payload == NULL) {
                secure_memzero(derived_keys, sizeof(derived_keys));
                entry_list_destroy(&new_list);
                fclose(file);
                return VAULT_ERR_ALLOCATION;
            }

            size_t bytes_read = fread(encrypted_payload, 1, payload_size, file);
            if (bytes_read != payload_size) {
                secure_memzero(encrypted_payload, payload_size);
                free(encrypted_payload);
                secure_memzero(derived_keys, sizeof(derived_keys));
                entry_list_destroy(&new_list);
                fclose(file);
                return VAULT_ERR_CORRUPT_DATA;
            }

            uint8_t computed_tag[CRYPTO_SHA256_HASH_SIZE];
            crypto_hmac_sha256(auth_key, 32, encrypted_payload, payload_size, computed_tag);

            if (!crypto_constant_time_equals(computed_tag, header.auth_tag, CRYPTO_SHA256_HASH_SIZE)) {
                secure_memzero(encrypted_payload, payload_size);
                free(encrypted_payload);
                secure_memzero(derived_keys, sizeof(derived_keys));
                entry_list_destroy(&new_list);
                fclose(file);
                return VAULT_ERR_INVALID_PASSWORD;
            }

            crypto_chacha20_xor(
                enc_key,
                header.nonce,
                1,
                encrypted_payload,
                (uint8_t*)new_list.entries,
                payload_size
            );

            secure_memzero(encrypted_payload, payload_size);
            free(encrypted_payload);
        } else {
            uint8_t computed_tag[CRYPTO_SHA256_HASH_SIZE];
            crypto_hmac_sha256(auth_key, 32, (const uint8_t*)"", 0, computed_tag);

            if (!crypto_constant_time_equals(computed_tag, header.auth_tag, CRYPTO_SHA256_HASH_SIZE)) {
                secure_memzero(derived_keys, sizeof(derived_keys));
                entry_list_destroy(&new_list);
                fclose(file);
                return VAULT_ERR_INVALID_PASSWORD;
            }
        }

        secure_memzero(derived_keys, sizeof(derived_keys));
    } else {
        /* Legacy version 1 unencrypted */
        if (header.entry_count > 0) {
            size_t items_read = fread(new_list.entries, sizeof(Entry), (size_t)header.entry_count, file);
            if (items_read != (size_t)header.entry_count) {
                entry_list_destroy(&new_list);
                fclose(file);
                return VAULT_ERR_CORRUPT_DATA;
            }
        }
    }

    /* Null-termination safety on loaded entries */
    for (uint32_t i = 0; i < header.entry_count; ++i) {
        new_list.entries[i].title[ENTRY_TITLE_SIZE - 1] = '\0';
        new_list.entries[i].username[ENTRY_USERNAME_SIZE - 1] = '\0';
        new_list.entries[i].password[ENTRY_PASSWORD_SIZE - 1] = '\0';
    }
    new_list.count = (int)header.entry_count;

    fclose(file);
    *list = new_list;
    return VAULT_SUCCESS;
}
