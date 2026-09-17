/**
 * @file main.cpp
 * @brief Interactive CLI Presentation Layer for kehl-vault password manager.
 *
 * Integrates credential management, encrypted persistence, password generation,
 * Diceware passphrases, TOTP 2FA, multi-vault profiles, fuzzy search, backup snapshots,
 * and security audits with masked console input and secure clipboard copying.
 */

#include "entry.h"
#include "password.h"
#include "storage.h"
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

#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>

#if defined(_WIN32) || defined(_WIN64)
#include <conio.h>
#endif

static std::string g_vault_filepath = "vault.dat";
static std::string g_active_profile_name = "default";
static bool g_show_passwords = false;
static std::string g_master_password = "";
static HistoryList g_history;

static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

static std::string to_lower(const std::string& str) {
    std::string lower_str = str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), [](unsigned char c) {
        return (char)std::tolower(c);
    });
    return lower_str;
}

static std::string read_masked_input(const std::string& prompt) {
    std::cout << prompt;
#if defined(_WIN32) || defined(_WIN64)
    std::string result;
    while (true) {
        int ch = _getch();
        if (ch == '\r' || ch == '\n') {
            std::cout << "\n";
            break;
        } else if (ch == '\b' || ch == 127) {
            if (!result.empty()) {
                result.pop_back();
                std::cout << "\b \b";
            }
        } else if (ch == 3) { // Ctrl+C
            std::cout << "\n";
            return "";
        } else if (ch >= 32 && ch <= 126) {
            result.push_back((char)ch);
            std::cout << "*";
        }
    }
    return result;
#else
    std::string result;
    std::getline(std::cin, result);
    return result;
#endif
}

static void print_entry_details(int index, const Entry* entry) {
    int strength = password_calculate_strength(entry->password, 8);
    const char* strength_label = password_strength_to_string(strength);

    std::cout << "[" << index << "] Title:       " << entry->title << "\n";
    std::cout << "    Username:    " << entry->username << "\n";

    if (g_show_passwords) {
        std::cout << "    Password:    " << entry->password << "\n";
    } else {
        std::cout << "    Password:    •••••••••••• (Press [V] to reveal or [6] to copy)\n";
    }
    std::cout << "    Strength:    " << strength_label << " (" << strength << "/5)\n";
    std::cout << "------------------------------------------------------------\n";
}

static void list_all_entries(const EntryList* list) {
    std::cout << "\n=== Stored Entries (" << list->count << ") [Profile: " << g_active_profile_name << "] ===\n";
    if (list->count == 0) {
        std::cout << "Vault is empty.\n";
        return;
    }

    for (int i = 0; i < list->count; ++i) {
        Entry* entry = entry_list_get(const_cast<EntryList*>(list), i);
        if (entry != nullptr) {
            print_entry_details(i, entry);
        }
    }
}

static void search_entries(const EntryList* list) {
    std::cout << "\nSearch query (fuzzy title/username): ";
    std::string query;
    if (!std::getline(std::cin, query)) return;
    query = trim(query);
    if (query.empty()) {
        std::cout << "Search cancelled (empty query).\n";
        return;
    }

    SearchResultSet results;
    int matches = search_vault(list, query.c_str(), &results);

    std::cout << "\n=== Search Results (" << matches << " matches) ===\n";
    if (matches == 0) {
        std::cout << "No matching entries found.\n";
        return;
    }

    for (int i = 0; i < results.count; ++i) {
        int idx = results.matches[i].entry_index;
        Entry* entry = entry_list_get(const_cast<EntryList*>(list), idx);
        if (entry != nullptr) {
            std::cout << "Relevance Score: " << results.matches[i].relevance_score << " | ";
            print_entry_details(idx, entry);
        }
    }
}

static void generate_totp_code() {
    std::cout << "\nEnter Base32 TOTP Secret Key: ";
    std::string secret;
    if (!std::getline(std::cin, secret)) return;
    secret = trim(secret);
    if (secret.empty()) {
        std::cout << "Cancelled.\n";
        return;
    }

    char code[16];
    int remaining = 0;
    if (totp_generate_current(secret.c_str(), code, sizeof(code), &remaining)) {
        std::cout << "\n>>> TOTP 2FA Code: " << code << " (Valid for " << remaining << "s) <<<\n";
        std::cout << "Copy code to clipboard? (y/n): ";
        std::string choice;
        std::getline(std::cin, choice);
        if (choice == "y" || choice == "Y") {
            if (clipboard_copy_text(code)) {
                std::cout << "Code copied to clipboard!\n";
            }
        }
    } else {
        std::cout << "Error: Invalid Base32 secret string.\n";
    }
}

static void copy_password_to_clipboard(const EntryList* list) {
    if (list->count == 0) {
        std::cout << "\nVault is empty.\n";
        return;
    }

    std::cout << "\nEnter Entry Index to copy password: ";
    std::string input;
    if (!std::getline(std::cin, input)) return;
    input = trim(input);

    try {
        int index = std::stoi(input);
        Entry* entry = entry_list_get(const_cast<EntryList*>(list), index);
        if (entry == nullptr) {
            std::cout << "Error: Invalid index.\n";
            return;
        }

        if (clipboard_copy_text(entry->password)) {
            std::cout << "Success: Password for '" << entry->title << "' copied to clipboard!\n";
        } else {
            std::cout << "Warning: Could not access system clipboard.\n";
        }
    } catch (...) {
        std::cout << "Error: Invalid input.\n";
    }
}

static void add_entry_interactive(EntryList* list) {
    std::cout << "\n=== Add New Entry ===\n";

    std::cout << "Title: ";
    std::string title;
    if (!std::getline(std::cin, title)) return;
    title = trim(title);
    if (title.empty()) {
        std::cout << "Error: Title cannot be empty.\n";
        return;
    }

    std::cout << "Username: ";
    std::string username;
    if (!std::getline(std::cin, username)) return;
    username = trim(username);
    if (username.empty()) {
        std::cout << "Error: Username cannot be empty.\n";
        return;
    }

    std::cout << "Password generation options:\n";
    std::cout << "  [1] Enter password manually\n";
    std::cout << "  [2] Generate strong random password\n";
    std::cout << "  [3] Generate Diceware passphrase (multi-word)\n";
    std::cout << "Choice: ";

    std::string gen_choice;
    std::getline(std::cin, gen_choice);
    gen_choice = trim(gen_choice);

    std::string password;

    if (gen_choice == "2") {
        char gen_buf[ENTRY_PASSWORD_SIZE];
        if (password_generate_default(16, gen_buf, sizeof(gen_buf))) {
            password = gen_buf;
            std::cout << "Generated password: " << password << "\n";
        } else {
            std::cout << "Generator failed. Please enter password manually.\n";
        }
    } else if (gen_choice == "3") {
        char pass_buf[ENTRY_PASSWORD_SIZE];
        if (password_generate_passphrase(4, "-", 1, pass_buf, sizeof(pass_buf))) {
            password = pass_buf;
            std::cout << "Generated passphrase: " << password << "\n";
        } else {
            std::cout << "Passphrase generator failed.\n";
        }
    }

    if (password.empty()) {
        password = read_masked_input("Enter password: ");
        password = trim(password);
        if (password.empty()) {
            std::cout << "Error: Password cannot be empty.\n";
            return;
        }
    }

    if (entry_list_add(list, title.c_str(), username.c_str(), password.c_str())) {
        std::cout << "Entry '" << title << "' added successfully!\n";
    } else {
        std::cout << "Error adding entry.\n";
    }
}

static void edit_entry_interactive(EntryList* list) {
    if (list->count == 0) {
        std::cout << "\nVault is empty.\n";
        return;
    }

    std::cout << "\nEnter Entry Index to edit: ";
    std::string input;
    if (!std::getline(std::cin, input)) return;
    input = trim(input);

    try {
        int index = std::stoi(input);
        Entry* entry = entry_list_get(list, index);
        if (entry == nullptr) {
            std::cout << "Error: Invalid index.\n";
            return;
        }

        /* Log modification event to history */
        history_log_event(&g_history, entry, HIST_ACTION_MODIFIED);

        std::cout << "New Title [" << entry->title << "]: ";
        std::string title;
        std::getline(std::cin, title);
        title = trim(title);
        if (title.empty()) title = entry->title;

        std::cout << "New Username [" << entry->username << "]: ";
        std::string username;
        std::getline(std::cin, username);
        username = trim(username);
        if (username.empty()) username = entry->username;

        std::string password = read_masked_input("New Password (leave empty to keep current): ");
        password = trim(password);
        if (password.empty()) password = entry->password;

        if (entry_create(entry, title.c_str(), username.c_str(), password.c_str())) {
            std::cout << "Entry updated successfully!\n";
        } else {
            std::cout << "Error updating entry.\n";
        }
    } catch (...) {
        std::cout << "Error: Invalid input.\n";
    }
}

static void delete_entry_interactive(EntryList* list) {
    if (list->count == 0) {
        std::cout << "\nVault is empty.\n";
        return;
    }

    std::cout << "\nEnter Entry Index to delete: ";
    std::string input;
    if (!std::getline(std::cin, input)) return;
    input = trim(input);

    try {
        int index = std::stoi(input);
        Entry* entry = entry_list_get(list, index);
        if (entry == nullptr) {
            std::cout << "Error: Invalid index.\n";
            return;
        }

        /* Record in history / trash bin prior to deletion */
        history_log_event(&g_history, entry, HIST_ACTION_DELETED);

        std::cout << "Are you sure you want to delete '" << entry->title << "'? (y/n): ";
        std::string confirm;
        std::getline(std::cin, confirm);
        if (confirm == "y" || confirm == "Y") {
            if (entry_list_remove(list, index)) {
                std::cout << "Entry deleted (moved to trash bin, restorable in session).\n";
            } else {
                std::cout << "Error deleting entry.\n";
            }
        }
    } catch (...) {
        std::cout << "Error: Invalid input.\n";
    }
}

static void manage_trash_bin(EntryList* list) {
    int trash_count = history_trash_count(&g_history);
    std::cout << "\n=== Trash Bin (" << trash_count << " items) ===\n";
    if (trash_count == 0) {
        std::cout << "Trash bin is empty.\n";
        return;
    }

    for (int i = 0; i < g_history.count; ++i) {
        if (g_history.records[i].action == HIST_ACTION_DELETED && !g_history.records[i].is_purged) {
            std::cout << "[" << i << "] " << g_history.records[i].entry.title
                      << " (" << g_history.records[i].entry.username << ")\n";
        }
    }

    std::cout << "\nOptions: [R] Restore item | [E] Empty trash | [B] Back: ";
    std::string choice;
    std::getline(std::cin, choice);
    choice = trim(choice);

    if (choice == "r" || choice == "R") {
        std::cout << "Enter history index to restore: ";
        std::string idx_str;
        std::getline(std::cin, idx_str);
        try {
            int h_idx = std::stoi(idx_str);
            if (history_restore_entry(&g_history, h_idx, list)) {
                std::cout << "Entry restored successfully!\n";
            } else {
                std::cout << "Restore failed.\n";
            }
        } catch (...) {
            std::cout << "Invalid index.\n";
        }
    } else if (choice == "e" || choice == "E") {
        int purged = history_empty_trash(&g_history);
        std::cout << "Purged " << purged << " entries from trash.\n";
    }
}

static void switch_profile_interactive(EntryList* list) {
    ProfileList prof_list;
    profile_discover(&prof_list);

    std::cout << "\n=== Vault Profiles ===\n";
    for (int i = 0; i < prof_list.count; ++i) {
        std::cout << "[" << i << "] " << prof_list.profiles[i].name
                  << " (" << prof_list.profiles[i].filepath << ")"
                  << (prof_list.profiles[i].name == g_active_profile_name ? " [ACTIVE]" : "") << "\n";
    }

    std::cout << "\nEnter profile index to switch, or enter new profile name: ";
    std::string input;
    std::getline(std::cin, input);
    input = trim(input);
    if (input.empty()) return;

    char new_path[PROFILE_PATH_MAX];
    try {
        int idx = std::stoi(input);
        if (idx >= 0 && idx < prof_list.count) {
            g_active_profile_name = prof_list.profiles[idx].name;
            g_vault_filepath = prof_list.profiles[idx].filepath;
        }
    } catch (...) {
        /* User entered a named profile */
        g_active_profile_name = input;
        profile_get_filepath(input.c_str(), new_path, sizeof(new_path));
        g_vault_filepath = new_path;
    }

    std::cout << "Active profile switched to '" << g_active_profile_name << "' (" << g_vault_filepath << ")\n";

    /* Reload database */
    entry_list_destroy(list);
    entry_list_init(list, 16);

    int is_enc = 0;
    if (vault_is_encrypted_file(g_vault_filepath.c_str(), &is_enc) && is_enc) {
        std::string pwd = read_masked_input("Enter master password for this profile: ");
        int status = vault_load_from_file(list, g_vault_filepath.c_str(), pwd.c_str());
        if (status == VAULT_SUCCESS) {
            g_master_password = pwd;
            std::cout << "Loaded " << list->count << " entries successfully.\n";
        } else {
            std::cout << "Failed to unlock profile vault.\n";
        }
    } else {
        vault_load_from_file(list, g_vault_filepath.c_str(), "");
    }
}

static void print_menu() {
    std::cout << "\n========================================\n";
    std::cout << "   KEHL-VAULT - SECURE PASSWORD MANAGER \n";
    std::cout << "   [Profile: " << g_active_profile_name << " | Passwords: " << (g_show_passwords ? "Visible" : "Masked") << "]\n";
    std::cout << "========================================\n";
    std::cout << " [1] List all entries\n";
    std::cout << " [2] Search entries (Fuzzy Search)\n";
    std::cout << " [3] Add new entry\n";
    std::cout << " [4] Edit existing entry\n";
    std::cout << " [5] Delete entry\n";
    std::cout << " [6] Copy password to clipboard\n";
    std::cout << " [7] Security Audit & Health Score\n";
    std::cout << " [8] Export entries (CSV / JSON)\n";
    std::cout << " [9] Import entries (CSV / JSON)\n";
    std::cout << " [T] TOTP 2FA Token Generator\n";
    std::cout << " [P] Switch / Manage Profiles\n";
    std::cout << " [B] Manage Trash Bin\n";
    std::cout << " [V] Toggle Password Visibility\n";
    std::cout << " [S] Save & Exit\n";
    std::cout << " [0] Exit without saving\n";
    std::cout << "----------------------------------------\n";
    std::cout << "Select an option: ";
}

int main() {
    EntryList vault;
    if (!entry_list_init(&vault, 16)) {
        std::cerr << "Fatal: Failed to allocate memory for vault.\n";
        return 1;
    }

    history_list_init(&g_history);

    std::cout << "========================================\n";
    std::cout << "      WELCOME TO KEHL-VAULT             \n";
    std::cout << "========================================\n";

    int is_encrypted = 0;
    if (vault_is_encrypted_file(g_vault_filepath.c_str(), &is_encrypted) && is_encrypted) {
        while (true) {
            std::string pwd = read_masked_input("Enter Master Password: ");
            if (pwd.empty()) {
                std::cout << "Cancelled. Exiting.\n";
                entry_list_destroy(&vault);
                return 0;
            }

            int load_res = vault_load_from_file(&vault, g_vault_filepath.c_str(), pwd.c_str());
            if (load_res == VAULT_SUCCESS) {
                g_master_password = pwd;
                std::cout << "Vault unlocked successfully! (" << vault.count << " entries loaded)\n";
                break;
            } else if (load_res == VAULT_ERR_INVALID_PASSWORD) {
                std::cout << "Incorrect master password. Try again.\n";
            } else {
                std::cout << "Error loading vault file.\n";
                break;
            }
        }
    } else {
        int load_res = vault_load_from_file(&vault, g_vault_filepath.c_str(), "");
        if (load_res == VAULT_SUCCESS) {
            std::cout << "Loaded " << vault.count << " entries from vault.\n";
        } else {
            std::cout << "No existing vault found. Creating a new one.\n";
            std::string new_pwd = read_masked_input("Set a Master Password for encryption: ");
            g_master_password = trim(new_pwd);
        }
    }

    bool running = true;
    while (running) {
        print_menu();
        std::string choice;
        if (!std::getline(std::cin, choice)) break;
        choice = trim(choice);

        if (choice == "1") {
            list_all_entries(&vault);
        } else if (choice == "2") {
            search_entries(&vault);
        } else if (choice == "3") {
            add_entry_interactive(&vault);
        } else if (choice == "4") {
            edit_entry_interactive(&vault);
        } else if (choice == "5") {
            delete_entry_interactive(&vault);
        } else if (choice == "6") {
            copy_password_to_clipboard(&vault);
        } else if (choice == "7") {
            VaultAuditReport report;
            if (audit_vault(&vault, &report)) {
                audit_print_report(&report);
            }
        } else if (choice == "8") {
            std::cout << "Export format: [1] CSV | [2] JSON: ";
            std::string fmt;
            std::getline(std::cin, fmt);
            if (fmt == "1") {
                if (impex_export_csv(&vault, "vault_export.csv")) {
                    std::cout << "Exported to vault_export.csv\n";
                }
            } else if (fmt == "2") {
                if (impex_export_json(&vault, "vault_export.json")) {
                    std::cout << "Exported to vault_export.json\n";
                }
            }
        } else if (choice == "9") {
            std::cout << "Import format: [1] CSV | [2] JSON: ";
            std::string fmt;
            std::getline(std::cin, fmt);
            if (fmt == "1") {
                int c = impex_import_csv(&vault, "vault_export.csv");
                std::cout << "Imported " << c << " entries.\n";
            } else if (fmt == "2") {
                int c = impex_import_json(&vault, "vault_export.json");
                std::cout << "Imported " << c << " entries.\n";
            }
        } else if (choice == "t" || choice == "T") {
            generate_totp_code();
        } else if (choice == "p" || choice == "P") {
            switch_profile_interactive(&vault);
        } else if (choice == "b" || choice == "B") {
            manage_trash_bin(&vault);
        } else if (choice == "v" || choice == "V") {
            g_show_passwords = !g_show_passwords;
            std::cout << "Password visibility set to: " << (g_show_passwords ? "VISIBLE" : "MASKED") << "\n";
        } else if (choice == "s" || choice == "S") {
            /* Create backup snapshot prior to saving */
            backup_create_snapshot(g_vault_filepath.c_str(), nullptr, 0);

            if (vault_save_to_file(&vault, g_vault_filepath.c_str(), g_master_password.c_str()) == VAULT_SUCCESS) {
                std::cout << "Vault saved successfully to " << g_vault_filepath << ".\n";
            } else {
                std::cerr << "Error saving vault to disk!\n";
            }
            running = false;
        } else if (choice == "0") {
            std::cout << "Exiting without saving.\n";
            running = false;
        } else {
            std::cout << "Invalid selection. Please try again.\n";
        }
    }

    /* Securely wipe memory before terminating */
    secure_wipe_list(&vault);
    entry_list_destroy(&vault);
    secure_memzero(&g_master_password[0], g_master_password.size());

    std::cout << "Kehl-Vault session terminated safely.\n";
    return 0;
}
