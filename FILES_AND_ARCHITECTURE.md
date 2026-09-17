# Kehl Vault - Dateistruktur und Gesamtarchitektur (FILES_AND_ARCHITECTURE.md)

Diese Dokumentation erklärt das Gesamtsystem `kehl-vault` auf Architektur-, Komponenten- und Dateiebene. Sie beschreibt die Aufgabenverteilung, Interaktionen, Kontrollflüsse, Datenflüsse und Sicherheitsmechanismen des Gesamtsystems.

---

## 1. Detaillierte Dateimatrix

| Pfad | Dateityp | Hauptverantwortung | Wichtigste Symbole | Eingehende Abhängigkeiten | Ausgehende Abhängigkeiten |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `secure_mem.h` / `.c` | C-Header / C-Source | Compiler-resistentes Speicherlöschen | `secure_zero_memory`, `secure_zero_string` | `crypto`, `entry`, `storage`, `main`, `tests` | Windows `SecureZeroMemory` / C11 `memset_s` |
| `crypto.h` / `.c` | C-Header / C-Source | Kryptographische Primitiven | `sha256_*`, `hmac_sha256`, `pbkdf2_*`, `chacha20_*`, `crypto_constant_time_cmp` | `storage`, `totp`, `tests` | `secure_mem` |
| `entry.h` / `.c` | C-Header / C-Source | In-Memory Datenhaltung & dynamisches Array | `Entry`, `EntryList`, `entry_list_init`, `entry_list_add`, `entry_list_remove`, `entry_list_free` | `storage`, `audit`, `impex`, `search`, `history`, `expiry`, `main`, `tests` | `secure_mem`, `stdlib.h` (`malloc`/`realloc`/`free`) |
| `password.h` / `.c` | C-Header / C-Source | Passwort-Regeln, CSPRNG & Diceware-Generator | `password_check_*`, `password_calculate_strength`, `password_get_random_bytes`, `password_generate_*` | `main`, `audit`, `tests` | `secure_mem`, Win32 `BCryptGenRandom` / POSIX `/dev/urandom` |
| `storage.h` / `.c` | C-Header / C-Source | Encrypt-then-MAC Dateipersistenz | `EncryptedVaultHeader`, `vault_save_encrypted`, `vault_load_encrypted` | `main`, `backup`, `tests` | `entry`, `crypto`, `secure_mem`, C I/O (`fopen`/`fwrite`/`fread`) |
| `profile.h` / `.c` | C-Header / C-Source | Multi-Vault Datenbank-Switcher | `VaultProfile`, `ProfileList`, `profile_discover`, `profile_set_active` | `main`, `tests` | Win32 `FindFirstFileA` / POSIX `opendir` |
| `category.h` / `.c` | C-Header / C-Source | Klassifizierung & Tag-Parsing | `VaultCategory`, `category_to_string`, `category_parse_tags`, `category_has_tag` | `main`, `tests` | C String-Bibliothek |
| `totp.h` / `.c` | C-Header / C-Source | 2FA-Code-Generator (RFC 6238) | `totp_base32_decode`, `totp_generate_current`, `totp_generate` | `main`, `tests` | `crypto` (HMAC-SHA1), `time.h` |
| `history.h` / `.c` | C-Header / C-Source | Versionsverlauf & Papierkorb | `HistoryLog`, `TrashBin`, `history_record_change`, `trash_bin_restore` | `main`, `tests` | `entry`, `secure_mem` |
| `expiry.h` / `.c` | C-Header / C-Source | Ablaufdaten & Alterstracking | `expiry_calculate_age_days`, `expiry_check_policy` | `main`, `tests` | `time.h` |
| `backup.h` / `.c` | C-Header / C-Source | Snapshot-Kopien & Backup-Rotation | `backup_create_snapshot`, `backup_rotate` | `main`, `tests` | Win32 / POSIX Dateisystem-APIs |
| `search.h` / `.c` | C-Header / C-Source | Fehlertolerante Fuzzy-Suche | `search_levenshtein_distance`, `search_vault_fuzzy` | `main`, `tests` | `entry`, `string.h` |
| `impex.h` / `.c` | C-Header / C-Source | CSV & JSON Import/Export | `impex_export_csv`, `impex_import_csv`, `impex_export_json`, `impex_import_json` | `main`, `tests` | `entry` |
| `clipboard.h` / `.c` | C-Header / C-Source | Sicheres Kopieren ins Clipboard | `clipboard_copy_text` | `main`, `tests` | Win32 `OpenClipboard`/`SetClipboardData` / POSIX Pipes |
| `audit.h` / `.c` | C-Header / C-Source | Sicherheitsbericht & Duplikat-Erkennung | `VaultAuditReport`, `audit_vault` | `main`, `tests` | `entry`, `password` |
| `main.cpp` | C++20 Source | Interaktives CLI & Anwendungssteuerung | `main()`, `run_cli_menu()`, `read_masked_input()` | Keine (Einstiegspunkt) | Alle Header (`entry.h`, `storage.h`, `crypto.h`, etc.) |
| `tests/test_vault.cpp` | C++20 Source | Automatisierte Testsuite | `main()` (Test-Runner), 15 modulare Testfunktionen | Keine (Test-Einstiegspunkt) | Alle Header (`entry.h`, `storage.h`, `crypto.h`, etc.) |
| `CMakeLists.txt` | CMake Buildscript | Build-Steuerung & Target-Definition | Targets: `kehl_vault`, `test_vault` | IDE / CI-Runner | GCC/Clang/MSVC, `bcrypt.lib` |

---

## 2. Gesamtarchitektur und Komponentenmodell

Das System folgt einer strikten Schichtenarchitektur (Layered Architecture):

```mermaid
graph TD
    subgraph UI_Layer [Präsentations- & Interaktionsschicht (C++20)]
        CLI[main.cpp - Interaktives CLI Menü]
        Tests[tests/test_vault.cpp - Testsuite]
    end

    subgraph Service_Layer [Fachlogik & Funktionsmodule (C17)]
        Auth[totp.c - 2FA Authenticator]
        Pass[password.c - Kennwort-Engine & Diceware]
        Aud[audit.c - Sicherheits-Audit]
        Hist[history.c - Historie & Papierkorb]
        Exp[expiry.c - Altersüberwachung]
        Fuzz[search.c - Fuzzy Suche]
        Imp[impex.c - CSV/JSON Datenaustausch]
        Prof[profile.c - Profil-Switcher]
        Clip[clipboard.c - Zwischenablage]
        Back[backup.c - Snapshot-Manager]
    end

    subgraph Core_Layer [Datenkern & Persistenz (C17)]
        Model[entry.c - In-Memory Vault Model]
        Store[storage.c - Encrypt-then-MAC File I/O]
        Crypt[crypto.c - SHA256 / HMAC / PBKDF2 / ChaCha20]
        Mem[secure_mem.c - Memory Scrubbing / Zeroization]
    end

    subgraph OS_Layer [Betriebssystem & Hardware]
        RNG[Windows CNG BCryptGenRandom / dev/urandom]
        Disk[(Dateisystem: vault.dat / vault.dat.tmp)]
        ClipOS[System Clipboard]
    end

    CLI --> Service_Layer
    CLI --> Core_Layer
    Tests --> Service_Layer
    Tests --> Core_Layer

    Service_Layer --> Model
    Store --> Crypt
    Store --> Mem
    Store --> Model
    Back --> Store
    Pass --> RNG
    Store --> Disk
    Clip --> ClipOS
    Crypt --> Mem
    Model --> Mem
```

---

## 3. Typischer Programmablauf (End-to-End Lifecycle)

1. **Programmstart (`main.cpp`)**:
   - `profile_discover(&profiles)` durchsucht das aktuelle Arbeitsverzeichnis nach vorhandenen Tresor-Dateien (`vault.dat`, `vault_work.dat`, etc.).
   - Das aktive Profil wird gewählt (Standard: `vault.dat`).
2. **Authentifizierung & Entschlüsselung**:
   - `main.cpp` fordert den Benutzer auf, das Master-Passwort maskiert einzugeben (über `_getch()` ohne Terminal-Echo).
   - Falls die Tresordatei existiert:
     - `vault_load_encrypted()` liest den `EncryptedVaultHeader` ein.
     - PBKDF2 leitet aus dem eingegebenen Passwort und dem gelesenen `salt` 64 Schlüsselbytes ab.
     - HMAC-SHA256 validiert die Integrität des Chiffrats in konstanter Zeit (`crypto_constant_time_cmp`).
     - Bei Erfolg entschlüsselt ChaCha20 die Daten direkt in die dynamische `EntryList`.
     - Bei Misserfolg bricht das Programm ab, ohne Klartext freizugeben.
   - Falls die Datei nicht existiert, wird ein neuer Tresor im RAM initialisiert.
3. **Interaktive CLI-Schleife**:
   - Der Benutzer führt Operationen aus (z. B. Einträge hinzufügen mit dem Kennwortgenerator, Suchen per Fuzzy-Matching, TOTP-Codes abrufen, Audit-Reports anzeigen).
4. **Beenden & Persistieren**:
   - Beim Beenden (`Option 10`) ruft das Programm `backup_create_snapshot()` auf, um eine Sicherungskopie anzulegen.
   - `vault_save_encrypted()` generiert einen neuen kryptographischen Salt und eine neue ChaCha20-Nonce, verschlüsselt die Daten, berechnet den HMAC-Tag und schreibt atomar über eine `.tmp`-Datei auf die Festplatte.
   - `entry_list_free()` nullt vor der Speicherfreigabe alle sensiblen Strukturen via `secure_zero_memory()` im RAM.

---

## 4. Call Flow & Funktionsaufrufketten

### Speicher-Aufrufkette (Speichern & Verschlüsseln)
```
main()
 └── vault_save_encrypted("vault.dat", &list, master_password)
      ├── password_get_random_bytes(salt, 16)       [OS Entropy]
      ├── password_get_random_bytes(nonce, 12)      [OS Entropy]
      ├── pbkdf2_hmac_sha256(password, salt, 100000, keys, 64)
      │    └── hmac_sha256() -> sha256_update()
      ├── chacha20_crypt(enc_key, nonce, 1, plaintext, ciphertext, len)
      │    └── chacha20_block() -> Quarterrounds
      ├── hmac_sha256(auth_key, ciphertext, len, auth_tag)
      ├── fopen("vault.dat.tmp", "wb")
      ├── fwrite(header + ciphertext)
      ├── fflush() & fclose()
      ├── remove("vault.dat") & rename("vault.dat.tmp", "vault.dat")
      └── secure_zero_memory(keys, 64)
```

### Lade- & Verifikationskette
```
main()
 └── vault_load_encrypted("vault.dat", &list, master_password)
      ├── fopen("vault.dat", "rb") & fread(header)
      ├── pbkdf2_hmac_sha256(password, header.salt, 100000, keys, 64)
      ├── hmac_sha256(auth_key, ciphertext, len, computed_tag)
      ├── crypto_constant_time_cmp(header.auth_tag, computed_tag, 32)
      │    ├── [MISSMATCH] -> return 0 (Abbruch, falsches Passwort/korrupt)
      │    └── [MATCH] -> Fortfahren
      ├── chacha20_crypt(enc_key, header.nonce, 1, ciphertext, plaintext, len)
      ├── entry_list_init(&list, header.entry_count)
      ├── memcpy(list->entries, plaintext, len)
      └── secure_zero_memory(keys + plaintext)
```

---

## 5. Speichermodell (Memory Model & Ownership)

- **Stack Allocation**:
  - Alle lokalen Variablen, Krypto-Kontexte (`SHA256_CTX`), temporäre Passworteingaben (`char pass[128]`) und Profil-Listen liegen auf dem Stack.
  - Sobald Funktionen verlassen werden, werden sensible Stack-Puffer explizit mittels `secure_zero_memory()` bereinigt.
- **Heap Allocation (`malloc` / `realloc` / `free`)**:
  - Ausschließlich das dynamische Daten-Array `list->entries` in `EntryList` wird auf dem Heap verwaltet.
  - **Ownership-Regel**: Die `EntryList` besitzt (owns) den allokierten Speicherblock exklusiv. Weder UI noch Speicherfunktionen behalten Zeiger auf interne Daten nach Abschluss einer Operation.
  - **Destruktor-Invariante**: Vor jedem Aufruf von `free(list->entries)` wird der Heap-Block mit `secure_zero_memory(list->entries, sizeof(Entry) * list->capacity)` genullt.

---

## 6. Build-System und Compiler-Pipeline

1. **Präprozessor**:
   - Löst `#include`, Include-Guards (`#ifndef ... #define`) und Makros (`ROTRIGHT`, `CH`, `MAJ`) auf.
2. **Kompilierung**:
   - C-Compiler (`gcc` / `clang`) übersetzt C17-Module (`entry.c`, `crypto.c`, etc.) in native Maschinencode-Objektdateien (`.obj` / `.o`).
   - C++-Compiler (`g++` / `clang++`) übersetzt C++20-Quelldateien (`main.cpp`, `test_vault.cpp`).
3. **Linker**:
   - Verknüpft alle Objektdateien.
   - Bindet unter Windows die Systembibliothek `bcrypt.lib` für den OS-Zufallsgenerator ein.
   - Erzeugt die fertigen Executables `kehl_vault.exe` und `test_vault.exe`.

---

## 7. Sicherheitsarchitektur (Security Model)

1. **Verschlüsselung im Ruhezustand (At-Rest Security)**:
   - Authentifizierte Verschlüsselung (Encrypt-then-MAC) verhindert Bit-Flipping- und Padding-Oracle-Angriffe.
   - ChaCha20 mit 256-Bit Schlüssellänge bietet höchste Sicherheit gegen Kryptoanalyse.
2. **Schutz vor Wörterbuchangriffen**:
   - PBKDF2-HMAC-SHA256 mit 100.000 Iterationen und individuellem 16-Byte Zufallssalt pro Speichervorgang macht Rainbow-Tables und Brute-Force-Hardware (GPUs/ASICs) ineffizient.
3. **Schutz vor Seitenkanalangriffen (Side-Channel Protection)**:
   - Konstanter Bytevergleich (`crypto_constant_time_cmp`) verhindert Laufzeit-basierte Angriffe auf Authentifizierungs-Tags.
4. **Schutz vor Cold-Boot & RAM-Dumps**:
   - `secure_zero_memory()` garantiert, dass Passwörter und Schlüssel unmittelbar nach der Verwendung aus dem RAM getilgt werden.
5. **Echte Entropie**:
   - Windows Cryptography Next Generation (CNG) Kernel-RNG bzw. POSIX `/dev/urandom` für alle Schlüssel, Nonces, Salts und Kennwörter.

---

## 8. Didaktische Lernlandkarte (Learning Map)

Um das Projekt `kehl-vault` als Entwickler von Grund auf zu verstehen, empfiehlt sich folgende Lesereihenfolge:

1. **Einstieg in die Datenstrukturen**:
   - `entry.h` & `entry.c` (C-Speicherlayout, Puffer, `malloc`/`realloc`/`free`).
2. **Speichersicherheit & RAM-Hygiene**:
   - `secure_mem.h` & `secure_mem.c` (`volatile`-Pointer, Compiler-Optimierungen, Dead Code Elimination).
3. **Entropie & Passwort-Generierung**:
   - `password.h` & `password.c` (OS-Kernel RNG, Regelsätze, Modulo-Mapping, Diceware).
4. **Kryptographie-Grundlagen**:
   - `crypto.h` & `crypto.c` (SHA-256 Bitmanipulation, HMAC, PBKDF2 Schlüsselableitung, ChaCha20 ARX-Runden).
5. **Dateipersistenz & Verschlüsselungs-Pipeline**:
   - `storage.h` & `storage.c` (Encrypt-then-MAC, Dateiformat-Header, atomare Schreibvorgänge via Rename).
6. **Erweiterte Subsysteme**:
   - `totp.h` & `totp.c` (RFC 6238, Base32, dynamisches Truncating).
   - `search.h` & `search.c` (Levenshtein-Distanz / Dynamic Programming).
   - `history.h` & `backup.h` & `profile.h` & `category.h` & `expiry.h` & `audit.h` & `impex.h`.
7. **Gesamtintegration & UI**:
   - `main.cpp` (Zusammenführen aller Module, maskierte Terminaleingaben, Menüschleife).
8. **Qualitätssicherung & Regressionstests**:
   - `tests/test_vault.cpp` (Verifikation aller Invarianten).
