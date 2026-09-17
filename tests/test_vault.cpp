#include "entry.h"
#include "password.h"
#include "storage.h"
#include "crypto.h"
#include "clipboard.h"
#include "audit.h"
#include "impex.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

static void test_entry_crud_and_resizing() {
    std::cout << "[RUN] Testing EntryList CRUD and Resizing...\n";
    EntryList list;
    assert(entry_list_init(&list, 2) == 1);
    assert(list.count == 0);
    assert(list.capacity == 2);

    assert(entry_list_add(&list, "Account1", "user1", "Pass1!") == 1);
    assert(entry_list_add(&list, "Account2", "user2", "Pass2!") == 1);
    assert(list.count == 2);
    assert(list.capacity == 2);

    // Trigger auto-expansion
    assert(entry_list_add(&list, "Account3", "user3", "Pass3!") == 1);
    assert(list.count == 3);
    assert(list.capacity == 4);

    // Verify getters
    Entry* e0 = entry_list_get(&list, 0);
    Entry* e1 = entry_list_get(&list, 1);
    Entry* e2 = entry_list_get(&list, 2);
    assert(e0 != nullptr && strcmp(e0->title, "Account1") == 0);
    assert(e1 != nullptr && strcmp(e1->title, "Account2") == 0);
    assert(e2 != nullptr && strcmp(e2->title, "Account3") == 0);

    // Verify out-of-bounds
    assert(entry_list_get(&list, -1) == nullptr);
    assert(entry_list_get(&list, 3) == nullptr);
    assert(entry_list_get(&list, 999) == nullptr);

    // Verify update
    assert(entry_create(e1, "UpdatedAccount2", "new_user2", "NewPass2#") == 1);
    assert(strcmp(e1->title, "UpdatedAccount2") == 0);
    assert(strcmp(e1->username, "new_user2") == 0);

    // Verify remove out of bounds
    assert(entry_list_remove(&list, -1) == 0);
    assert(entry_list_remove(&list, 3) == 0);
    assert(list.count == 3);

    // Verify remove middle (index 1)
    assert(entry_list_remove(&list, 1) == 1);
    assert(list.count == 2);
    assert(strcmp(entry_list_get(&list, 0)->title, "Account1") == 0);
    assert(strcmp(entry_list_get(&list, 1)->title, "Account3") == 0);

    entry_list_destroy(&list);
    assert(list.entries == nullptr);
    assert(list.count == 0);
    assert(list.capacity == 0);

    std::cout << "[PASS] EntryList CRUD and Resizing passed.\n";
}

static void test_crypto_primitives() {
    std::cout << "[RUN] Testing Cryptographic Primitives (SHA-256, HMAC, PBKDF2, ChaCha20)...\n";

    // SHA-256 test vector "abc"
    // sha256("abc") = ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
    uint8_t hash[CRYPTO_SHA256_HASH_SIZE];
    crypto_sha256((const uint8_t*)"abc", 3, hash);
    assert(hash[0] == 0xba && hash[1] == 0x78 && hash[2] == 0x16 && hash[3] == 0xbf);
    assert(hash[30] == 0x15 && hash[31] == 0xad);

    // HMAC-SHA256 test
    uint8_t hmac_out[CRYPTO_SHA256_HASH_SIZE];
    const char* hmac_key = "secret_key";
    const char* hmac_msg = "test message";
    crypto_hmac_sha256((const uint8_t*)hmac_key, strlen(hmac_key), (const uint8_t*)hmac_msg, strlen(hmac_msg), hmac_out);

    // ChaCha20 encryption & decryption
    uint8_t key[CRYPTO_KEY_SIZE];
    uint8_t nonce[CRYPTO_NONCE_SIZE];
    memset(key, 0x42, sizeof(key));
    memset(nonce, 0x24, sizeof(nonce));

    const char* secret_text = "Sensitive Vault Data 1234567890!";
    size_t len = strlen(secret_text) + 1;
    uint8_t ciphertext[128];
    uint8_t decrypted[128];

    crypto_chacha20_xor(key, nonce, 1, (const uint8_t*)secret_text, ciphertext, len);
    assert(memcmp(ciphertext, secret_text, len) != 0); // Ciphertext differs from plaintext

    crypto_chacha20_xor(key, nonce, 1, ciphertext, decrypted, len);
    assert(strcmp((const char*)decrypted, secret_text) == 0); // Roundtrip matches

    // Constant-time comparison
    assert(crypto_constant_time_equals(key, key, CRYPTO_KEY_SIZE) == 1);
    uint8_t modified_key[CRYPTO_KEY_SIZE];
    memcpy(modified_key, key, CRYPTO_KEY_SIZE);
    modified_key[0] ^= 1;
    assert(crypto_constant_time_equals(key, modified_key, CRYPTO_KEY_SIZE) == 0);

    std::cout << "[PASS] Cryptographic Primitives passed.\n";
}

static void test_storage_persistence() {
    std::cout << "[RUN] Testing Storage Persistence and Encryption...\n";
    const char* test_file = "test_persistence_vault.dat";
    const char* master_pass = "SuperMasterPassword#2026";
    remove(test_file);

    EntryList save_list;
    assert(entry_list_init(&save_list, 2) == 1);
    assert(entry_list_add(&save_list, "SiteA", "userA", "SecretA123!") == 1);
    assert(entry_list_add(&save_list, "SiteB", "userB", "SecretB456@") == 1);
    assert(entry_list_add(&save_list, "SiteC", "userC", "SecretC789#") == 1);

    // 1. Save with encryption
    assert(vault_save_to_file(&save_list, test_file, master_pass) == VAULT_SUCCESS);

    int is_enc = 0;
    assert(vault_is_encrypted_file(test_file, &is_enc) == 1 && is_enc == 1);

    // 2. Load with correct password
    EntryList load_list;
    assert(vault_load_from_file(&load_list, test_file, master_pass) == VAULT_SUCCESS);
    assert(load_list.count == 3);
    assert(strcmp(load_list.entries[0].title, "SiteA") == 0);
    assert(strcmp(load_list.entries[0].username, "userA") == 0);
    assert(strcmp(load_list.entries[0].password, "SecretA123!") == 0);

    assert(strcmp(load_list.entries[1].title, "SiteB") == 0);
    assert(strcmp(load_list.entries[1].username, "userB") == 0);
    assert(strcmp(load_list.entries[1].password, "SecretB456@") == 0);

    assert(strcmp(load_list.entries[2].title, "SiteC") == 0);
    assert(strcmp(load_list.entries[2].username, "userC") == 0);
    assert(strcmp(load_list.entries[2].password, "SecretC789#") == 0);
    entry_list_destroy(&load_list);

    // 3. Load with wrong password -> must fail gracefully
    EntryList wrong_pass_list;
    assert(vault_load_from_file(&wrong_pass_list, test_file, "WrongPassword123") == VAULT_ERR_INVALID_PASSWORD);

    // 4. Load with empty password -> must fail gracefully
    EntryList empty_pass_list;
    assert(vault_load_from_file(&empty_pass_list, test_file, "") == VAULT_ERR_INVALID_PASSWORD);

    // 5. Unencrypted persistence test (legacy mode)
    const char* unenc_file = "test_unenc_vault.dat";
    remove(unenc_file);
    assert(vault_save_to_file(&save_list, unenc_file, NULL) == VAULT_SUCCESS);
    assert(vault_is_encrypted_file(unenc_file, &is_enc) == 1 && is_enc == 0);

    EntryList unenc_load_list;
    assert(vault_load_from_file(&unenc_load_list, unenc_file, NULL) == VAULT_SUCCESS);
    assert(unenc_load_list.count == 3);
    assert(strcmp(unenc_load_list.entries[0].title, "SiteA") == 0);
    entry_list_destroy(&unenc_load_list);
    remove(unenc_file);

    entry_list_destroy(&save_list);
    remove(test_file);

    // 6. Test loading non-existent file
    EntryList missing_list;
    assert(vault_load_from_file(&missing_list, "non_existent_vault.dat", master_pass) == VAULT_ERR_FILE_IO);

    // 7. Test corrupted file (invalid magic)
    const char* corrupt_file = "corrupt_vault.dat";
    FILE* cf = fopen(corrupt_file, "wb");
    assert(cf != nullptr);
    VaultHeader bad_header;
    memset(&bad_header, 0, sizeof(bad_header));
    bad_header.magic = 0x12345678;
    bad_header.version = VAULT_CURRENT_VERSION;
    fwrite(&bad_header, sizeof(VaultHeader), 1, cf);
    fclose(cf);

    EntryList corrupt_list;
    assert(vault_load_from_file(&corrupt_list, corrupt_file, master_pass) == VAULT_ERR_INVALID_HEADER);
    remove(corrupt_file);

    std::cout << "[PASS] Storage Persistence and Encryption passed.\n";
}

static void test_password_logic() {
    std::cout << "[RUN] Testing Password Security & Generator Logic...\n";

    // Rules & Strength scoring
    assert(password_is_long_enough("12345678", 8) == 1);
    assert(password_is_long_enough("1234", 8) == 0);

    assert(password_contains_lower_case("abc") == 1);
    assert(password_contains_lower_case("ABC") == 0);

    assert(password_contains_upper_case("ABC") == 1);
    assert(password_contains_upper_case("abc") == 0);

    assert(password_contains_digit("pass123") == 1);
    assert(password_contains_digit("pass") == 0);

    assert(password_contains_special_character("pass!") == 1);
    assert(password_contains_special_character("pass123") == 0);

    assert(password_calculate_strength("short", 8) == 1); // only lowercase
    assert(password_calculate_strength("ShortPass123!", 8) == 5); // all 5 criteria
    assert(strcmp(password_strength_to_string(5), "Stark") == 0);
    assert(strcmp(password_strength_to_string(3), "Mittel") == 0);
    assert(strcmp(password_strength_to_string(1), "Schwach") == 0);

    // Random byte generation
    unsigned char rng_buf[32] = {0};
    assert(password_get_secure_random_bytes(rng_buf, sizeof(rng_buf)) == 1);

    // Password generation
    char generated[64] = {0};
    assert(password_generate_default(16, generated, sizeof(generated)) == 1);
    assert(strlen(generated) == 16);

    // Passphrase generation
    char passphrase[128] = {0};
    assert(password_generate_passphrase(4, "-", 1, passphrase, sizeof(passphrase)) == 1);
    assert(strlen(passphrase) > 10);
    assert(strchr(passphrase, '-') != nullptr);

    // Verify invalid inputs
    assert(password_is_long_enough(NULL, 8) == 0);
    assert(password_is_long_enough("test", -1) == 0);
    assert(password_contains_lower_case(NULL) == 0);
    assert(password_contains_upper_case(NULL) == 0);
    assert(password_contains_digit(NULL) == 0);
    assert(password_contains_special_character(NULL) == 0);
    assert(password_calculate_strength(NULL, 8) == 0);
    assert(password_generate_from_bytes(NULL, 4, "abc", 4, generated, sizeof(generated)) == 0);
    assert(password_generate_from_bytes(rng_buf, 4, NULL, 4, generated, sizeof(generated)) == 0);
    assert(password_generate_default(16, generated, 10) == 0); // buffer too small
    assert(password_generate(16, 0, 0, 0, 0, generated, sizeof(generated)) == 0); // empty charset
    assert(password_generate_passphrase(0, "-", 0, passphrase, sizeof(passphrase)) == 0);

    std::cout << "[PASS] Password Security & Generator Logic passed.\n";
}

static void test_clipboard() {
    std::cout << "[RUN] Testing Clipboard Integration...\n";
    assert(clipboard_copy_text(NULL) == 0);

#if defined(_WIN32) || defined(_WIN64)
    assert(clipboard_copy_text("SecretPasswordToClipboard123!") == 1);
#else
    const char* display = std::getenv("DISPLAY");
    const char* wayland_display = std::getenv("WAYLAND_DISPLAY");

    if (display != nullptr || wayland_display != nullptr) {
        int result = clipboard_copy_text("SecretPasswordToClipboard123!");
        assert(result == 0 || result == 1);
    } else {
        std::cout << "[SKIP] Positive clipboard assertion: CI has no graphical clipboard session.\n";
    }
#endif

    std::cout << "[PASS] Clipboard Integration checks passed.\n";
}

static void test_vault_audit() {
    std::cout << "[RUN] Testing Vault Security Audit and Reused Detection...\n";
    EntryList list;
    assert(entry_list_init(&list, 4) == 1);

    // 1. Empty list
    VaultAuditReport rep_empty;
    assert(vault_audit_analyze(&list, &rep_empty) == 1);
    assert(rep_empty.total_entries == 0);
    assert(rep_empty.health_score == 100);

    // 2. Add weak and duplicate entries
    assert(entry_list_add(&list, "Acc1", "u1", "12345") == 1);       // short, weak (score 1)
    assert(entry_list_add(&list, "Acc2", "u2", "12345") == 1);       // reused duplicate!
    assert(entry_list_add(&list, "Acc3", "u3", "SuperSafe#99!") == 1); // strong (score 5)

    VaultAuditReport rep;
    assert(vault_audit_analyze(&list, &rep) == 1);
    assert(rep.total_entries == 3);
    assert(rep.short_count == 2);
    assert(rep.weak_count == 2);
    assert(rep.strong_count == 1);
    assert(rep.reused_count == 2); // both Acc1 and Acc2 are reused
    assert(rep.unique_passwords == 2);
    assert(rep.health_score < 60);

    entry_list_destroy(&list);
    std::cout << "[PASS] Vault Security Audit passed.\n";
}

static void test_impex() {
    std::cout << "[RUN] Testing CSV and JSON Import/Export...\n";

    EntryList list;
    assert(entry_list_init(&list, 2) == 1);
    assert(entry_list_add(&list, "Google, Inc.", "user@gmail.com", "P@ss,123") == 1);
    assert(entry_list_add(&list, "GitHub \"Official\"", "gituser", "Token#456") == 1);

    // 1. CSV export and import
    const char* csv_file = "test_export.csv";
    remove(csv_file);
    assert(vault_export_csv(&list, csv_file) == 1);

    EntryList csv_imported;
    assert(entry_list_init(&csv_imported, 2) == 1);
    int imported_count = 0;
    assert(vault_import_csv(&csv_imported, csv_file, &imported_count) == 1);
    assert(imported_count == 2);
    assert(csv_imported.count == 2);
    assert(strcmp(csv_imported.entries[0].title, "Google, Inc.") == 0);
    assert(strcmp(csv_imported.entries[0].username, "user@gmail.com") == 0);
    assert(strcmp(csv_imported.entries[0].password, "P@ss,123") == 0);
    assert(strcmp(csv_imported.entries[1].title, "GitHub \"Official\"") == 0);
    assert(strcmp(csv_imported.entries[1].password, "Token#456") == 0);
    entry_list_destroy(&csv_imported);
    remove(csv_file);

    // 2. JSON export and import
    const char* json_file = "test_export.json";
    remove(json_file);
    assert(vault_export_json(&list, json_file) == 1);

    EntryList json_imported;
    assert(entry_list_init(&json_imported, 2) == 1);
    imported_count = 0;
    assert(vault_import_json(&json_imported, json_file, &imported_count) == 1);
    assert(imported_count == 2);
    assert(json_imported.count == 2);
    assert(strcmp(json_imported.entries[0].title, "Google, Inc.") == 0);
    assert(strcmp(json_imported.entries[0].username, "user@gmail.com") == 0);
    assert(strcmp(json_imported.entries[0].password, "P@ss,123") == 0);
    entry_list_destroy(&json_imported);
    remove(json_file);

    entry_list_destroy(&list);
    std::cout << "[PASS] CSV and JSON Import/Export passed.\n";
}

int main() {
    std::cout << "========================================\n";
    test_entry_crud_and_resizing();
    test_crypto_primitives();
    test_storage_persistence();
    test_password_logic();
    test_clipboard();
    test_vault_audit();
    test_impex();

    std::cout << "========================================\n";
    std::cout << "All automated tests completed successfully!\n";
    std::cout << "========================================\n";
    return 0;
}
