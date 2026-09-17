# Kehl Vault - Vollständige Code-Erklärung (CODE_EXPLANATION.md)

Diese Dokumentation bietet eine tiefgehende, didaktische und technische Analyse jeder einzelnen Quellcodedatei im Projekt `kehl-vault`. Sie richtet sich an Entwickler, die den Code auf Systemebene (C17 / C++20, Speicherlayout, Pointer, Bitoperationen, Kryptographie, Betriebssystem-APIs) von Grund auf verstehen und eigenständig weiterentwickeln möchten.

---

## Inhaltsverzeichnis der analysierten Dateien

1. [Build- und Konfigurationsdateien](#1-build--und-konfigurationsdateien)
   - `CMakeLists.txt`
   - `.github/workflows/build.yml`
   - `.gitignore`
   - `scripts/run-and-sync.ps1`
   - `scripts/git-pull.ps1`
2. [Sicherheits- und Speicher-Grundbausteine](#2-sicherheits--und-speicher-grundbausteine)
   - `secure_mem.h` & `secure_mem.c`
   - `crypto.h` & `crypto.c`
3. [Datenmodell und Datenstrukturen](#3-datenmodell-und-datenstrukturen)
   - `entry.h` & `entry.c`
   - `category.h` & `category.c`
4. [Kryptographische Passwort- und Passphrasenlogik](#4-kryptographische-passwort--und-passphrasenlogik)
   - `password.h` & `password.c`
5. [Zwei-Faktor-Authentifizierung (TOTP)](#5-zwei-faktor-authentifizierung-totp)
   - `totp.h` & `totp.c`
6. [Persistenz und Dateiverwaltung](#6-persistenz-und-dateiverwaltung)
   - `storage.h` & `storage.c`
   - `backup.h` & `backup.c`
   - `profile.h` & `profile.c`
7. [Audit, Suche, Versionierung und Datenaustausch](#7-audit-suche-versionierung-und-datenaustausch)
   - `audit.h` & `audit.c`
   - `expiry.h` & `expiry.c`
   - `history.h` & `history.c`
   - `search.h` & `search.c`
   - `impex.h` & `impex.c`
   - `clipboard.h` & `clipboard.c`
8. [CLI Benutzeroberfläche und Einstiegspunkt](#8-cli-benutzeroberfl%C3%A4che-und-einstiegspunkt)
   - `main.cpp`
9. [Automatisierte Testsuite](#9-automatisierte-testsuite)
   - `tests/test_vault.cpp`
10. [Dokumentationsdateien](#10-dokumentationsdateien)
   - `README.md`
   - `CODEBASE_DOCUMENTATION.md`

---

## 1. Build- und Konfigurationsdateien

### `CMakeLists.txt`
- **Pfad:** `CMakeLists.txt`
- **Aufgabe:** Zentrale Build-Konfiguration für das CMake-Build-System. Definiert Compiler-Standards, Include-Pfade, Targets (die Hauptanwendung `kehl_vault` und die Test-Executable `test_vault`) sowie plattformspezifische Bibliotheksbindungen.

#### Detaillierte Code-Analyse:
```cmake
cmake_minimum_required(VERSION 3.25)
project(kehl_vault LANGUAGES C CXX)
```
- `cmake_minimum_required(VERSION 3.25)`: Fordert mindestens CMake Version 3.25 an. Garantiert, dass moderne CMake-Befehle und Compiler-Features verfügbar sind.
- `project(kehl_vault LANGUAGES C CXX)`: Benennt das Projekt `kehl_vault` und aktiviert sowohl den C- als auch den C++-Compiler (da der Core in reinem C17 und das UI/Tests in C++20 geschrieben sind).

```cmake
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```
- `set(CMAKE_C_STANDARD 17)`: Setzt den C-Standard auf ISO/IEC 9899:2018 (C17).
- `set(CMAKE_C_STANDARD_REQUIRED ON)`: Verbietet ein Downgrade des C-Standards, falls der Compiler C17 nicht vollständig beherrscht.
- `set(CMAKE_CXX_STANDARD 20)`: Aktiviert den C++20-Standard (ISO/IEC 14882:2020) für moderne Sprachfeatures (wie `std::string_view`, verbesserte Initialisierer und Formatierungsunterstützung).
- `set(CMAKE_CXX_STANDARD_REQUIRED ON)`: Erzwingt den C++20-Standard.

```cmake
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
```
- Fügt das Wurzelverzeichnis des Repositories zu den Include-Suchpfaden des Compilers hinzu. Dadurch können Quelldateien Header direkt mit `#include "entry.h"` statt mit relativen Pfaden wie `../../entry.h` einbinden.

```cmake
add_executable(kehl_vault
        main.cpp
        entry.c entry.h
        password.c password.h
        storage.c storage.h
        crypto.c crypto.h
        clipboard.c clipboard.h
        audit.c audit.h
        impex.c impex.h
        profile.c profile.h
        category.c category.h
        totp.c totp.h
        history.c history.h
        expiry.c expiry.h
        backup.c backup.h
        search.c search.h
        secure_mem.c secure_mem.h
)
```
- `add_executable(kehl_vault ...)`: Kompiliert alle gelisteten `.c`- und `.cpp`-Dateien zu Objektdateien (`.o` bzw. `.obj`) und linkt sie zusammen mit `main.cpp` zur ausführbaren Hauptanwendung `kehl_vault` (unter Windows: `kehl_vault.exe`).
- Die `.h`-Headerdateien werden ebenfalls gelistet, damit moderne IDEs (wie CLion) die Header-Hierarchie und Code-Navigation im Projektbaum korrekt abbilden.

```cmake
add_executable(test_vault
        tests/test_vault.cpp
        entry.c entry.h
        ...
        secure_mem.c secure_mem.h
)
```
- Erstellt das Test-Target `test_vault`. Linkt dieselben C-Module wie die Hauptanwendung, ersetzt jedoch `main.cpp` durch die Testsuite `tests/test_vault.cpp`.

```cmake
enable_testing()
add_test(NAME test_vault COMMAND test_vault)
```
- `enable_testing()`: Aktiviert das integrierte CTest-Framework.
- `add_test(...)`: Registriert die ausführbare Datei `test_vault` als automatisierten Testfall, der via `ctest` aufgerufen werden kann.

```cmake
if (WIN32)
    target_link_libraries(kehl_vault bcrypt)
    target_link_libraries(test_vault bcrypt)
endif ()
```
- `if (WIN32)`: Bedingung greift, wenn das Zielbetriebssystem Windows ist.
- `target_link_libraries(... bcrypt)`: Linkt die Windows-Systembibliothek `bcrypt.lib` / `libbcrypt.a` gegen beide Targets. Dies ist zwingend erforderlich, da `password.c` und `storage.c` die Windows Cryptography Next Generation (CNG) Funktion `BCryptGenRandom` zur Generierung kryptographisch sicherer Zufallsbytes aufrufen.

---

### `.github/workflows/build.yml`
- **Pfad:** `.github/workflows/build.yml`
- **Aufgabe:** CI/CD-Pipeline (Continuous Integration) via GitHub Actions. Führt bei jedem Push und Pull-Request auf dem `master`-Branch einen automatisierten Clean-Build und die gesamte Testsuite unter Linux (Ubuntu) aus.

#### Detaillierte Code-Analyse:
- `name: Build`: Name des GitHub Actions Workflows.
- `on: push: branches: [master]`, `pull_request: branches: [master]`, `workflow_dispatch:`: Trigger, die den Workflow starten (Änderungen am `master`-Branch oder manueller Start).
- `runs-on: ubuntu-latest`: Definiert eine frische virtuelle Ubuntu-Linux-Umgebung.
- `steps:`:
  1. `actions/checkout@v4`: Klont das Git-Repository in die VM.
  2. `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`: Generiert die Build-Dateien mit Debug-Symbolen.
  3. `cmake --build build --parallel`: Kompiliert das gesamte Projekt parallel auf allen verfügbaren CPU-Kernen.
  4. `ctest --test-dir build --output-on-failure`: Führt alle registrierten Tests aus und gibt bei Fehlern detaillierte Stacktraces/Logs aus.
  5. `./build/kehl_vault`: Führt einen Smoke-Test der CLI aus, um sicherzustellen, dass die Executable dynamisch gelinkt und ohne Segfaults gestartet werden kann.

---

### `.gitignore`
- **Pfad:** `.gitignore`
- **Aufgabe:** Schließt Build-Artefakte, temporäre Dateien, IDE-Konfigurationen und unverschlüsselte Backups von der Git-Versionskontrolle aus.

#### Wichtige Einträge:
- `cmake-build-*/`, `build/`, `bin/`: Verhindert das Einchecken von Binärdateien und Kompilaten.
- `.idea/`, `.vscode/`: Ignoriert IDE-spezifische Benutzerdaten.
- `*.dat`, `vault*.dat`, `*.bak`: Verhindert, dass lokale Vault-Datenbanken versehentlich öffentlich ins Git-Repo committet werden.

---

### `scripts/run-and-sync.ps1` & `scripts/git-pull.ps1`
- **Pfad:** `scripts/run-and-sync.ps1` und `scripts/git-pull.ps1`
- **Aufgabe:** PowerShell-Automatisierungsskripte für Windows-Entwickler.
- `run-and-sync.ps1`:
  - Liest `CMakeCache.txt` aus, um den aktiven Compiler-Pfad (MinGW GCC/Clang) zu ermitteln.
  - Setzt die `PATH`-Umgebungsvariable temporär auf das Compiler-Verzeichnis.
  - Führt `kehl_vault.exe` aus.
  - Prüft den Exit-Code (`$LASTEXITCODE`). War der Lauf erfolgreich (`0`), werden geänderte Dateien automatisch via `git add`, `git commit` und `git push` mit Remote synchronisiert.
- `git-pull.ps1`:
  - Führt `git pull --ff-only` aus, um saubere Fast-Forward-Updates von GitHub abzurufen.

---

## 2. Sicherheits- und Speicher-Grundbausteine

### `secure_mem.h` & `secure_mem.c`
- **Pfad:** `secure_mem.h`, `secure_mem.c`
- **Aufgabe:** Sicheres, compiler-optimierungsresistentes Löschen (Zeroization) von sensiblem Speicher (Passwörter, kryptographische Schlüssel, Master-Passwörter, Tokens) im RAM.

#### Technische Motivation:
Standardfunktionen wie `memset(buffer, 0, size)` werden von modernen Compilern (GCC, Clang, MSVC) durch "Dead Code Elimination" häufig wegoptimiert, wenn der Puffer danach den Gültigkeitsbereich verlässt und nicht mehr gelesen wird. Dadurch verbleiben Klartext-Passwörter im RAM und sind anfällig für Cold-Boot-Attacken oder Speicher-Dumps.

#### Detaillierte Code-Analyse (`secure_mem.h`):
```c
#ifndef KEHL_VAULT_SECURE_MEM_H
#define KEHL_VAULT_SECURE_MEM_H
#include <stddef.h>
```
- Include-Guard `#ifndef ... #define`: Verhindert mehrfache Deklarationen bei zirkulärem Include.
- `<stddef.h>`: Definiert `size_t` (vorzeichenloser Integer für Speichergrößen).

```c
#ifdef __cplusplus
extern "C" {
#endif
```
- `extern "C"`: Verhindert C++ Name Mangling. Ermöglicht das nahtlose Linken von C-Funktionen in C++-Dateien (`main.cpp`, `test_vault.cpp`).

```c
void secure_zero_memory(void* ptr, size_t size);
void secure_zero_string(char* str);
```
- `secure_zero_memory(void* ptr, size_t size)`: Überschreibt `size` Bytes ab Speicheradresse `ptr` unwiderruflich mit Nullen.
- `secure_zero_string(char* str)`: Berechnet via `strlen` die Länge eines nullterminierten Strings und nullt diesen inklusive Nullterminator.

#### Detaillierte Code-Analyse (`secure_mem.c`):
```c
#include "secure_mem.h"
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif
```
- Bindet unter Windows `<windows.h>` ein, um Zugriff auf die Kernel-Funktion `SecureZeroMemory` zu erhalten.

```c
void secure_zero_memory(void* ptr, size_t size) {
    if (!ptr || size == 0) {
        return;
    }
#if defined(_WIN32) || defined(_WIN64)
    SecureZeroMemory(ptr, size);
#elif defined(__STDC_LIB_EXT1__)
    memset_s(ptr, size, 0, size);
#else
    volatile unsigned char* p = (volatile unsigned char*)ptr;
    while (size--) {
        *p++ = 0;
    }
#endif
}
```
1. `if (!ptr || size == 0) return;`: Defensiver Nullpointer- und Längen-Check. Verhindert Speicherzugriffsfehler (Segmentation Faults / Access Violations).
2. Windows-Zweig (`SecureZeroMemory`): Ruft die Win32-Inline-Funktion auf, die intern als volatile Speicheroperation implementiert ist und garantiert nicht vom Compiler verworfen wird.
3. C11-Zweig (`memset_s`): Verwendet den optionalen C11-Sicherheitsstandard, falls verfügbar.
4. Fallback-Zweig (`volatile unsigned char*`):
   - Der Cast auf `volatile unsigned char*` signalisiert dem Compiler, dass der referenzierte Speicherbereich externe Seiteneffekte haben kann.
   - Der Optimierer darf Schleifenschritte mit `volatile`-Zugriffen niemals eliminieren oder umordnen.

```c
void secure_zero_string(char* str) {
    if (!str) return;
    size_t len = strlen(str);
    secure_zero_memory(str, len);
}
```
- Ermittelt die Länge und ruft `secure_zero_memory` auf.

---

### `crypto.h` & `crypto.c`
- **Pfad:** `crypto.h`, `crypto.c`
- **Aufgabe:** Vollständige, autonome Implementierung von Industriestandard-Kryptographie ohne externe Abhängigkeiten:
  - **SHA-256** (FIPS 180-4)
  - **HMAC-SHA256** (RFC 2104)
  - **PBKDF2-HMAC-SHA256** (RFC 8018 PKCS #5 v2.1 mit 100.000 Runden)
  - **ChaCha20 Stream Cipher** (RFC 8439)
  - **Constant-Time Memory Compare** (Schutz vor Timing-Side-Channel-Angriffen)

#### Detaillierte Code-Analyse (`crypto.h`):
```c
#define SHA256_BLOCK_SIZE 64
#define SHA256_DIGEST_SIZE 32
#define CHACHA20_KEY_SIZE 32
#define CHACHA20_NONCE_SIZE 12
#define PBKDF2_DEFAULT_ITERATIONS 100000
```
- Konstanten für Block- und Digest-Größen in Bytes.
- `PBKDF2_DEFAULT_ITERATIONS 100000`: Wehrt Brute-Force- und Dictionary-Angriffe auf das Master-Passwort durch künstliche Rechenzeit-Erhöhung ab.

```c
typedef struct {
    uint8_t data[64];
    uint32_t datalen;
    uint64_t bitlen;
    uint32_t state[8];
} SHA256_CTX;
```
- Struktur für den Zustand eines fortlaufenden SHA-256 Hashing-Vorgangs (`Init` -> `Update` -> `Final`).
  - `data[64]`: Puffer für unvollständige 512-Bit (64-Byte) Nachrichtenblöcke.
  - `datalen`: Aktuelle Byteanzahl im Puffer (0–63).
  - `bitlen`: Gesamtlänge der verarbeiteten Nachricht in Bits (erforderlich für das SHA-256 Merkle-Damgård Padding).
  - `state[8]`: Die acht 32-Bit Zustandsvariablen ($H_0 \dots H_7$), initialisiert mit den Bruchteilen der Quadratwurzeln der ersten 8 Primzahlen.

#### Detaillierte Code-Analyse (`crypto.c`):

##### 1. Constant-Time Comparison:
```c
int crypto_constant_time_cmp(const void* a, const void* b, size_t len) {
    const uint8_t* p1 = (const uint8_t*)a;
    const uint8_t* p2 = (const uint8_t*)b;
    uint8_t diff = 0;
    for (size_t i = 0; i < len; ++i) {
        diff |= (p1[i] ^ p2[i]);
    }
    return diff == 0 ? 0 : 1;
}
```
- **Warum diese Funktion existiert:** Standard `memcmp` bricht beim ersten ungleichen Byte sofort ab (`early exit`). Ein Angreifer kann über präzise Laufzeitmessungen (Timing Attacks) Byte für Byte eines HMAC-Authentifizierungs-Tags erraten.
- **Funktionsweise:** Führt für alle `len` Bytes eine XOR-Verknüpfung (`^`) durch und akkumuliert eventuelle Bit-Unterschiede mittels Bitwise-OR (`|=`). Die Schleife benötigt unabhängig vom Inhalt immer exakt dieselbe Anzahl von CPU-Zyklen.

##### 2. SHA-256 Bit-Transformationen:
```c
#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))
#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))
```
- Bitweise Rotationen (`ROTRIGHT`) und nicht-lineare logische Funktionen (`CH` = Choose, `MAJ` = Majority) gemäß FIPS PUB 180-4.
- `sha256_transform`: Verarbeitet einen 64-Byte Block über 64 Runden unter Verwendung der K-Konstanten-Tabelle (`K[64]`).

##### 3. HMAC-SHA256 (Hash-based Message Authentication Code):
```c
void hmac_sha256(const uint8_t* key, size_t key_len, const uint8_t* data, size_t data_len, uint8_t* out_mac)
```
- Implementiert RFC 2104:
  - $HMAC(K, m) = H((K' \oplus opad) \parallel H((K' \oplus ipad) \parallel m))$
  - `ipad` (Inner Pad): Byte `0x36` 64-mal wiederholt.
  - `opad` (Outer Pad): Byte `0x5C` 64-mal wiederholt.
- Falls `key_len > 64`, wird der Schlüssel vorab mit SHA-256 auf 32 Bytes gehasht.
- Löscht nach der Berechnung alle temporären Schlüssel-Puffer mit `secure_zero_memory`.

##### 4. PBKDF2 (Password-Based Key Derivation Function 2):
```c
void pbkdf2_hmac_sha256(const char* password, size_t pass_len, const uint8_t* salt, size_t salt_len, uint32_t iterations, uint8_t* out_key, size_t out_key_len)
```
- Leitet aus einem variablen Master-Passwort und einem kryptographischen Salt kryptographisch starke Schlüssel ab.
- Berechnet für jeden 32-Byte Block $T_k = U_1 \oplus U_2 \oplus \dots \oplus U_{iterations}$ mittels iterativer HMAC-Berechnung.

##### 5. ChaCha20 Stream Cipher:
```c
#define CHACHA20_QUARTERROUND(a, b, c, d) \
    a += b; d ^= a; d = ROTL32(d, 16); \
    c += d; b ^= c; b = ROTL32(b, 12); \
    a += b; d ^= a; d = ROTL32(d, 8);  \
    c += d; b ^= c; b = ROTL32(b, 7);
```
- RFC 8439 konformer 256-Bit Stream Cipher.
- Die Quarterround-Funktion modifiziert vier 32-Bit Worte mittels Add-Rotate-XOR (ARX).
- Ein Block umfasst 16 32-Bit Worte (State): 4 Worte Konstanten ("expand 32-byte k"), 8 Worte Schlüssel (256 Bit), 1 Wort Counter (32 Bit), 3 Worte Nonce (96 Bit).
- 20 Runden (10 Spaltenrunden, 10 Diagonalrunden).
- Die erzeugte Keystream-Matrix wird per XOR mit dem Klartext verknüpft. Da XOR symmetrisch ist ($C = P \oplus K \implies P = C \oplus K$), ist die Entschlüsselungsfunktion identisch zur Verschlüsselungsfunktion.

---

## 3. Datenmodell und Datenstrukturen

### `entry.h` & `entry.c`
- **Pfad:** `entry.h`, `entry.c`
- **Aufgabe:** In-Memory Datenhaltung der Zugangsdaten und dynamisch wachsendes Array (`EntryList`) auf dem Heap.

#### Detaillierte Code-Analyse (`entry.h`):
```c
#define ENTRY_TITLE_SIZE 64
#define ENTRY_USERNAME_SIZE 64
#define ENTRY_PASSWORD_SIZE 64

typedef struct {
    char title[ENTRY_TITLE_SIZE];
    char username[ENTRY_USERNAME_SIZE];
    char password[ENTRY_PASSWORD_SIZE];
} Entry;
```
- `Entry`: Feste Puffergrößen (64 Bytes) verhindern Heap-Fragmentierung und garantieren ein planares, zusammenhängendes Speicherlayout (`sizeof(Entry) == 192 Bytes`).
- Alle Strings sind garantiert nullterminiert (`\0`).

```c
typedef struct {
    Entry* entries;
    int count;
    int capacity;
} EntryList;
```
- `EntryList`: Dynamisches Array.
  - `entries`: Zeiger auf den Heap-Speicherbereich (`Entry*`).
  - `count`: Aktuell belegte Slots.
  - `capacity`: Maximal reservierte Slots, bevor reallokiert werden muss.

#### Detaillierte Code-Analyse (`entry.c`):
```c
int entry_list_init(EntryList* list, int initial_capacity) {
    if (!list || initial_capacity <= 0) return 0;
    list->entries = (Entry*)malloc(sizeof(Entry) * initial_capacity);
    if (!list->entries) {
        list->count = 0;
        list->capacity = 0;
        return 0;
    }
    list->count = 0;
    list->capacity = initial_capacity;
    return 1;
}
```
- `malloc(sizeof(Entry) * initial_capacity)`: Allokiert zusammenhängenden Heap-Speicher.
- `if (!list->entries)`: Prüft auf `NULL` (Out-of-Memory Schutz).

```c
int entry_list_add(EntryList* list, const char* title, const char* username, const char* password) {
    if (!list) return 0;
    if (list->count >= list->capacity) {
        int new_capacity = (list->capacity == 0) ? 4 : list->capacity * 2;
        Entry* new_entries = (Entry*)realloc(list->entries, sizeof(Entry) * new_capacity);
        if (!new_entries) return 0;
        list->entries = new_entries;
        list->capacity = new_capacity;
    }
    ...
}
```
- **Geometrisches Wachstum (`capacity * 2`):** Verhindert häufige Reallokationen und reduziert die amortisierte Laufzeit für Einfügeoperationen auf $O(1)$.
- **Sichere Reallokation:** Verwendet `new_entries`. Falls `realloc` fehlschlägt, bleibt der ursprüngliche Zeiger `list->entries` intakt (kein Speicherleck).

```c
void entry_list_free(EntryList* list) {
    if (!list) return;
    if (list->entries) {
        secure_zero_memory(list->entries, sizeof(Entry) * list->capacity);
        free(list->entries);
        list->entries = NULL;
    }
    list->count = 0;
    list->capacity = 0;
}
```
- **Sicherheit & Destruktor:** Vor dem `free()` wird der gesamte Speicherbereich mittels `secure_zero_memory` genullt, damit keine Passwörter im freigegebenen Heap-Speicher verbleiben. Anschließend wird der Zeiger auf `NULL` gesetzt (Schutz vor Use-After-Free).

---

### `category.h` & `category.c`
- **Pfad:** `category.h`, `category.c`
- **Aufgabe:** Kategorisierung von Einträgen (`Login`, `Card`, `Secure Note`, `Identity`, `Other`) und Parsen kommagetrennter Tags.

#### Detaillierte Code-Analyse:
- `VaultCategory`: Aufzählungstyp (`enum`).
- `category_parse_tags`: Zerlegt Strings wie `"work, email, private"` in ein 2D-Array `char out_tags[][TAG_NAME_MAX]`.
  - Entfernt führende und nachgestellte Leerzeichen (Whitespace Trimming).
  - Garantiert Nullterminierung jedes einzelnen Tags.
- `category_has_tag`: Führt einen case-insensitiven Vergleich durch, um zu prüfen, ob ein Eintrag einen bestimmten Tag besitzt.

---

## 4. Kryptographische Passwort- und Passphrasenlogik

### `password.h` & `password.c`
- **Pfad:** `password.h`, `password.c`
- **Aufgabe:**
  - Validierungsregeln für Passwörter (Länge, Groß-/Kleinbuchstaben, Ziffern, Sonderzeichen).
  - Stärkeberechnung (Score 0 bis 5).
  - Abrufen echter kryptographischer Entropie vom Betriebssystem (`BCryptGenRandom` unter Windows, `/dev/urandom` unter Linux/POSIX).
  - Generierung sicherer Zufallspasswörter und Diceware-Passphrasen (aus einer kuratierten Wortliste mit 100 Wörtern).

#### Detaillierte Code-Analyse (`password.c`):
```c
int password_get_random_bytes(uint8_t* buffer, size_t length) {
    if (!buffer || length == 0) return 0;
#if defined(_WIN32) || defined(_WIN64)
    NTSTATUS status = BCryptGenRandom(NULL, buffer, (ULONG)length, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    return BCRYPT_SUCCESS(status) ? 1 : 0;
#else
    FILE* f = fopen("/dev/urandom", "rb");
    if (!f) return 0;
    size_t read_bytes = fread(buffer, 1, length, f);
    fclose(f);
    return (read_bytes == length) ? 1 : 0;
#endif
}
```
- **Echte Entropie:** Verwendet niemals `rand()` oder `srand(time(NULL))`, da Pseudozufallsgeneratoren vorhersagbar sind.
- **Windows:** Nutzt den kernelbasierten CNG-RNG (`BCryptGenRandom`).
- **POSIX:** Liest direkt aus dem CSPRNG-Kernel-Interface `/dev/urandom`.

```c
int password_generate_passphrase(char* buffer, size_t buffer_size, int word_count, const char* separator)
```
- Wählt mittels kryptographischer Zufallsbytes Wörter aus dem internen Wörterbuch `DICEWARE_WORDS` (100 Wörter) und verknüpft sie mit einem frei wählbaren Trennzeichen (z. B. `-` oder `.`).
- Erzeugt hochgradig einprägsame, aber entropiestarke Passphrasen (z. B. `galaxy-falcon-matrix-shadow`).

---

## 5. Zwei-Faktor-Authentifizierung (TOTP)

### `totp.h` & `totp.c`
- **Pfad:** `totp.h`, `totp.c`
- **Aufgabe:** Standardkonforme Generierung von zeitbasierten Einmalpasswörtern (2FA-Codes) gemäß **RFC 6238** (TOTP) und **RFC 4226** (HOTP).

#### Mathematischer & Technischer Hintergrund:
1. **Base32-Dekodierung:** Der geheime Schlüssel (z. B. `JBSWY3DPEHPK3PXP`) wird aus dem 5-Bit-Base32-Alphabet (A–Z, 2–7) in rohe Binärbytes dekodiert.
2. **Zeitintervall:** Der aktuelle Unix-Timestamp in Sekunden wird durch 30 geteilt:
   $$T = \lfloor \frac{\text{current\_unix\_time}}{30} \rfloor$$
3. **HMAC-SHA1 Hash:** $T$ wird als 64-Bit Big-Endian Integer mit dem dekodierten Secret per HMAC-SHA1 gehasht:
   $$HS = \text{HMAC-SHA1}(\text{Secret}, T)$$
4. **Dynamisches Truncation:** Das letzte Halbbyte (Nibble) des 20-Byte Hashes bestimmt den Offset $i \in [0, 15]$:
   $$\text{Offset} = HS[19] \& 0x0F$$
   Ein 31-Bit Integer wird extrahiert:
   $$P = ((HS[i] \& 0x7F) \ll 24) \mid ((HS[i+1] \& 0xFF) \ll 16) \mid ((HS[i+2] \& 0xFF) \ll 8) \mid (HS[i+3] \& 0xFF)$$
5. **6-stelliger Code:**
   $$\text{TOTP} = P \pmod{10^6}$$

#### Detaillierte Code-Analyse (`totp.c`):
- `totp_base32_decode`: Ignoriert Leerzeichen und Bindestriche und wandelt 8 Base32-Zeichen in 5 Datenbytes um.
- `totp_generate_current`: Ermittelt via `time(NULL)` die Systemzeit, berechnet den 6-stelligen Code und gibt über `remaining_seconds` die verbleibende Gültigkeitsdauer des aktuellen 30-Sekunden-Fensters zurück.

---

## 6. Persistenz und Dateiverwaltung

### `storage.h` & `storage.c`
- **Pfad:** `storage.h`, `storage.c`
- **Aufgabe:** Sichere, verschlüsselte Dateipersistenz auf der Festplatte mit atomarem Schreibvorgang.

#### Dateiformat-Spezifikation (Version 2 - Encrypted):
```c
typedef struct {
    uint32_t magic;         // Magic Bytes: 0x564B4548 ("HEKV" im Speicher, "KEHV" ASCII)
    uint32_t version;       // Dateiformat-Version: 2
    uint32_t entry_count;   // Anzahl der enthaltenen Einträge
    uint8_t salt[16];       // Kryptographischer Salt für PBKDF2 (16 Bytes)
    uint8_t nonce[12];      // Einmalige ChaCha20 Nonce (12 Bytes)
    uint8_t auth_tag[32];   // HMAC-SHA256 Authentifizierungs-Tag (32 Bytes)
} EncryptedVaultHeader;
```

#### Sicherheitskonzept (Encrypt-then-MAC):
1. Aus dem Master-Passwort und dem 16-Byte Salt werden per PBKDF2 (100.000 Runden) 64 Schlüsselbytes abgeleitet:
   - Die ersten 32 Bytes dienen als **Encryption Key** für ChaCha20.
   - Die zweiten 32 Bytes dienen als **Authentication Key** für HMAC-SHA256.
2. Die Payload (`sizeof(Entry) * count`) wird mit ChaCha20 verschlüsselt.
3. Über das Chiffrat wird ein HMAC-SHA256 Tag berechnet.
4. Beim Laden wird **zuerst** der HMAC-Tag in konstanter Zeit geprüft (`crypto_constant_time_cmp`). Schlägt die Prüfung fehl (z. B. falsches Passwort oder Manipulation der Datei), wird der Ladevorgang sofort abgebrochen, ohne dass potenziell korrupte Daten entschlüsselt oder verarbeitet werden.

#### Atomares Speichern:
```c
int vault_save_encrypted(const char* filename, const EntryList* list, const char* master_password) {
    ...
    snprintf(temp_filename, sizeof(temp_filename), "%s.tmp", filename);
    FILE* file = fopen(temp_filename, "wb");
    // Schreiben & flushen...
    fflush(file);
    fclose(file);
    remove(filename);
    rename(temp_filename, filename);
}
```
- Schreibt die Daten zunächst in eine temporäre Datei `.tmp` und benennt diese erst nach erfolgreichem `fflush()` in den Zieldateinamen um (`rename`).
- Verhindert Datenverlust bei Stromausfall oder Programmabsturz während des Schreibvorgangs.

---

### `backup.h` & `backup.c`
- **Pfad:** `backup.h`, `backup.c`
- **Aufgabe:** Automatische Erstellung zeitgestempelter Snapshot-Sicherungen (z. B. `vault_20260917_221500.bak`) vor jedem Speichervorgang sowie automatisches Bereinigen (Pruning) alter Backups bei Überschreitung des Limits (`MAX_BACKUPS 10`).

---

### `profile.h` & `profile.c`
- **Pfad:** `profile.h`, `profile.c`
- **Aufgabe:** Multi-Vault-Verwaltung. Ermöglicht das Anlegen, Erkennen und nahtlose Umschalten zwischen verschiedenen Tresor-Dateien (z. B. `vault.dat` für Standard, `vault_work.dat` für Arbeit, `vault_finance.dat` für Finanzen) zur Laufzeit.
- Durchsucht das Verzeichnis plattformübergreifend mittels `FindFirstFileA` (Windows) bzw. `opendir`/`readdir` (POSIX).

---

## 7. Audit, Suche, Versionierung und Datenaustausch

### `audit.h` & `audit.c`
- **Pfad:** `audit.h`, `audit.c`
- **Aufgabe:** Sicherheitsanalyse des Tresors:
  - Zählt schwache Passwörter (Score < 3).
  - Erkennt wiederverwendete/doppelte Passw��rter über quadratischen Vergleich ($O(N^2)$).
  - Berechnet einen Gesamt-Health-Score (0–100%).

---

### `expiry.h` & `expiry.c`
- **Pfad:** `expiry.h`, `expiry.c`
- **Aufgabe:** Altersüberwachung von Zugangsdaten. Berechnet das Alter in Tagen anhand von Unix-Timestamps und warnt vor Passwörtern, die Richtlinien von 30, 60, 90, 180 oder 365 Tagen überschreiten.

---

### `history.h` & `history.c`
- **Pfad:** `history.h`, `history.c`
- **Aufgabe:** Revisionsverwaltung und Papierkorb (Trash Bin):
  - Speichert historische Versionen eines Eintrags bei Änderungen mit Zeitstempel.
  - Soft-Delete: Gelöschte Einträge landen im Papierkorb und können wiederhergestellt oder endgültig bereinigt werden.

---

### `search.h` & `search.c`
- **Pfad:** `search.h`, `search.c`
- **Aufgabe:** Fehlertolerante Fuzzy-Suche basierend auf der **Levenshtein-Distanz** (Dynamic Programming).
- Berechnet Ähnlichkeitsscores für Titel und Benutzernamen und sortiert die Suchergebnisse nach Relevanz.

---

### `impex.h` & `impex.c`
- **Pfad:** `impex.h`, `impex.c`
- **Aufgabe:** Daten-Import und -Export in Standardformaten:
  - **CSV:** RFC 4180 konformes Handling von Kommas, Anführungszeichen und Zeilenumbrüchen.
  - **JSON:** Strukturierte Serialisierung und Deserialisierung von JSON-Arrays.

---

### `clipboard.h` & `clipboard.c`
- **Pfad:** `clipboard.h`, `clipboard.c`
- **Aufgabe:** Sicheres Kopieren von Passwörtern in die System-Zwischenablage.
- **Windows:** Verwendet die Win32 APIs `OpenClipboard()`, `EmptyClipboard()`, `GlobalAlloc(GMEM_MOVEABLE, ...)`, `GlobalLock()` und `SetClipboardData(CF_TEXT, ...)`.
- **POSIX:** Öffnet Pipes (`popen`) zu System-Tools wie `wl-copy`, `xclip`, `xsel` oder `pbcopy` (macOS).

---

## 8. CLI Benutzeroberfläche und Einstiegspunkt

### `main.cpp`
- **Pfad:** `main.cpp`
- **Aufgabe:** Zentraler Einstiegspunkt (`main()`) und interaktive textbasierte Benutzeroberfläche (CLI).
- Implementiert:
  - Maskierte Passworteingabe auf der Konsole (unter Windows via `_getch()`, unter POSIX via `termios` ohne Echo).
  - Authentifizierungs-Prompt beim Start und Speichern beim Beenden.
  - Menügesteuerte Navigation für alle 20 Funktionen des Tresors.

---

## 9. Automatisierte Testsuite

### `tests/test_vault.cpp`
- **Pfad:** `tests/test_vault.cpp`
- **Aufgabe:** Umfassende Testsuite (15 Testmodule, über 100 Assertions), die ohne externe Testframeworks auskommt und alle Core-Module auf Herz und Nieren prüft:
  1. Entry CRUD & dynamisches Resizing
  2. Kryptographische Primitiven (SHA-256, HMAC, PBKDF2, ChaCha20, Constant-Time Compare)
  3. Dateipersistenz & Verschlüsselung
  4. Passwort-Generierung & Diceware
  5. Clipboard-Integration
  6. Sicherheits-Audit
  7. CSV & JSON Import/Export
  8. Profile & Multi-Vault
  9. Kategorien & Tag-Parsing
  10. TOTP RFC 6238 Generator
  11. Historie & Papierkorb
  12. Passwort-Ablauf & Alter
  13. Backup-Snapshots & Pruning
  14. Fuzzy Search
  15. Sicheres Speicherlöschen (`secure_mem`)

---

## 10. Dokumentationsdateien

### `README.md`
- **Pfad:** `README.md`
- **Aufgabe:** Projektübersicht, Bildungshinweise, Feature-Auflistung, Build-Befehle und Sicherheits-Roadmap für externe Entwickler und GitHub-Besucher.

### `CODEBASE_DOCUMENTATION.md`
- **Pfad:** `CODEBASE_DOCUMENTATION.md`
- **Aufgabe:** Vollständige englischsprachige Dokumentation aller 20 Architektur-Schritte der `kehl-vault` Roadmap.
