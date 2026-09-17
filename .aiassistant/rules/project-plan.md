---
apply: always
---

# Project Plan

## Project goal

This is my personal C++ learning project.

The project should be developed step by step as a real, usable application while simultaneously teaching me the fundamentals of C++.

The project plan is the roadmap. Do not skip planned learning steps just to reach a finished application faster.

## Development principles

* Follow the project plan in order whenever possible.
* Keep the implementation appropriate for my current skill level.
* Introduce new concepts only when they are needed for the current project step.
* Do not over-engineer the application.
* Prefer simple, understandable C++ over advanced solutions.
* Do not introduce frameworks or external libraries unless the project plan explicitly requires them.
* Do not implement future project steps prematurely.
* Keep the existing project structure stable unless a planned project step requires a change.

## Learning approach

Each project step should teach one or more concrete C++ concepts.

When helping me with a step:

1. Explain what we are trying to achieve.
2. Explain the C++ concepts needed for it.
3. Let me implement as much as reasonably possible myself.
4. Give hints before giving the complete solution.
5. Only provide the complete implementation when necessary or explicitly requested.
6. After implementation, explain the important parts of the resulting code.

## Project roadmap
---
# Kehl Vault

## 1. Projektübersicht

**Projektname:** Kehl Vault **Executable:** `kehl-vault` **Repository:** `kehl-vault` **Primäre Sprachen:** C + C++ **C-Standard:** C17 **C++-Standard:** C++20 **Zielplattformen:** Linux, Windows, macOS **GUI:** SDL3 + Dear ImGui **Text-/Font-Rendering:** SDL3_ttf **Kryptografie:** libsodium **Buildsystem:** CMake **Tests:** Catch2 + CTest **Codeanalyse:** Clang-Tidy + Cppcheck **Debugging:** GDB / LLDB / Debugger der jeweiligen IDE **Memory Safety:** AddressSanitizer + UndefinedBehaviorSanitizer + Valgrind **Speicherformat:** eigenes versioniertes verschlüsseltes Vault-Format


---

# 2. Grundidee

Kehl Vault ist ein Lernprojekt, das mit einfachen C-Konzepten beginnt und sich schrittweise zu einer ernsthaften C/C++-Desktopanwendung entwickelt.

Der wichtige Punkt:

**Nicht sofort alles in C++ schreiben.**

Stattdessen:

```text
C lernen
    ↓
C-Probleme verstehen
    ↓
C-Modul entwickeln
    ↓
C++ kennenlernen
    ↓
C und C++ miteinander verbinden
    ↓
moderne C++-Architektur
    ↓
GUI
    ↓
Security
    ↓
Cross Platform
    ↓
später Robotics
```


---

# 3. Warum C + C++?

## C

C vermittelt besonders gut:

* Speicher
* Pointer
* Arrays
* Strings
* Structs
* ABI
* Dateien
* Systemnähe
* manuelle Ressourcenverwaltung
* Datenrepräsentation

Diese Dinge sind für spätere C++-Programmierung extrem hilfreich.


---

## C++

C++ erweitert das Ganze um:

* Klassen
* Objekte
* Konstruktoren
* Destruktoren
* RAII
* References
* Namespaces
* Templates
* STL
* `std::string`
* `std::vector`
* `std::unordered_map`
* `std::optional`
* `std::variant`
* Smart Pointer
* Exceptions
* Lambdas
* Algorithmen
* Concepts
* moderne API-Architektur


---

# 4. Das eigentliche Lernziel

Kehl Vault soll nicht nur "ein Passwortmanager" werden.

Das Projekt soll dir am Ende zeigen:

```text
Wie baut man eine größere C/C++-Anwendung?
```

Du lernst dabei:

```text
C
├── Memory
├── Pointer
├── Structs
├── Files
├── APIs
└── System Programming

C++
├── Classes
├── RAII
├── STL
├── Templates
├── Modern APIs
└── Architecture

Software Engineering
├── CMake
├── Git
├── Testing
├── Debugging
└── CI

Security
├── KDF
├── Encryption
├── Authentication
└── Secret Management

GUI
├── SDL3
└── Dear ImGui

Cross Platform
├── Linux
├── Windows
└── macOS
```


---

# 5. Hauptziel des Projekts

Kehl Vault soll einen lokalen verschlüsselten Passwort-Tresor bereitstellen.

Das Programm soll:

* einen Tresor erstellen
* einen Tresor öffnen
* einen Tresor sperren
* Entries erstellen
* Entries bearbeiten
* Entries löschen
* Entries suchen
* Passwörter generieren
* Passwörter kopieren
* Passwörter anzeigen/verstecken
* Auto-Lock unterstützen
* Einstellungen speichern
* eine grafische Oberfläche besitzen
* optional über die CLI bedienbar sein
* auf Linux, Windows und macOS laufen


---

# 6. Projektgrenze

Kehl Vault ist zunächst ein **Lern- und Entwicklungsprojekt**.

Solange keine vollständige Sicherheitsprüfung und kein Security Review erfolgt ist:

* keine wichtigen produktiven Passwörter darin speichern
* keine Sicherheitsgarantien behaupten
* keine eigene Kryptografie implementieren
* keine eigenen Verschlüsselungsalgorithmen schreiben
* keine Geheimnisse loggen

Der Fokus liegt auf:

**Lernen durch ein echtes Projekt.**


---

# 7. Architektur

Die Architektur wird bewusst in Schichten getrennt.

```text
                         Kehl Vault
                              │
              ┌───────────────┴────────────────┐
              │                                │
             GUI                              CLI
              │                                │
              └───────────────┬────────────────┘
                              │
                         Application
                              │
                         Core API
                              │
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
      Vault                 Search              Generator
        │
    ┌───┴───────────────┐
    │                   │
 Storage              Crypto
    │                   │
    │                libsodium
    │
    └───────────────┬───────────────┐
                    │               │
              Platform API      File Format
                    │
          ┌─────────┼─────────┐
          │         │         │
        Linux    Windows    macOS
```


---

# 8. Spracheinsatz

Ein zentraler Bestandteil des Projekts ist die bewusste Aufteilung zwischen C und C++.

## C

C wird zunächst für fundamentale Module verwendet.

Beispielsweise:

* Datenstrukturen
* Entry-Datenmodell
* Serialization
* Storage
* Teile des Crypto-Interfaces
* Password Generator
* C-kompatible APIs


---

## C++

C++ übernimmt später komplexere Anwendungsschichten.

Beispielsweise:

* Application Layer
* GUI
* Controller
* View Model
* Settings Management
* moderne Container
* RAII
* Resource Management
* CLI
* komplexere Datenstrukturen


---

# 9. Hybridarchitektur

Ein mögliches Modell:

```text
kehl-core
    │
    ├── C API
    │
    └── C17
         │
         ▼
    C++ Application
         │
         ├── C++20
         ├── STL
         ├── RAII
         └── GUI
              │
              ├── SDL3
              └── Dear ImGui
```

Damit lernst du auch, wie C-Code aus C++ heraus verwendet wird.


---

# 10. C API ↔ C++

Eine wichtige Lernstation wird sein:

```text
C library
    ↓
C header
    ↓
C++
```

C++ kann C-Funktionen direkt benutzen.

Dabei ist es wichtig, die Unterschiede zwischen:

* C ABI
* C++ ABI
* Headern
* Linkage
* `extern "C"`

zu verstehen.

Das ist später für Hardware- und Systembibliotheken sehr nützlich.


---

# 11. Warum Dear ImGui?

Mit C++ wird Dear ImGui deutlich attraktiver.

Es bietet:

* Buttons
* Input Fields
* Lists
* Menus
* Tables
* Tabs
* Dialoge
* Tooltips
* Layout-System
* GUI-State
* Debug UI

Dear ImGui besitzt offizielle SDL3-Unterstützung und Renderer-Backends für mehrere Grafik-APIs.


---

# 12. SDL3

SDL3 übernimmt:

* Fenster
* Input
* Mouse
* Keyboard
* Events
* Rendering-Grundlagen
* Clipboard
* Plattformabstraktion
* verschiedene Systemfunktionen

Dadurch bleibt die Anwendung grundsätzlich cross-platform.


---

# 13. SDL3 + Dear ImGui

Die Rollen bleiben getrennt.

```text
SDL3
 ↓
Window / Input / OS Integration
 ↓
Dear ImGui
 ↓
Widgets / Layout / GUI
 ↓
Kehl Application
 ↓
Kehl Core
```

Dear ImGui ist damit nicht selbst die komplette Plattformabstraktion.


---

# 14. SDL3_ttf

SDL3_ttf wird für Fonts und Text verwendet.

Ziel:

* TrueType Fonts
* skalierbarer Text
* UTF-8
* UI-Schrift
* Passwortsterne
* Labels
* Fehlermeldungen


---

# 15. Bibliotheksstack

| Library | Sprache | Zweck | Priorität |
|---------|---------|-------|-----------|
| SDL3    | C       | Window/Input/Clipboard/Platform | Pflicht   |
| Dear ImGui | C++     | GUI   | Pflicht   |
| SDL3_ttf | C       | Fonts/Text | Pflicht   |
| libsodium | C       | Kryptografie/KDF/Random | Pflicht   |
| CMake   | C/C++   | Buildsystem | Pflicht   |
| Catch2  | C++     | Unit Tests | Empfohlen |
| CTest   | CMake   | Test Runner | Empfohlen |
| Clang-Tidy | C/C++   | Static Analysis | Empfohlen |
| Cppcheck | C/C++   | Static Analysis | Empfohlen |
| Eigen   | C++     | Linear Algebra/Robotics | Später    |
| fmt     | C++     | modernes Formatting | Optional  |
| spdlog  | C++     | Logging | Optional  |
| SQLite  | C       | Datenbank | Später    |


---

# 16. libsodium

libsodium bleibt die zentrale Crypto-Library.

Sie liefert APIs für:

* password-based key derivation
* Argon2id
* secure random
* authenticated encryption
* secretstream
* secure memory

Die offizielle Dokumentation empfiehlt `crypto_pwhash()` zur Ableitung eines kryptografischen Schlüssels aus einem Passwort. Für dateibasierte Verschlüsselung beschreibt libsodium außerdem `crypto_secretstream`, das Authentifizierung, Nonce-Handling und Erkennung von Manipulation bzw. Trunkierung unterstützt.


---

# 17. C++ und libsodium

libsodium selbst ist eine C-Bibliothek.

Das ist für dieses Projekt ideal.

Denn du lernst:

```text
C Library
     ↓
C Header
     ↓
C++ Application
```

Damit hast du später genau das Grundverständnis, das auch bei vielen Hardware-, Embedded- und Systembibliotheken gebraucht wird.


---

# 18. Was niemals selbst implementiert wird

Nicht selbst schreiben:

* AES
* ChaCha20
* Argon2
* SHA
* HMAC
* MAC
* kryptografischer RNG
* eigenes Password Hashing
* eigene AEAD-Verfahren

Stattdessen:

**libsodium verwenden.**


---

# 19. Passwortableitung

Grundmodell:

```text
Master Password
       ↓
     Argon2id
       ↓
Derived Key
       ↓
Encryption
       ↓
Encrypted Vault
```

Das Passwort selbst wird nicht als Encryption Key verwendet.


---

# 20. Passwortprüfung

Ein falsches Master Password muss dazu führen, dass die Entschlüsselung bzw. Authentifizierung des Vaults fehlschlägt.

Dadurch braucht der Vault keine unnötige unverschlüsselte Passwortkopie.


---

# 21. Vault-Dateiformat

Das Format soll versioniert sein.

Beispiel:

```text
┌────────────────────────────────────┐
│ Kehl Vault Header                  │
├────────────────────────────────────┤
│ Magic                              │
│ Format Version                     │
│ KDF Version                        │
│ KDF Parameters                     │
│ Salt                               │
│ Crypto Parameters                  │
├────────────────────────────────────┤
│                                    │
│     Encrypted Payload              │
│                                    │
│ Entries                            │
│ Usernames                          │
│ Passwords                          │
│ URLs                               │
│ Notes                              │
│ Tags                               │
│                                    │
└────────────────────────────────────┘
```


---

# 22. Versionierung

Der Header muss erkennen lassen:

```text
Ist das eine Kehl Vault Datei?
```

und:

```text
Welche Formatversion ist das?
```

Beispiel:

```text
v1
v2
v3
```

Später können Migrationen implementiert werden.


---

# 23. Migration

Langfristig:

```text
Vault v1
   ↓
Parser
   ↓
Migration
   ↓
Vault v2
```

Damit wird das Projekt auch ein gutes Beispiel für Software Evolution.


---

# 24. Entry-Datenmodell

Ein Entry besitzt zunächst:

* ID
* Titel
* Username
* Password
* URL
* Notes
* Tags
* optional Favorite
* optional Created At
* optional Modified At


---

# 25. Spätere Entry-Felder

Erst später:

* TOTP Secret
* Password History
* Custom Fields
* Attachments
* Kategorien
* Favorite
* weitere Metadaten

Nicht alles gleichzeitig implementieren.


---

# 26. Phase 0 – Projektvorbereitung

## Ziel

Ein minimales C/C++-Projekt aufbauen.

### Checkliste

- [ ] Git Repository erstellen
- [ ] Projekt `kehl-vault`
- [ ] README
- [ ] LICENSE
- [ ] `.gitignore`
- [ ] CMake
- [ ] C17 konfigurieren
- [ ] C++20 konfigurieren
- [ ] `src/`
- [ ] `include/`
- [ ] `tests/`
- [ ] `assets/`
- [ ] erste C-Datei
- [ ] erste C++-Datei
- [ ] erstes Binary bauen

CMake ist ausdrücklich für moderne C- und C++-Buildsysteme gedacht und unterstützt auch Test- und Packaging-Workflows.


---

# 27. Phase 1 – C Grundlagen

## Lernen

* Variablen
* Datentypen
* Operatoren
* Bedingungen
* Schleifen
* Funktionen
* Arrays
* Strings
* `printf`
* Rückgabewerte
* `enum`
* `struct`


---

# 28. C Mini-Projekte

Vor dem eigentlichen Vault:

## Mini-Projekt 1

Passwortlänge prüfen.

## Mini-Projekt 2

Passwortzeichen analysieren.

## Mini-Projekt 3

Passwortstärke ermitteln.

## Mini-Projekt 4

Password Generator.

## Mini-Projekt 5

Entry-Liste im RAM.


---

# 29. Phase 2 – Structs

Jetzt entsteht das Datenmodell.

Lernen:

* `struct`
* `typedef`
* Arrays von Structs
* Pointer auf Structs
* Funktionen mit Structs

### Checkliste

- [ ] Entry definieren
- [ ] Vault definieren
- [ ] Entry erstellen
- [ ] Entry löschen
- [ ] Entry bearbeiten
- [ ] Entries anzeigen
- [ ] Entry Count verwalten


---

# 30. Phase 3 – Pointer

Jetzt bewusst intensiv mit Pointer arbeiten.

Lernen:

* Adresse
* Dereferenzierung
* Pointer auf Pointer
* Pointer + Arrays
* Struct Pointer
* Pointer als Funktionsparameter

Ziel:

Du sollst später wissen, warum C++ References und Smart Pointer existieren.


---

# 31. Phase 4 – Dynamischer Speicher

Lernen:

* `malloc`
* `calloc`
* `realloc`
* `free`
* Speicherbesitz
* Lebensdauer
* NULL-Prüfung

### Checkliste

- [ ] dynamische Entry-Liste
- [ ] Reallocation
- [ ] Entry entfernen
- [ ] Speicher freigeben
- [ ] Allocation Failure behandeln
- [ ] keine Leaks


---

# 32. Phase 5 – C String Handling

Besonders wichtig für Security.

Lernen:

* Nullterminierung
* Buffer Size
* `strlen`
* `strcmp`
* `strncmp`
* `memcpy`
* `memmove`
* `memset`
* Bounds Checking

Dabei verstehen:

**Warum sind C-Strings fehleranfällig?**


---

# 33. Phase 6 – Password Generator

Der erste größere Core-Baustein.

## Einstellungen

* Länge
* Kleinbuchstaben
* Großbuchstaben
* Zahlen
* Sonderzeichen
* ähnliche Zeichen vermeiden
* bestimmte Zeichen ausschließen
* Mindestanzahl je Zeichensatz
* Passphrase-Modus


---

# 34. Zufälligkeit

Für sicherheitsrelevante Passwörter:

**kein** `**rand()**`

Stattdessen libsodium Random API.

Damit erhältst du kryptografisch geeignete Zufallsdaten und kannst beispielsweise `randombytes_uniform()` für gleichmäßige Auswahl verwenden.


---

# 35. Phase 7 – File I/O

Lernen:

* `fopen`
* `fclose`
* `fread`
* `fwrite`
* `fseek`
* `ftell`

Ziel:

```text
Vault
 ↓
Serialize
 ↓
File

File
 ↓
Deserialize
 ↓
Vault
```


---

# 36. Phase 8 – Serialization

Jetzt wird gelernt, wie Daten in eine Datei übertragen werden.

Themen:

* Binärdaten
* Feldgrößen
* Endianness
* Integer Sizes
* Längen
* Bounds Checking
* Formatversionen


---

# 37. Phase 9 – Eigenes Vault Format

Jetzt wird `Kehl Vault Format v1` entworfen.

## Header

* Magic
* Version
* KDF Version
* Salt
* Crypto Metadata
* Payload Length

## Payload

* Vault Metadata
* Entries
* Tags
* Settings


---

# 38. Phase 10 – C++ Einstieg

Jetzt beginnt bewusst C++.

Nicht direkt alles umschreiben.

## Erste C++ Themen

* `namespace`
* `std::string`
* `std::vector`
* `std::array`
* References
* `const`
* Klassen
* Konstruktoren
* Destruktoren
* Methoden


---

# 39. C++ statt C

Jetzt wird verglichen.

### C

```text
struct
malloc
free
char*
```

### C++

```text
class
std::vector
std::string
RAII
```

Das Ziel ist nicht:

**C schlecht, C++ gut.**

Sondern:

**Wann ist welches Werkzeug sinnvoll?**


---

# 40. Phase 11 – RAII

Sehr wichtig für später.

Lernen:

**Resource Acquisition Is Initialization**

Anstatt:

```text
open
...
remember close
```

wird eine Ressource an die Lebensdauer eines Objektes gebunden.

Das gilt später für:

* Dateien
* Mutexes
* Sockets
* Threads
* Speicher
* SDL Ressourcen
* GUI Ressourcen


---

# 41. Phase 12 – Smart Pointer

Lernen:

* `std::unique_ptr`
* `std::shared_ptr`
* `std::weak_ptr`

Aber:

**Nicht alles mit** `**shared_ptr**` **bauen.**

Ziel ist zu verstehen:

* Ownership
* Lifetime
* eindeutiger Besitzer
* Shared Ownership


---

# 42. Phase 13 – STL

Kehl Vault wird jetzt schrittweise moderner.

Wichtige Container:

* `std::vector`
* `std::string`
* `std::unordered_map`
* `std::map`
* `std::array`
* `std::optional`
* `std::variant`


---

# 43. C++ Datenmodell

Ein möglicher späterer Aufbau:

```text
Vault
 ├── Entry
 ├── Entry
 ├── Entry
 └── Entry
```

In C++ kann daraus eine stärker gekapselte API werden.

Beispielsweise konzeptionell:

```text
Vault
 ├── addEntry()
 ├── removeEntry()
 ├── updateEntry()
 ├── findEntry()
 └── lock()
```

Noch kein Code notwendig.


---

# 44. C Core + C++ Layer

Eine besonders interessante Architektur:

```text
┌─────────────────────────────┐
│ C++ Application             │
│                             │
│ Classes                     │
│ RAII                        │
│ STL                         │
│ GUI                         │
└──────────────┬──────────────┘
               │
          C-compatible API
               │
┌──────────────▼──────────────┐
│ C Core                      │
│                             │
│ Storage                     │
│ Serialization               │
│ Low-level Structures        │
│ Crypto Interface            │
└─────────────────────────────┘
```

Damit übst du eine realistische Grenze zwischen C und C++.


---

# 45. Phase 14 – C++ Application Layer

Jetzt entsteht:

```text
Application
 ├── VaultController
 ├── SearchController
 ├── GeneratorController
 ├── SettingsController
 └── ClipboardController
```

Diese Schicht koordiniert Module.

Sie enthält nicht die primitive Kryptografie selbst.


---

# 46. Phase 15 – GUI

Jetzt kommt:

**SDL3 + Dear ImGui**


---

# 47. GUI-Architektur

```text
SDL3
   ↓
Window / Input / Clipboard
   ↓
Dear ImGui
   ↓
Views
   ↓
Application Layer
   ↓
Core
```


---

# 48. Erste GUI

```text
┌─────────────────────────────────────────────┐
│ Kehl Vault                           ⚙  🔒 │
├─────────────────────────────────────────────┤
│ Search: [_______________________________]   │
├──────────────────┬──────────────────────────┤
│ Entries          │ Entry                    │
│                  │                          │
│ GitHub           │ Username                │
│ Discord          │ mimi@example.com         │
│ School           │                          │
│ Proxmox          │ Password                │
│ Nextcloud        │ •••••••••••••••••       │
│                  │                          │
│ [+ New Entry]    │ [Show] [Copy] [Generate]│
│                  │                          │
└──────────────────┴──────────────────────────┘
```


---

# 49. Login Screen

```text
┌────────────────────────────────┐
│                                │
│          Kehl Vault            │
│                                │
│        Vault locked            │
│                                │
│ Master Password                │
│ [••••••••••••••••••••]         │
│                                │
│            [Unlock]            │
│                                │
└────────────────────────────────┘
```


---

# 50. Entry Editor

Felder:

* Title
* Username
* Password
* URL
* Tags
* Notes

Buttons:

* Save
* Cancel
* Generate


---

# 51. Password Generator GUI

```text
Password Generator

Length
[ 24 ]

☑ Lowercase
☑ Uppercase
☑ Numbers
☑ Symbols

☐ Avoid similar characters

Generated Password
[ ••••••••••••••••••••• ]

Strength
████████████

[ Regenerate ]
[ Use Password ]
```


---

# 52. Phase 16 – Search

Suchfelder:

* Title
* Username
* URL
* Tags

Später:

* Notes
* Fuzzy Search
* Filter
* Sortierung
* Favorites
* Recently Used


---

# 53. Search API

Die Suche darf nicht in ImGui geschrieben werden.

Richtig:

```text
GUI
 ↓
Application
 ↓
Search API
 ↓
Core
 ↓
Results
 ↓
GUI
```

Dann funktioniert dieselbe Logik später über die CLI.


---

# 54. Phase 17 – Master Password

Jetzt wird der Vault wirklich geschützt.

Modell:

```text
Master Password
       ↓
      Salt
       ↓
    Argon2id
       ↓
  Derived Key
       ↓
Authenticated Encryption
       ↓
Encrypted Vault
```

Salt und erforderliche Parameter werden zusammen mit der verschlüsselten Datei gespeichert; das Geheimnis bleibt das Passwort bzw. der daraus abgeleitete Schlüssel. libsodium dokumentiert genau dieses Muster für passwortbasierte Dateiverschlüsselung.


---

# 55. Phase 18 – Authenticated Encryption

Der Vault soll nicht nur geheim, sondern auch manipulationsgeschützt sein.

```text
Vault
 ↓
Verschlüsseln
 ↓
Authentication Tag
 ↓
Datei
```

Bei Manipulation:

```text
Datei verändert
 ↓
Authentication Failure
 ↓
Vault ablehnen
```


---

# 56. Secretstream

Für das Vault-Format ist `crypto_secretstream` eine interessante Option, weil libsodium damit verschlüsselte Datenströme inklusive Authentifizierung und Manipulationserkennung bereitstellt. Die API erzeugt und verwaltet die benötigten Nonce-/Stream-Informationen selbst.

Die konkrete Formatentscheidung wird erst getroffen, nachdem das Datenformat und die Teststrategie stehen.


---

# 57. Phase 19 – Secure Memory

Sensible Daten:

* Master Password
* Encryption Key
* Entry Passwords
* TOTP Secrets
* entschlüsselte Vault-Daten

sollen möglichst kurz leben.

Lernen:

* sichere Speicherbereinigung
* Memory Lifetime
* Secret Exposure
* Lock/Unlock State
* Copy Lifetime


---

# 58. Clipboard

Funktionen:

* Copy Password
* Copy Username
* Copy URL

SDL3/der SDL3-Stack bringt bereits Clipboard-Unterstützung mit, und der offizielle Dear-ImGui-SDL3-Backend unterstützt Clipboard ebenfalls. Daher wird zunächst keine zusätzliche Clipboard-Library benötigt.


---

# 59. Clipboard Timeout

Beispiel:

```text
Copy
 ↓
Clipboard
 ↓
30 Sekunden
 ↓
Clear
```

Einstellbar:

* 15 Sekunden
* 30 Sekunden
* 60 Sekunden
* deaktiviert

Wichtig:

Das Entfernen aus dem System-Clipboard garantiert nicht, dass Clipboard-Manager oder andere Prozesse frühere Kopien bereits vergessen haben.


---

# 60. Phase 20 – Auto-Lock

Mögliche Einstellungen:

* 1 Minute
* 5 Minuten
* 10 Minuten
* 30 Minuten
* deaktiviert

Ablauf:

```text
Vault unlocked
      ↓
Inaktivität
      ↓
Auto-Lock
      ↓
Secret Cleanup
      ↓
Locked GUI
```


---

# 61. Phase 21 – Platform Layer

Plattformabhängige Dinge kommen in eine eigene Schicht.

Beispielsweise:

* App Data Path
* Config Path
* Clipboard
* OS Information
* System Notifications
* native APIs


---

# 62. Linux

Beispiele:

```text
Linux implementation
```

## Ziel

Kein Linux-spezifischer Code im Core.


---

# 63. Windows

```text
Windows implementation
```

Auch hier:

Der Core weiß nicht, dass Windows existiert.


---

# 64. macOS

```text
macOS implementation
```


---

# 65. Application Paths

Keine Hardcodes wie:

```text
/home/mimi/...
C:\Users\...
/Users/...
```

Stattdessen Plattformabstraktion.

SDL3 stellt dafür unter anderem `SDL_GetPrefPath()` bereit, sodass Anwendungsdaten plattformabhängig an einem geeigneten Benutzerpfad abgelegt werden können.


---

# 66. Phase 22 – CLI

Später:

```text
kehl-vault
kehl-vault add
kehl-vault get
kehl-vault search
kehl-vault generate
kehl-vault lock
```

CLI und GUI verwenden dieselbe Application-/Core-Schicht.


---

# 67. Warum CLI?

Weil du damit testen kannst, ob die Architektur wirklich unabhängig von der GUI ist.

```text
        GUI
         │
         ▼
      Core/API
         ▲
         │
        CLI
```

Wenn beide dieselben Funktionen verwenden:

**saubere Architektur.**


---

# 68. Phase 23 – Settings

## Allgemein

* Theme
* Language
* UI Scale
* Auto-Lock

## Password Generator

* Default Length
* Character Sets
* Passphrase

## Clipboard

* Timeout
* Copy Options


---

# 69. Theme-System

Theme nicht überall hardcoden.

Stattdessen:

```text
Theme
 ├── Background
 ├── Surface
 ├── Text
 ├── Accent
 ├── Warning
 ├── Error
 ├── Border
 └── Selection
```


---

# 70. Spätere Themes

* Dark
* Light
* System
* High Contrast
* Kehl Theme


---

# 71. Phase 24 – Modern C++

Jetzt werden weitere C++-Features eingeführt.

## Lernen

* `auto`
* range-based for
* lambdas
* `constexpr`
* `consteval` später
* move semantics
* `std::move`
* `std::optional`
* `std::variant`
* `std::string_view`
* `std::span`

Nicht alles gleichzeitig.


---

# 72. Templates

Templates zunächst anhand kleiner Dinge lernen:

* generische Hilfsfunktionen
* Container
* Algorithmen
* Traits später

Danach verstehen:

**Warum ist die STL so mächtig?**


---

# 73. Exceptions

Kehl Vault soll bewusst unterschiedliche Fehlerstrategien untersuchen.

Beispielsweise:

```text
C Layer
→ Error Codes
```

und:

```text
C++ Layer
→ Exceptions / std::expected-ähnliche Designs
```

Nicht pauschal eines davon als "richtig" festlegen.


---

# 74. Moderne Error Handling Architektur

Später kann untersucht werden:

```text
C
 ↓
Error Code
 ↓
C++ Wrapper
 ↓
typed result
 ↓
GUI error message
```

Das ist ein gutes fortgeschrittenes C/C++-Lernziel.


---

# 75. Phase 25 – RAII für Systemressourcen

Beispiele:

* File Handle
* SDL Window
* SDL Renderer
* ImGui Context
* Memory
* Mutex
* Threads

Ziel:

```text
Objekt erzeugt
    ↓
Ressource gültig
    ↓
Objekt verlässt Scope
    ↓
automatisches Cleanup
```

Hier wird der Unterschied zwischen C und modernem C++ besonders greifbar.


---

# 76. Projektstruktur

Eine Hybridstruktur könnte so aussehen:

```text
kehl-vault/
│
├── CMakeLists.txt
├── README.md
├── LICENSE
├── .gitignore
│
├── include/
│   └── kehl/
│       ├── c/
│       │   ├── vault.h
│       │   ├── entry.h
│       │   ├── storage.h
│       │   ├── crypto.h
│       │   └── password.h
│       │
│       └── cpp/
│           ├── Application.hpp
│           ├── Vault.hpp
│           ├── Search.hpp
│           ├── Settings.hpp
│           └── Clipboard.hpp
│
├── src/
│   ├── c/
│   │   ├── vault.c
│   │   ├── entry.c
│   │   ├── storage.c
│   │   ├── crypto.c
│   │   └── password.c
│   │
│   └── cpp/
│       ├── main.cpp
│       ├── Application.cpp
│       ├── Vault.cpp
│       ├── Search.cpp
│       ├── Settings.cpp
│       ├── Clipboard.cpp
│       │
│       ├── gui/
│       │   ├── Gui.cpp
│       │   ├── LoginView.cpp
│       │   ├── VaultView.cpp
│       │   ├── EntryView.cpp
│       │   ├── SettingsView.cpp
│       │   └── Theme.cpp
│       │
│       └── platform/
│           ├── Platform.cpp
│           ├── Filesystem.cpp
│           └── Clipboard.cpp
│
├── tests/
│   ├── c/
│   │   ├── test_entry.c
│   │   ├── test_storage.c
│   │   ├── test_crypto.c
│   │   └── test_password.c
│   │
│   └── cpp/
│       ├── test_vault.cpp
│       ├── test_search.cpp
│       └── test_application.cpp
│
└── assets/
    ├── fonts/
    └── icons/
```


---

# 77. Noch wichtiger: Modulgrenzen

## C

```text
vault.c
entry.c
storage.c
crypto.c
password.c
```

## C++

```text
Application
GUI
Settings
Search
Controllers
RAII
```

Das Projekt zeigt dadurch ganz konkret:

**Wo endet C und wo beginnt modernes C++?**


---

# 78. Phase 26 – Unit Tests

Empfohlener Stack:

* Catch2
* CTest

C-Funktionen können ebenfalls getestet werden.

Beispielsweise:

```text
C Test
 ↓
Core Function
 ↓
Result
```

und:

```text
C++ Test
 ↓
C++ Wrapper
 ↓
C Core
```


---

# 79. Entry Tests

- [ ] Entry erstellen
- [ ] Entry löschen
- [ ] Entry bearbeiten
- [ ] mehrere Entries
- [ ] leere Felder
- [ ] lange Strings
- [ ] ungültige Eingaben


---

# 80. Search Tests

- [ ] exact match
- [ ] partial match
- [ ] no match
- [ ] multiple matches
- [ ] case insensitive
- [ ] tags
- [ ] URL


---

# 81. Password Generator Tests

- [ ] Länge korrekt
- [ ] Zeichensätze korrekt
- [ ] Sonderzeichen
- [ ] Mindestanzahl
- [ ] Passphrase
- [ ] Grenzwerte


---

# 82. Storage Tests

- [ ] Save
- [ ] Load
- [ ] Save → Load identisch
- [ ] leere Vault
- [ ] große Vault
- [ ] beschädigte Datei
- [ ] falsche Version
- [ ] abgeschnittene Datei


---

# 83. Crypto Tests

- [ ] Encryption
- [ ] Decryption
- [ ] wrong password
- [ ] wrong key
- [ ] modified ciphertext
- [ ] invalid header
- [ ] invalid authentication
- [ ] corrupted vault


---

# 84. Phase 27 – Memory Safety

Werkzeuge:

* AddressSanitizer
* UndefinedBehaviorSanitizer
* LeakSanitizer
* Valgrind

Tests:

- [ ] Buffer Overflow
- [ ] Use-after-free
- [ ] Double-free
- [ ] Memory Leak
- [ ] Null Dereference
- [ ] Invalid Free
- [ ] Out-of-Bounds


---

# 85. Warum das für C++ wichtig ist

C++ reduziert durch RAII und STL viele typische Fehler.

Aber:

C++ macht diese Fehler nicht unmöglich.

Daher weiterhin testen:

* raw pointer
* references
* iterators
* lifetime
* ownership
* concurrency
* ABI


---

# 86. Phase 28 – Static Analysis

Werkzeuge:

* Clang-Tidy
* Cppcheck

Ziel:

* problematische Konstrukte erkennen
* API-Nutzung prüfen
* Warnungen finden
* mögliche Bugs früh erkennen


---

# 87. Compiler Warnings

## C

```text
-Wall
-Wextra
-Wpedantic
-Wconversion
-Wshadow
```

## C++

Zusätzlich sinnvoll:

```text
-Wnon-virtual-dtor
-Wold-style-cast
-Woverloaded-virtual
```

Warnungsniveau soll schrittweise verschärft werden.


---

# 88. Phase 29 – CMake professioneller verwenden

Ziel:

```text
CMake
 ├── kehl_core
 ├── kehl_application
 ├── kehl_gui
 ├── kehl_tests
 └── kehl_vault
```

CMake ermöglicht genau diese Art von Target-basierter Struktur und kann daraus plattformabhängige Buildsysteme erzeugen.


---

# 89. Debug Build

Enthält:

* Debug Symbols
* Sanitizers
* Tests
* Debug Logging


---

# 90. Release Build

Enthält:

* Optimierung
* kein Debug Logging
* Release Assets
* Packaging


---

# 91. Phase 30 – Git Workflow

Branches beispielsweise:

```text
main
develop

feature/c-core
feature/password-generator
feature/storage
feature/crypto
feature/cpp-layer
feature/gui
feature/search
feature/clipboard
```

Commit-Beispiele:

```text
feat: add vault data model
feat: add password generator
feat: add encrypted storage
feat: add C++ application layer
feat: add ImGui vault view
fix: reject malformed vault header
test: add storage roundtrip tests
refactor: isolate platform layer
```


---

# 92. Versionierung

```text
0.1.0
0.2.0
0.3.0
0.4.0
0.5.0
...
1.0.0
```


---

# 93. Version 0.1 – C Core

Enthält:

* Entry
* Vault
* Pointer
* Dynamic Memory
* Search
* Password Generator

### Checkliste

- [ ] Core funktioniert
- [ ] Memory korrekt
- [ ] Tests vorhanden
- [ ] keine bekannten Leaks


---

# 94. Version 0.2 – Storage

Enthält:

* File I/O
* Serialization
* Deserialization
* Vault Format
* Versioning

### Checkliste

- [ ] speichern
- [ ] laden
- [ ] Validierung
- [ ] beschädigte Datei erkennen


---

# 95. Version 0.3 – Crypto

Enthält:

* Master Password
* Argon2id
* Key Derivation
* Encryption
* Authentication

### Checkliste

- [ ] Vault verschlüsselt
- [ ] falsches Passwort erkannt
- [ ] Manipulation erkannt
- [ ] Crypto-Tests


---

# 96. Version 0.4 – C++ Layer

Jetzt beginnt die eigentliche C++-Anwendung.

Enthält:

* C++ Wrappers
* Classes
* RAII
* STL
* Application Layer

### Checkliste

- [ ] C Core wird aus C++ verwendet
- [ ] RAII eingesetzt
- [ ] `std::vector`
- [ ] `std::string`
- [ ] klare Ownership


---

# 97. Version 0.5 – GUI

Enthält:

* SDL3
* Dear ImGui
* Login
* Entry List
* Entry Editor
* Search
* Generator

### Checkliste

- [ ] Fenster
- [ ] Login
- [ ] Entry List
- [ ] Entry View
- [ ] Search
- [ ] Generator
- [ ] Theme


---

# 98. Version 0.6 – Desktop Features

- [ ] Clipboard
- [ ] Clipboard Timeout
- [ ] Auto-Lock
- [ ] Settings
- [ ] Themes
- [ ] Secret Cleanup


---

# 99. Version 0.7 – CLI

- [ ] Vault öffnen
- [ ] Search
- [ ] Get Entry
- [ ] Add Entry
- [ ] Generate Password
- [ ] Lock


---

# 100. Version 0.8 – Engineering

- [ ] Unit Tests
- [ ] CTest
- [ ] ASan
- [ ] UBSan
- [ ] Valgrind
- [ ] Cppcheck
- [ ] Clang-Tidy
- [ ] Fuzzing vorbereiten


---

# 101. Version 0.9 – Cross Platform

## Linux

- [ ] build
- [ ] run
- [ ] install
- [ ] filesystem
- [ ] clipboard
- [ ] GUI

## Windows

- [ ] build
- [ ] run
- [ ] install
- [ ] filesystem
- [ ] clipboard
- [ ] GUI

## macOS

- [ ] build
- [ ] run
- [ ] install
- [ ] filesystem
- [ ] clipboard
- [ ] GUI


---

# 102. Version 1.0

## Core

- [ ] Vault
- [ ] Entry
- [ ] Search
- [ ] Generator
- [ ] Storage
- [ ] Crypto

## Application

- [ ] C++
- [ ] RAII
- [ ] STL
- [ ] Application Layer

## GUI

- [ ] SDL3
- [ ] Dear ImGui
- [ ] Login
- [ ] Entry Editor
- [ ] Search
- [ ] Settings

## Security

- [ ] Argon2id
- [ ] authenticated encryption
- [ ] secure random
- [ ] Auto-Lock
- [ ] Clipboard Timeout
- [ ] Secret Cleanup

## Platforms

- [ ] Linux
- [ ] Windows
- [ ] macOS


---

# 103. Security UX

Standardmäßig:

* Passwort versteckt
* Lock Button sichtbar
* Clipboard Timeout
* Auto-Lock
* keine Secrets in Logs
* Warnung vor ungespeicherten Änderungen
* klare Fehlermeldungen


---

# 104. Logging

Erlaubt:

```text
Vault opened
Vault locked
Entry created
Storage error
GUI initialization failed
```

Nicht erlaubt:

```text
Master Password
Encryption Key
Password
TOTP Secret
Complete decrypted Entry
```


---

# 105. Keine globalen Secrets

Kein globales:

```text
master_password
encryption_key
current_password
```

Stattdessen:

* klarer Application State
* definierte Ownership
* definierte Lifetime
* kontrollierter Zugriff


---

# 106. Input Validation

Validieren:

* Master Password
* Title
* Username
* Password
* URL
* Notes
* Tags
* Generator Length
* CLI Arguments
* Dateipfade


---

# 107. Parser Security

Das Dateiformat muss defensiv verarbeitet werden.

Zum Beispiel:

```text
Datei
 ↓
Header
 ↓
Bounds Check
 ↓
Length Check
 ↓
Version Check
 ↓
Payload
```

Nie:

```text
Datei
 ↓
blind lesen
```


---

# 108. Fuzzing

Später können automatisiert zufällige Dateien erzeugt werden.

Ziel:

```text
Random Data
     ↓
Vault Parser
     ↓
No Crash
```

Testfälle:

* zufällige Bytes
* riesige Größen
* abgeschnittene Dateien
* falsche Versionen
* falsche Header
* kaputte Payloads


---

# 109. Definition of Done

Eine Funktion ist erst fertig, wenn:

- [ ] implementiert
- [ ] getestet
- [ ] Fehlerfälle getestet
- [ ] Memory Safety geprüft
- [ ] dokumentiert
- [ ] keine unnötigen Warnings
- [ ] API verständlich
- [ ] Ownership klar
- [ ] Git Commit vorhanden


---

# 110. Was zunächst NICHT gebaut wird

- [ ] Cloud Sync
- [ ] Browser Extension
- [ ] Mobile App
- [ ] Server
- [ ] Netzwerk
- [ ] Account System
- [ ] Sharing
- [ ] Multi User
- [ ] Browser Autofill

Erst einmal:

**lokal + offline + sauber.**


---

# 111. SQLite

SQLite bleibt eine spätere Option.

Nicht weil SQLite schlecht wäre, sondern weil das eigene Vault-Format zunächst mehr über:

* Binärdaten
* Serialization
* Versionierung
* Parsing
* Storage Design

lehrt.

Später kann verglichen werden:

```text
Own Binary Format
        vs.
Encrypted SQLite
```


---

# 112. C++-Bibliotheken später

Nicht jede Library wird sofort eingebaut.

Mögliche spätere Kandidaten:

## Eigen

Für:

* Matrizen
* Vektoren
* Transformationen
* Quaternionen
* Lineare Algebra

Das ist besonders relevant für Robotik.

## fmt

Für:

* modernes String Formatting
* strukturierte Ausgabe

## spdlog

Für:

* Logging
* Log Levels
* strukturierte Logs

Diese Bibliotheken werden erst hinzugefügt, wenn das Projekt einen konkreten Nutzen dafür hat.


---

# 113. Warum Eigen für Robotik wichtig wird

Später bei Robotik:

```text
Sensor
 ↓
Messwerte
 ↓
Vektor / Matrix
 ↓
Transformation
 ↓
Controller
 ↓
Motor
```

Typische mathematische Konzepte:

* 2D/3D Vektoren
* Matrizen
* Rotationen
* Koordinatensysteme
* Transformationen
* Quaternionen
* Kalibrierung

Eigen ist dafür eine sehr relevante C++-Bibliothek.


---

# 114. Kehl Vault als Robotics-Vorbereitung

Viele Konzepte kommen später direkt wieder.

## Kehl Vault

```text
Hardware Abstraction
Application Layer
Core
Storage
Error Handling
State Management
Testing
CMake
```

## Robotik

```text
Hardware Abstraction
Control Layer
Application Layer
State Management
Logging
Testing
CMake
```

Die Namen ändern sich.

Die Softwarearchitektur bleibt überraschend ähnlich.


---

# 115. Gemeinsame Konzepte mit Robotik

## State Machines

Kehl Vault:

```text
LOCKED
UNLOCKING
UNLOCKED
SAVING
LOCKING
ERROR
```

Robotik:

```text
IDLE
STARTING
DRIVING
TURNING
ERROR
FINISHED
```

Die Architekturidee ist dieselbe.


---

# 116. Hardware Abstraction in der Robotik

Später:

```text
Robot
 ↓
Motor Interface
 ↓
VEX Motor
```

statt:

```text
Robot
 ↓
direkt VEX API
```

Das gleiche Prinzip wie:

```text
Kehl Vault
 ↓
Platform API
 ↓
Linux / Windows / macOS
```


---

# 117. Robotics-Lernpfad nach Kehl Vault

Nach Kehl Vault kann die Architektur fast direkt weiterverwendet werden.

```text
C Basics
   ↓
C++
   ↓
STL
   ↓
CMake
   ↓
OO Design
   ↓
State Machines
   ↓
Linear Algebra
   ↓
Control
   ↓
Hardware Abstraction
   ↓
Robot Software
```


---

# 118. Sinnvolle Robotics-Projekte danach

## Projekt 1 – C++

Motor-/Sensor-Simulation im Terminal.

## Projekt 2

PID Controller.

## Projekt 3

2D Robot Simulator.

## Projekt 4

Sensor Fusion.

## Projekt 5

VEX Robot Framework.

## Projekt 6

Hardware Abstraction Layer.

## Projekt 7

kleine autonome Robotik-Anwendung.


---

# 119. Beispiel einer späteren Robotik-Struktur

```text
robot/
│
├── core/
│   ├── Vector
│   ├── Pose
│   ├── PID
│   └── StateMachine
│
├── hardware/
│   ├── Motor
│   ├── Encoder
│   ├── DistanceSensor
│   └── InertialSensor
│
├── control/
│   ├── DriveController
│   └── TurnController
│
├── application/
│   └── Robot.cpp
│
└── main.cpp
```

Das ist konzeptionell sehr ähnlich zu:

```text
kehl-vault/
├── core/
├── application/
├── platform/
└── gui/
```


---

# 120. Die große Lernkurve

```text
C
│
├── Pointer
├── Memory
├── Structs
├── File I/O
└── Low Level
│
▼
C++
│
├── Classes
├── RAII
├── STL
├── Templates
└── Modern C++
│
▼
Software Engineering
│
├── CMake
├── Git
├── Tests
├── Debugging
└── Static Analysis
│
▼
Application Development
│
├── SDL3
├── Dear ImGui
└── Platform APIs
│
▼
Security
│
├── libsodium
├── Argon2id
└── Authenticated Encryption
│
▼
Robotics
│
├── Eigen
├── State Machines
├── Control
├── Hardware Abstraction
└── Real-Time-ish Systems
```


---

# 121. Wichtigste Architekturregel

Bei jedem neuen Feature zuerst fragen:

> In welcher Schicht gehört dieses Feature?

Mögliche Antworten:

```text
Core
Application
GUI
Platform
Storage
Crypto
```

Beispiele:

```text
Password Generator
→ Core

Argon2id
→ Crypto

Save Vault
→ Storage

Login Screen
→ GUI

Auto-Lock Logic
→ Application/Core

Clipboard
→ Platform

Entry Search
→ Core

Search Bar
→ GUI
```


---

# 122. Sprache bewusst auswählen

Wenn etwas sehr systemnah ist:

**C kann sinnvoll sein.**

Wenn etwas stark objektorientiert oder komplex abstrahiert ist:

**C++ kann sinnvoll sein.**

Wenn eine Library nur eine C-API besitzt:

**aus C++ heraus verwenden.**

Wenn eine Hardwareplattform C++ verwendet:

**C++ direkt einsetzen.**

Es geht nicht darum, überall dieselbe Sprache zu erzwingen.


---

# 123. Finale Checkliste – C

- [ ] Variables
- [ ] Data Types
- [ ] Conditions
- [ ] Loops
- [ ] Functions
- [ ] Arrays
- [ ] Strings
- [ ] Structs
- [ ] Enums
- [ ] Pointers
- [ ] Dynamic Memory
- [ ] File I/O
- [ ] Serialization
- [ ] Headers
- [ ] Modules
- [ ] Error Codes


---

# 124. Finale Checkliste – C++

- [ ] Classes
- [ ] Constructors
- [ ] Destructors
- [ ] RAII
- [ ] References
- [ ] Smart Pointers
- [ ] `std::string`
- [ ] `std::vector`
- [ ] `std::unordered_map`
- [ ] `std::optional`
- [ ] `std::variant`
- [ ] Lambdas
- [ ] Algorithms
- [ ] Move Semantics
- [ ] Templates
- [ ] Namespaces
- [ ] modern Error Handling


---

# 125. Finale Checkliste – Software Engineering

- [ ] CMake
- [ ] Git
- [ ] Unit Tests
- [ ] Integration Tests
- [ ] CTest
- [ ] Debugger
- [ ] ASan
- [ ] UBSan
- [ ] Valgrind
- [ ] Cppcheck
- [ ] Clang-Tidy
- [ ] Fuzzing
- [ ] Documentation
- [ ] Packaging
- [ ] Releases


---

# 126. Finale Checkliste – GUI

- [ ] SDL3
- [ ] Dear ImGui
- [ ] Window
- [ ] Events
- [ ] Input
- [ ] Login
- [ ] Entry List
- [ ] Search
- [ ] Entry Editor
- [ ] Generator
- [ ] Clipboard
- [ ] Settings
- [ ] Themes
- [ ] Auto-Lock


---

# 127. Finale Checkliste – Security

- [ ] Master Password niemals speichern
- [ ] keine Secrets loggen
- [ ] Argon2id
- [ ] Salt
- [ ] Secure Random
- [ ] Authenticated Encryption
- [ ] Nonces korrekt behandeln
- [ ] Manipulation erkennen
- [ ] Secret Lifetime minimieren
- [ ] Clipboard Timeout
- [ ] Auto-Lock
- [ ] Memory Safety
- [ ] Parser Hardening


---

# 128. Finale Checkliste – Cross Platform

## Linux

- [ ] Build
- [ ] Run
- [ ] Paths
- [ ] Clipboard
- [ ] GUI
- [ ] Packaging

## Windows

- [ ] Build
- [ ] Run
- [ ] Paths
- [ ] Clipboard
- [ ] GUI
- [ ] Packaging

## macOS

- [ ] Build
- [ ] Run
- [ ] Paths
- [ ] Clipboard
- [ ] GUI
- [ ] Packaging


---

# 129. Finale Checkliste – Robotics Transfer

- [ ] C Grundlagen
- [ ] C++ Grundlagen
- [ ] RAII
- [ ] STL
- [ ] CMake
- [ ] State Machines
- [ ] Hardware Abstraction
- [ ] Error Handling
- [ ] Unit Tests
- [ ] Logging
- [ ] Eigen
- [ ] Linear Algebra
- [ ] PID
- [ ] Sensor Processing
- [ ] Control Architecture


---

# 130. Endzustand

Am Ende steht:

```text
                    KEHL VAULT
                         │
        ┌────────────────┴────────────────┐
        │                                 │
       GUI                               CLI
        │                                 │
        └────────────────┬────────────────┘
                         │
                  C++ Application
                         │
                    Core / API
                         │
             ┌───────────┴───────────┐
             │                       │
            C17                    C++20
             │                       │
       low-level modules       abstractions
             │                       │
             └───────────┬───────────┘
                         │
                    Storage / Crypto
                         │
                      libsodium
                         │
                    Platform Layer
                         │
              Linux / Windows / macOS
```


---

# 131. Der eigentliche Wert des Projekts

Kehl Vault soll dich Schritt für Schritt von:

```text
"Ich kann C-Code schreiben."
```

zu:

```text
"Ich kann eine größere C/C++-Softwarearchitektur entwerfen."
```

bringen.

Und danach von:

```text
Desktop Software
```

weiter zu:

```text
Robotik
Embedded
Hardware
Control Systems
```


---

# 132. Empfohlene Reihenfolge – kompakt

```text
01  C Basics
02  Structs
03  Pointer
04  Dynamic Memory
05  Strings
06  Entry System
07  Vault System
08  Password Generator
09  File I/O
10  Serialization
11  Vault Format
12  C++ Basics
13  Classes
14  RAII
15  STL
16  C ↔ C++
17  C++ Application Layer
18  SDL3
19  Dear ImGui
20  GUI
21  Search
22  Master Password
23  libsodium
24  Encryption
25  Clipboard
26  Auto-Lock
27  Settings
28  Platform Layer
29  CLI
30  Unit Tests
31  Sanitizers
32  Static Analysis
33  Fuzzing
34  Cross Platform
35  Packaging
36  Release
37  Robotics Transfer
```


---

# 133. Wichtigster Startpunkt

Nicht mit GUI anfangen.

Nicht mit Kryptografie anfangen.

Nicht mit C++-Templates anfangen.

Der Start ist:

```text
C
 ↓
Entry
 ↓
Vault
 ↓
Memory
 ↓
Password Generator
```

Danach:

```text
Storage
 ↓
Crypto
 ↓
C++
 ↓
GUI
```

Das hält die Lernkurve kontrollierbar und macht jeden neuen Abschnitt auf dem vorherigen aufbauend.


---

# 134. Ziel

## Kehl Vault 1.0

Ein plattformübergreifender C/C++ Desktop Password Manager mit:

* verschlüsseltem Vault
* Master Password
* Argon2id
* authenticated encryption
* Password Generator
* Entries
* Search
* Clipboard
* Auto-Lock
* Settings
* Themes
* GUI
* CLI
* Linux Support
* Windows Support
* macOS Support
* Unit Tests
* Memory Safety Checks
* Static Analysis
* sauberer C/C++-Architektur

Und gleichzeitig ein Lernprojekt, das die Grundlagen für spätere **C++-, Embedded- und Robotikprojekte** legt.

---

## Current progress

The current implementation status should be taken from the actual project files.

Do not assume that planned features are already implemented.

Before suggesting the next task:

* Check what is already implemented.
* Compare it with the roadmap.
* Identify the next unfinished project step.
* Keep the next step small and manageable.

## Scope control

The project plan defines the intended scope.

Do not add unrelated features just because they would be technically interesting.

When suggesting an additional feature that is not part of the plan, clearly label it as optional and do not implement it automatically.

## Code evolution

The project is intentionally being developed gradually.

Avoid replacing working beginner code with a more advanced implementation merely because it is more elegant or idiomatic.

Refactoring is appropriate when:

* the current structure prevents the next planned feature,
* the existing implementation contains a real problem,
* or I explicitly ask for a refactor.

Otherwise, prefer incremental changes.
