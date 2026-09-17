#include "entry.h"
#include "password.h"
#include "storage.h"
#include "clipboard.h"
#include "audit.h"
#include "impex.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>

#if defined(_WIN32) || defined(_WIN64)
#include <conio.h>
#endif

static const char* VAULT_FILENAME = "vault.dat";
static bool g_show_passwords = false;
static std::string g_master_password = "";

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

    std::cout << "[" << index << "] Titel:      " << entry->title << "\n";
    std::cout << "    Benutzer:   " << entry->username << "\n";

    if (g_show_passwords) {
        std::cout << "    Passwort:   " << entry->password << "\n";
    } else {
        std::cout << "    Passwort:   •••••��•••••• (Druecke [V] zum Aufdecken oder [6] zum Kopieren)\n";
    }
    std::cout << "    Staerke:    " << strength_label << " (" << strength << "/5)\n";
    std::cout << "------------------------------------------------------------\n";
}

static void list_all_entries(const EntryList* list) {
    std::cout << "\n=== Gespeicherte Eintraege (" << list->count << ") ===\n";
    if (list->count == 0) {
        std::cout << "Keine Eintraege vorhanden.\n";
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
    std::cout << "\nSuchbegriff (Titel oder Benutzername): ";
    std::string query;
    if (!std::getline(std::cin, query)) return;
    query = trim(query);
    if (query.empty()) {
        std::cout << "Suche abgebrochen (leere Eingabe).\n";
        return;
    }

    std::string query_lower = to_lower(query);
    int match_count = 0;

    std::cout << "\n=== Suchergebnisse fuer '" << query << "' ===\n";
    for (int i = 0; i < list->count; ++i) {
        Entry* entry = entry_list_get(const_cast<EntryList*>(list), i);
        if (entry != nullptr) {
            std::string title_lower = to_lower(entry->title);
            std::string user_lower = to_lower(entry->username);

            if (title_lower.find(query_lower) != std::string::npos ||
                user_lower.find(query_lower) != std::string::npos) {
                print_entry_details(i, entry);
                match_count++;
            }
        }
    }

    if (match_count == 0) {
        std::cout << "Keine passenden Eintraege gefunden.\n";
    } else {
        std::cout << match_count << " Treffer gefunden.\n";
    }
}

static std::string prompt_password_creation() {
    std::cout << "Passwortmethode:\n";
    std::cout << "  [1] Manuell eingeben (maskiert)\n";
    std::cout << "  [2] Zufallspasswort generieren (Standard: 16 Zeichen)\n";
    std::cout << "  [3] Diceware-Passphrase generieren (z. B. correct-horse-battery-staple)\n";
    std::cout << "Auswahl [1-3, Standard: 2]: ";

    std::string method_str;
    std::getline(std::cin, method_str);
    method_str = trim(method_str);

    if (method_str == "1") {
        std::string pass = read_masked_input("Passwort eingeben: ");
        return trim(pass);
    } else if (method_str == "3") {
        std::cout << "Anzahl Woerter (Standard: 4): ";
        std::string words_str;
        std::getline(std::cin, words_str);
        int words = 4;
        if (!trim(words_str).empty()) {
            try { words = std::stoi(words_str); } catch (...) { words = 4; }
        }
        if (words < 2) words = 2;
        if (words > 12) words = 12;

        char passphrase[ENTRY_PASSWORD_SIZE] = {0};
        if (password_generate_passphrase(words, "-", 1, passphrase, sizeof(passphrase))) {
            std::cout << "Generierte Passphrase: " << passphrase << "\n";
            return std::string(passphrase);
        }
    }

    // Default: Standard generator
    std::cout << "Passwortlaenge (Standard: 16): ";
    std::string len_str;
    std::getline(std::cin, len_str);
    int length = 16;
    if (!trim(len_str).empty()) {
        try { length = std::stoi(len_str); } catch (...) { length = 16; }
    }
    if (length < 4) length = 4;
    if (length >= ENTRY_PASSWORD_SIZE) length = ENTRY_PASSWORD_SIZE - 1;

    char gen_buf[ENTRY_PASSWORD_SIZE] = {0};
    if (password_generate_default(length, gen_buf, sizeof(gen_buf))) {
        std::cout << "Generiertes Passwort: " << gen_buf << "\n";
        return std::string(gen_buf);
    }

    return "";
}

static void add_entry(EntryList* list) {
    std::cout << "\n=== Neuen Eintrag anlegen ===\n";

    std::cout << "Titel: ";
    std::string title;
    if (!std::getline(std::cin, title)) return;
    title = trim(title);
    if (title.empty()) {
        std::cout << "Fehler: Titel darf nicht leer sein.\n";
        return;
    }

    std::cout << "Benutzername / E-Mail: ";
    std::string username;
    if (!std::getline(std::cin, username)) return;
    username = trim(username);

    std::string password = prompt_password_creation();
    if (password.empty()) {
        std::cout << "Fehler: Passwort konnte nicht erstellt werden.\n";
        return;
    }

    int success = entry_list_add(list, title.c_str(), username.c_str(), password.c_str());
    if (success) {
        int strength = password_calculate_strength(password.c_str(), 8);
        std::cout << "Eintrag erfolgreich hinzugefuegt! (Passwortstaerke: "
                  << password_strength_to_string(strength) << ")\n";
    } else {
        std::cout << "Fehler: Eintrag konnte nicht hinzugefuegt werden.\n";
    }
}

static void edit_entry(EntryList* list) {
    std::cout << "\n=== Eintrag bearbeiten ===\n";
    if (list->count == 0) {
        std::cout << "Keine Eintraege zum Bearbeiten vorhanden.\n";
        return;
    }

    std::cout << "Index des zu bearbeitenden Eintrags (0 bis " << (list->count - 1) << "): ";
    std::string index_str;
    if (!std::getline(std::cin, index_str)) return;
    index_str = trim(index_str);

    int index = -1;
    try {
        index = std::stoi(index_str);
    } catch (...) {
        std::cout << "Ungueltige Zahl.\n";
        return;
    }

    Entry* entry = entry_list_get(list, index);
    if (entry == nullptr) {
        std::cout << "Fehler: Kein Eintrag mit Index " << index << " gefunden.\n";
        return;
    }

    std::cout << "Aktueller Titel [" << entry->title << "] (Enter zum Beibehalten): ";
    std::string new_title;
    std::getline(std::cin, new_title);
    new_title = trim(new_title);
    if (new_title.empty()) new_title = entry->title;

    std::cout << "Aktueller Benutzer [" << entry->username << "] (Enter zum Beibehalten): ";
    std::string new_user;
    std::getline(std::cin, new_user);
    new_user = trim(new_user);
    if (new_user.empty()) new_user = entry->username;

    std::cout << "Moechtest du das Passwort aendern? (j/N): ";
    std::string change_pass;
    std::getline(std::cin, change_pass);
    change_pass = trim(change_pass);

    std::string new_pass = entry->password;
    if (change_pass == "j" || change_pass == "J" || change_pass == "y" || change_pass == "Y") {
        new_pass = prompt_password_creation();
        if (new_pass.empty()) {
            new_pass = entry->password;
        }
    }

    if (entry_create(entry, new_title.c_str(), new_user.c_str(), new_pass.c_str())) {
        std::cout << "Eintrag [" << index << "] erfolgreich aktualisiert!\n";
    } else {
        std::cout << "Fehler beim Aktualisieren des Eintrags.\n";
    }
}

static void delete_entry(EntryList* list) {
    std::cout << "\n=== Eintrag loeschen ===\n";
    if (list->count == 0) {
        std::cout << "Keine Eintraege zum Loeschen vorhanden.\n";
        return;
    }

    std::cout << "Index des zu loeschenden Eintrags (0 bis " << (list->count - 1) << "): ";
    std::string index_str;
    if (!std::getline(std::cin, index_str)) return;
    index_str = trim(index_str);

    int index = -1;
    try {
        index = std::stoi(index_str);
    } catch (...) {
        std::cout << "Ungueltige Zahl.\n";
        return;
    }

    Entry* entry = entry_list_get(list, index);
    if (entry == nullptr) {
        std::cout << "Fehler: Kein Eintrag mit Index " << index << " gefunden.\n";
        return;
    }

    std::cout << "Bist du sicher, dass du Eintrag '" << entry->title << "' loeschen moechtest? (j/N): ";
    std::string confirm;
    std::getline(std::cin, confirm);
    confirm = trim(confirm);

    if (confirm == "j" || confirm == "J" || confirm == "y" || confirm == "Y") {
        if (entry_list_remove(list, index)) {
            std::cout << "Eintrag [" << index << "] wurde geloescht.\n";
        } else {
            std::cout << "Fehler beim Loeschen des Eintrags.\n";
        }
    } else {
        std::cout << "Loeschvorgang abgebrochen.\n";
    }
}

static void copy_password_to_clipboard(const EntryList* list) {
    std::cout << "\n=== Passwort in Zwischenablage kopieren ===\n";
    if (list->count == 0) {
        std::cout << "Keine Eintraege vorhanden.\n";
        return;
    }

    std::cout << "Index des Eintrags (0 bis " << (list->count - 1) << "): ";
    std::string index_str;
    if (!std::getline(std::cin, index_str)) return;
    index_str = trim(index_str);

    int index = -1;
    try {
        index = std::stoi(index_str);
    } catch (...) {
        std::cout << "Ungueltige Zahl.\n";
        return;
    }

    Entry* entry = entry_list_get(const_cast<EntryList*>(list), index);
    if (entry == nullptr) {
        std::cout << "Fehler: Kein Eintrag mit Index " << index << " gefunden.\n";
        return;
    }

    if (clipboard_copy_text(entry->password)) {
        std::cout << "Passwort fuer '" << entry->title << "' wurde erfolgreich in die Zwischenablage kopiert!\n";
    } else {
        std::cout << "Fehler: Kopieren in die Zwischenablage fehlgeschlagen.\n";
    }
}

static void show_security_audit(const EntryList* list) {
    std::cout << "\n=== Sicherheits-Audit & Vault-Health ===\n";
    VaultAuditReport report;
    if (!vault_audit_analyze(list, &report)) {
        std::cout << "Fehler bei der Analyse.\n";
        return;
    }

    std::cout << "Gesamtzahl Eintraege:    " << report.total_entries << "\n";
    std::cout << "Einzigartige Passwoerter: " << report.unique_passwords << "\n";
    std::cout << "Starke Passwoerter:       " << report.strong_count << "\n";
    std::cout << "Mittlere Passwoerter:     " << report.medium_count << "\n";
    std::cout << "Schwache Passwoerter:     " << report.weak_count << "\n";
    std::cout << "Zu kurze Passwoerter (<8):" << report.short_count << "\n";
    std::cout << "Wiederverwendete Eintraege:" << report.reused_count << "\n";
    std::cout << "------------------------------------------------------------\n";
    std::cout << "Tresor-Gesundheitsscore:  " << report.health_score << " / 100\n";

    if (report.health_score >= 80) {
        std::cout << "Status: AUSGEZEICHNET - Dein Tresor ist sehr gut geschuetzt!\n";
    } else if (report.health_score >= 50) {
        std::cout << "Status: GUT - Aktualisiere schwache oder wiederverwendete Passwoerter.\n";
    } else {
        std::cout << "Status: WARNUNG - Viele Passwoerter sind unsicher oder mehrfach vergeben!\n";
    }
}

static void export_data(const EntryList* list) {
    std::cout << "\n=== Daten Exportieren ===\n";
    std::cout << "Format: [1] CSV  [2] JSON: ";
    std::string format;
    std::getline(std::cin, format);
    format = trim(format);

    std::cout << "Dateipfad fuer Export (z.B. backup.csv / backup.json): ";
    std::string path;
    std::getline(std::cin, path);
    path = trim(path);

    if (path.empty()) {
        std::cout << "Export abgebrochen.\n";
        return;
    }

    int success = 0;
    if (format == "2") {
        success = vault_export_json(list, path.c_str());
    } else {
        success = vault_export_csv(list, path.c_str());
    }

    if (success) {
        std::cout << "Export erfolgreich in '" << path << "' gespeichert.\n";
    } else {
        std::cout << "Fehler beim Exportieren in '" << path << "'.\n";
    }
}

static void import_data(EntryList* list) {
    std::cout << "\n=== Daten Importieren ===\n";
    std::cout << "Format: [1] CSV  [2] JSON: ";
    std::string format;
    std::getline(std::cin, format);
    format = trim(format);

    std::cout << "Dateipfad der Importdatei: ";
    std::string path;
    std::getline(std::cin, path);
    path = trim(path);

    if (path.empty()) {
        std::cout << "Import abgebrochen.\n";
        return;
    }

    int count = 0;
    int success = 0;
    if (format == "2") {
        success = vault_import_json(list, path.c_str(), &count);
    } else {
        success = vault_import_csv(list, path.c_str(), &count);
    }

    if (success) {
        std::cout << count << " Eintraege erfolgreich importiert!\n";
    } else {
        std::cout << "Fehler beim Importieren der Datei '" << path << "'.\n";
    }
}

static void print_menu() {
    std::cout << "\n========================================\n";
    std::cout << "               KEHL VAULT               \n";
    std::cout << "========================================\n";
    std::cout << "[1] Alle Eintraege auflisten\n";
    std::cout << "[2] Eintrag suchen\n";
    std::cout << "[3] Neuen Eintrag anlegen\n";
    std::cout << "[4] Eintrag bearbeiten\n";
    std::cout << "[5] Eintrag loeschen\n";
    std::cout << "[6] Passwort in Zwischenablage kopieren\n";
    std::cout << "[7] Sicherheits-Audit & Health-Report\n";
    std::cout << "[8] Backup exportieren (CSV/JSON)\n";
    std::cout << "[9] Daten importieren (CSV/JSON)\n";
    std::cout << "[V] Passwort-Sichtbarkeit umschalten (Aktuell: "
              << (g_show_passwords ? "Klartext" : "Maskiert") << ")\n";
    std::cout << "[0] Speichern & Beenden\n";
    std::cout << "Auswahl: ";
}

int main() {
    std::cout << "========================================\n";
    std::cout << "   Willkommen beim Kehl Vault Manager   \n";
    std::cout << "========================================\n";

    EntryList list;
    if (!entry_list_init(&list, 4)) {
        std::cerr << "Fehler bei der Initialisierung des Speichers.\n";
        return 1;
    }

    int is_encrypted = 0;
    int file_status = vault_is_encrypted_file(VAULT_FILENAME, &is_encrypted);

    if (file_status && is_encrypted) {
        std::cout << "Verschluesselter Tresor '" << VAULT_FILENAME << "' gefunden.\n";
        int unlocked = 0;
        while (!unlocked) {
            std::string pass = read_masked_input("Master-Passwort zum Entsperren eingeben: ");
            if (pass.empty()) {
                std::cout << "Abbruch durch Benutzer.\n";
                entry_list_destroy(&list);
                return 0;
            }

            int load_res = vault_load_from_file(&list, VAULT_FILENAME, pass.c_str());
            if (load_res == VAULT_SUCCESS) {
                g_master_password = pass;
                unlocked = 1;
                std::cout << "Tresor erfolgreich entsperrt (" << list.count << " Eintraege geladen).\n";
            } else if (load_res == VAULT_ERR_INVALID_PASSWORD) {
                std::cout << "Falsches Master-Passwort! Bitte erneut versuchen.\n";
            } else {
                std::cout << "Fehler beim Laden des Tresors (Code: " << load_res << ").\n";
                break;
            }
        }
    } else if (file_status && !is_encrypted) {
        std::cout << "Unverschluesselte Tresordatei gefunden. Wird geladen...\n";
        vault_load_from_file(&list, VAULT_FILENAME, NULL);
    } else {
        std::cout << "Keine bestehende Tresordatei gefunden. Neuer Tresor wird erstellt.\n";
        std::string new_master = read_masked_input("Lege ein Master-Passwort fuer deinen neuen Tresor fest: ");
        g_master_password = trim(new_master);
        if (g_master_password.empty()) {
            std::cout << "Kein Master-Passwort vergeben (Tresor bleibt unverschluesselt).\n";
        } else {
            std::cout << "Master-Passwort gesetzt. Dein Tresor wird automatisch verschluesselt.\n";
        }
    }

    bool running = true;
    while (running) {
        print_menu();
        std::string choice_str;
        if (!std::getline(std::cin, choice_str)) {
            break;
        }
        choice_str = trim(choice_str);

        if (choice_str == "1") {
            list_all_entries(&list);
        } else if (choice_str == "2") {
            search_entries(&list);
        } else if (choice_str == "3") {
            add_entry(&list);
        } else if (choice_str == "4") {
            edit_entry(&list);
        } else if (choice_str == "5") {
            delete_entry(&list);
        } else if (choice_str == "6") {
            copy_password_to_clipboard(&list);
        } else if (choice_str == "7") {
            show_security_audit(&list);
        } else if (choice_str == "8") {
            export_data(&list);
        } else if (choice_str == "9") {
            import_data(&list);
        } else if (choice_str == "v" || choice_str == "V") {
            g_show_passwords = !g_show_passwords;
            std::cout << "Passwortanzeige ist nun: " << (g_show_passwords ? "KLARTEXT" : "MASKIERT") << "\n";
        } else if (choice_str == "0" || choice_str == "exit" || choice_str == "q") {
            running = false;
        } else {
            std::cout << "Ungueltige Option. Bitte 0-9 oder V waehlen.\n";
        }
    }

    std::cout << "\nTresor wird in '" << VAULT_FILENAME << "' gespeichert...\n";
    const char* pass_to_use = g_master_password.empty() ? NULL : g_master_password.c_str();
    if (vault_save_to_file(&list, VAULT_FILENAME, pass_to_use) == VAULT_SUCCESS) {
        std::cout << "Tresor erfolgreich gespeichert (" << list.count << " Eintraege).\n";
    } else {
        std::cerr << "Fehler beim Speichern des Tresors.\n";
    }

    entry_list_destroy(&list);
    std::cout << "Auf Wiedersehen!\n";
    return 0;
}
