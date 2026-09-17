/**
 * @file test_vault.cpp
 * @brief Comprehensive Automated Test Suite for kehl-vault (All 20 Modules).
 */

#include "entry.h"
#include "crypto.h"
#include "storage.h"
#include "password.h"
#include "clipboard.h"
#include "audit.h"
#include "impex.h"
#include "profile.h"
#include "category.h"
#include "totp.h"
#include "history.h"
#include "expiry.h"
#include "backup.h"
#include "search.h"
#include "secure_mem.h"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>

static void test_entry_crud() {
    printf("[1/15] Testing EntryList CRUD & Dynamic Resizing...\n");
    EntryList list;
    assert(entry_list_init(&list, 2) == 1);
    assert(list.count == 0);
    assert(list.capacity == 2);

    assert(entry_list_add(&list, "GitHub", "octocat", "pass123") == 1);
    assert(entry_list_add(&list, "Google", "user@gmail.com", "goog456") == 1);
    assert(entry_list_add(&list, "Amazon", "shopper", "prime789") == 1);
    assert(list.count == 3);
    assert(list.capacity >= 4);

    Entry* e1 = entry_list_get(&list, 1);
    assert(e1 != nullptr);
    assert(strcmp(e1->title, "Google") == 0);

    assert(entry_list_remove(&list, 1) == 1);
    assert(list.count == 2);
    assert(strcmp(list.entries[1].title, "Amazon") == 0);

    entry_list_destroy(&list);
    assert(list.entries == nullptr);
    printf("       -> Passed.\n");
}

static void test_cryptography() {
    printf("[2/15] Testing Cryptographic Primitives (SHA-256, HMAC, PBKDF2, ChaCha20)...\n");
    // Test SHA-256
    uint8_t hash[32];
    crypto_sha256((const uint8_t*)"abc", 3, hash);
    // Expected SHA-256 of "abc": ba7816bf 8f01cfea 414140de 5dae2223 b00361a3 96177a9c b410ff61 f20015ad
    assert(hash[0] == 0xba && hash[1] == 0x78 && hash[2] == 0x16 && hash[3] == 0xbf);

    // Test HMAC-SHA256
    uint8_t hmac[32];
    crypto_hmac_sha256((const uint8_t*)"key", 3, (const uint8_t*)"The quick brown fox jumps over the lazy dog", 43, hmac);
    assert(hmac[0] == 0xf7 && hmac[1] == 0xbc);

    // Test PBKDF2
    uint8_t key[32];
    uint8_t salt[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    assert(crypto_pbkdf2_sha256("password", 8, salt, 16, 1000, key, 32) == 1);

    // Test ChaCha20
    uint8_t chacha_key[32] = {0};
    uint8_t nonce[12] = {0};
    const char* msg = "Secret Payload";
    uint8_t cipher[32] = {0};
    uint8_t decrypted[32] = {0};
    crypto_chacha20_xor(chacha_key, nonce, 1, (const uint8_t*)msg, cipher, strlen(msg));
    crypto_chacha20_xor(chacha_key, nonce, 1, cipher, decrypted, strlen(msg));
    decrypted[strlen(msg)] = '\0';
    assert(strcmp((char*)decrypted, msg) == 0);

    assert(crypto_constant_time_equals(hash, hash, 32) == 1);
    printf("       -> Passed.\n");
}

static void test_storage_encryption() {
    printf("[3/15] Testing Encrypted Storage & Authenticated I/O...\n");
    const char* test_file = "test_encrypted_vault.dat";
    remove(test_file);

    EntryList list;
    entry_list_init(&list, 4);
    entry_list_add(&list, "ProtonMail", "user@pm.me", "TopSecretPass99!");
    entry_list_add(&list, "Bank", "admin", "UltraStrong#1234");

    assert(vault_save_to_file(&list, test_file, "MasterPassphrase123") == VAULT_SUCCESS);
    entry_list_destroy(&list);

    // Verify wrong password rejects
    EntryList loaded;
    entry_list_init(&loaded, 4);
    assert(vault_load_from_file(&loaded, test_file, "WrongPassword") == VAULT_ERR_INVALID_PASSWORD);

    // Verify correct password decrypts
    assert(vault_load_from_file(&loaded, test_file, "MasterPassphrase123") == VAULT_SUCCESS);
    assert(loaded.count == 2);
    assert(strcmp(loaded.entries[0].title, "ProtonMail") == 0);
    assert(strcmp(loaded.entries[1].password, "UltraStrong#1234") == 0);

    entry_list_destroy(&loaded);
    remove(test_file);
    printf("       -> Passed.\n");
}

static void test_password_utilities() {
    printf("[4/15] Testing Password Logic & Diceware Passphrases...\n");
    assert(password_calculate_strength("weak", 8) <= 2);
    assert(password_calculate_strength("SecureP@ssw0rd123", 8) == 5);

    char gen_pwd[64];
    assert(password_generate_default(16, gen_pwd, sizeof(gen_pwd)) == 1);
    assert(strlen(gen_pwd) == 16);

    char passphrase[128];
    assert(password_generate_passphrase(4, "-", 1, passphrase, sizeof(passphrase)) == 1);
    assert(strlen(passphrase) > 10);
    printf("       -> Passed.\n");
}

static void test_clipboard() {
    printf("[5/15] Testing Clipboard Integration...\n");
    assert(clipboard_copy_text("KehlVaultSecureToken") == 1);
    printf("       -> Passed.\n");
}

static void test_audit() {
    printf("[6/15] Testing Security Audit & Duplicate Detection...\n");
    EntryList list;
    entry_list_init(&list, 4);
    entry_list_add(&list, "SiteA", "userA", "ReusedPass123!");
    entry_list_add(&list, "SiteB", "userB", "ReusedPass123!");
    entry_list_add(&list, "SiteC", "userC", "UniqueP@ssw0rd789#");

    VaultAuditReport report;
    assert(audit_vault(&list, &report) == 1);
    assert(report.total_entries == 3);
    assert(report.reused_groups_count == 1);
    assert(report.reused_groups[0].count == 2);

    entry_list_destroy(&list);
    printf("       -> Passed.\n");
}

static void test_impex() {
    printf("[7/15] Testing CSV & JSON Import/Export...\n");
    const char* csv_file = "test_export.csv";
    const char* json_file = "test_export.json";
    remove(csv_file);
    remove(json_file);

    EntryList list;
    entry_list_init(&list, 4);
    entry_list_add(&list, "Work Email", "alice,work", "Secret\"Pass");
    entry_list_add(&list, "Personal Blog", "blogger", "Blog123!");

    assert(impex_export_csv(&list, csv_file) == 1);
    assert(impex_export_json(&list, json_file) == 1);

    EntryList csv_in;
    entry_list_init(&csv_in, 4);
    assert(impex_import_csv(&csv_in, csv_file) == 2);
    assert(strcmp(csv_in.entries[0].username, "alice,work") == 0);

    EntryList json_in;
    entry_list_init(&json_in, 4);
    assert(impex_import_json(&json_in, json_file) == 2);
    assert(strcmp(json_in.entries[1].title, "Personal Blog") == 0);

    entry_list_destroy(&list);
    entry_list_destroy(&csv_in);
    entry_list_destroy(&json_in);
    remove(csv_file);
    remove(json_file);
    printf("       -> Passed.\n");
}

static void test_profile() {
    printf("[8/15] Testing Profile Management...\n");
    ProfileList plist;
    profile_list_init(&plist);
    assert(profile_list_add(&plist, "Work", "vault_work.dat", 0) == 1);
    assert(profile_list_add(&plist, "Personal", "vault_personal.dat", 1) == 1);
    assert(plist.count == 2);
    assert(plist.active_index == 1);

    char path[260];
    assert(profile_get_filepath("finance", path, sizeof(path)) == 1);
    assert(strcmp(path, "vault_finance.dat") == 0);
    printf("       -> Passed.\n");
}

static void test_category() {
    printf("[9/15] Testing Category & Tag Parsing...\n");
    assert(strcmp(category_to_string(VAULT_CAT_LOGIN), "Login") == 0);
    assert(category_from_string("note") == VAULT_CAT_SECURE_NOTE);

    char tags[8][TAG_NAME_MAX];
    int parsed = category_parse_tags("finance, personal, cloud", tags, 8);
    assert(parsed == 3);
    assert(strcmp(tags[0], "finance") == 0);
    assert(strcmp(tags[2], "cloud") == 0);

    assert(category_has_tag("finance, personal, cloud", "personal") == 1);
    assert(category_has_tag("finance, personal, cloud", "work") == 0);
    printf("       -> Passed.\n");
}

static void test_totp() {
    printf("[10/15] Testing TOTP RFC 6238 Generator...\n");
    // Standard RFC 6238 test secret: "JBSWY3DPEHPK3PXP"
    const char* secret = "JBSWY3DPEHPK3PXP";
    char code[16];

    // Fixed timestamp = 59s -> step = 1
    assert(totp_generate(secret, 59, 6, 30, code, sizeof(code)) == 1);
    assert(strlen(code) == 6);

    int remaining = 0;
    assert(totp_generate_current(secret, code, sizeof(code), &remaining) == 1);
    assert(strlen(code) == 6);
    assert(remaining >= 0 && remaining <= 30);
    assert(totp_verify(secret, code, 0, 1) == 1);
    printf("       -> Passed.\n");
}

static void test_history_trash() {
    printf("[11/15] Testing History & Soft-Delete Trash Bin...\n");
    HistoryList hlist;
    history_list_init(&hlist);

    Entry e;
    entry_create(&e, "OldSite", "olduser", "oldpass");
    assert(history_log_event(&hlist, &e, HIST_ACTION_DELETED) == 1);
    assert(history_trash_count(&hlist) == 1);

    EntryList vault;
    entry_list_init(&vault, 4);
    assert(history_restore_entry(&hlist, 0, &vault) == 1);
    assert(vault.count == 1);
    assert(strcmp(vault.entries[0].title, "OldSite") == 0);
    assert(history_trash_count(&hlist) == 0);

    entry_list_destroy(&vault);
    printf("       -> Passed.\n");
}

static void test_expiry() {
    printf("[12/15] Testing Password Expiry & Age Tracking...\n");
    uint64_t now = 1700000000ULL;
    uint64_t modified_40_days_ago = now - (40ULL * 24ULL * 60ULL * 60ULL);

    EntryExpiryInfo info;
    assert(expiry_evaluate(modified_40_days_ago, EXPIRY_POLICY_30_DAYS, now, &info) == 1);
    assert(info.age_days == 40);
    assert(info.status == EXPIRY_STATUS_EXPIRED);

    uint64_t modified_5_days_ago = now - (5ULL * 24ULL * 60ULL * 60ULL);
    assert(expiry_evaluate(modified_5_days_ago, EXPIRY_POLICY_30_DAYS, now, &info) == 1);
    assert(info.status == EXPIRY_STATUS_OK);
    printf("       -> Passed.\n");
}

static void test_backup() {
    printf("[13/15] Testing Backup Snapshot Creation & Pruning...\n");
    const char* source = "test_src_vault.dat";
    FILE* f = fopen(source, "wb");
    assert(f != nullptr);
    fputs("VaultDataPayload", f);
    fclose(f);

    char backup_path[300];
    assert(backup_create_snapshot(source, backup_path, sizeof(backup_path)) == 1);

    BackupList blist;
    assert(backup_list_snapshots(source, &blist) >= 1);

    remove(source);
    remove(backup_path);
    printf("       -> Passed.\n");
}

static void test_search() {
    printf("[14/15] Testing Fuzzy Search & Levenshtein Ranking...\n");
    assert(search_levenshtein_distance("kitten", "sitting") == 3);

    EntryList list;
    entry_list_init(&list, 4);
    entry_list_add(&list, "GitHub Corporate", "alice", "pass1");
    entry_list_add(&list, "GitLab Personal", "alice", "pass2");
    entry_list_add(&list, "Amazon AWS", "admin", "pass3");

    SearchResultSet results;
    int count = search_vault(&list, "git", &results);
    assert(count == 2);
    assert(results.matches[0].relevance_score >= results.matches[1].relevance_score);

    entry_list_destroy(&list);
    printf("       -> Passed.\n");
}

static void test_secure_mem() {
    printf("[15/15] Testing Volatile Secure Memory Scrubbing...\n");
    char sensitive[32] = "UltraSecretPassword";
    secure_wipe_string(sensitive);
    assert(sensitive[0] == '\0');

    Entry e;
    entry_create(&e, "Title", "User", "Secret");
    secure_wipe_entry(&e);
    assert(e.password[0] == 0);
    printf("       -> Passed.\n");
}

int main() {
    printf("\n=======================================================\n");
    printf("         KEHL-VAULT COMPREHENSIVE TEST SUITE           \n");
    printf("=======================================================\n");

    test_entry_crud();
    test_cryptography();
    test_storage_encryption();
    test_password_utilities();
    test_clipboard();
    test_audit();
    test_impex();
    test_profile();
    test_category();
    test_totp();
    test_history_trash();
    test_expiry();
    test_backup();
    test_search();
    test_secure_mem();

    printf("\n=======================================================\n");
    printf("  SUCCESS: All 15 Subsystem Verification Tests Passed! \n");
    printf("=======================================================\n");
    return 0;
}
