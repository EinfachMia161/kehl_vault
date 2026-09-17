# Kehl Vault – Codebase & Architecture Documentation

## 1. Purpose and current status

Kehl Vault is an educational C17/C++20 password-vault project. It is built incrementally to teach systems programming, memory management, file handling, application structure, and security engineering.

The current source contains an interactive CLI application, dynamic entry management, persistent vault files, a custom cryptographic layer, clipboard integration, password auditing, CSV/JSON import-export, and a committed C++ test program.

> **Security notice:** The project is not independently security-reviewed and must not be used as a real password manager or trusted credential store.

This document describes the implementation currently present in the repository. Future GUI, RAII-controller and cryptographic-hardening work is explicitly separated from current code.

## 2. Repository layout

```text
kehl_vault/
├── CMakeLists.txt
├── main.cpp
├── entry.h / entry.c
├── password.h / password.c
├── storage.h / storage.c
├── crypto.h / crypto.c
├── clipboard.h / clipboard.c
├── audit.h / audit.c
├── impex.h / impex.c
├── README.md
├── CODEBASE_DOCUMENTATION.md
├── .github/workflows/build.yml
└── tests/
    └── test_vault.cpp
```

The low-level modules are written in C. `main.cpp` and `tests/test_vault.cpp` provide the current C++20 integration layer.

## 3. Build system

`CMakeLists.txt` requires CMake 4.3 or newer and enables:

- C17 for the C modules
- C++20 for the application and test runner

The build creates:

- `kehl_vault` – interactive CLI application
- `test_vault` – committed C++ integration/unit-style test executable

CTest is enabled and registers `test_vault` as the `test_vault` test.

Typical build:

```bash
cmake -S . -B build
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

The repository also contains GitHub Actions that configure and build the project, run the CTest suite, and execute the CLI as a smoke test for pushes and pull requests targeting `master`.

## 4. C/C++ boundary

Public C headers use an `extern "C"` wrapper when included from C++:

```c
#ifdef __cplusplus
extern "C" {
#endif

/* C declarations */

#ifdef __cplusplus
}
#endif
```

This preserves C linkage names while allowing the C++ application to consume the C APIs.

The repository does not yet contain the larger opaque-handle `VaultController` architecture described by the site's target specifications. That remains future work.

## 5. Entry management – `entry.h` / `entry.c`

`Entry` stores fixed-size character buffers for title, username and password. `EntryList` is a heap-backed array carrying its current count and capacity.

Conceptually:

```text
EntryList
├── entries   → heap array of Entry
├── count     → valid element count
└── capacity  → allocated element capacity
```

The module currently provides initialization, insertion, indexed lookup, update, removal, list printing, destruction and capacity growth. When the list reaches capacity, the backing allocation grows before insertion; removal compacts the remaining entries.

The data structure provides bounded application buffers, but the project does not yet claim a complete secure-secret memory lifecycle or independent memory-safety certification.

## 6. Password handling – `password.h` / `password.c`

### 6.1 Validation

The current password helpers check minimum length, lowercase characters, uppercase characters, digits and special characters. `password_calculate_strength()` returns a 0–5 rule score. This score is a simple application heuristic, not a general password-strength estimator.

The public functions now reject NULL pointers and invalid length parameters before calling string functions.

### 6.2 Random bytes

`password_get_secure_random_bytes()` currently uses:

- Windows: `BCryptGenRandom(..., BCRYPT_USE_SYSTEM_PREFERRED_RNG)`
- POSIX: reads from `/dev/urandom`

### 6.3 Password and passphrase generation

The module can build a configurable character set and generate passwords from random bytes. It also provides multi-word passphrase generation from an embedded word list.

The current character selection uses modulo reduction. When the character-set size does not divide the source byte range evenly, modulo bias can occur. This is an explicit hardening item rather than a hidden security claim.

## 7. Cryptography – `crypto.h` / `crypto.c`

The repository currently implements these primitives directly:

- SHA-256
- HMAC-SHA256
- PBKDF2-HMAC-SHA256
- ChaCha20
- constant-time byte comparison

This is an educational custom implementation. Passing functional tests does not prove resistance to implementation flaws, side-channel problems, misuse, or cryptanalysis.

The long-term hardening direction is to move the application cryptographic boundary onto a maintained, externally reviewed cryptographic library instead of keeping primitive implementations inside the application codebase.

## 8. Persistent storage – `storage.h` / `storage.c`

### 8.1 File versions

`storage.h` defines:

```c
#define VAULT_VERSION_LEGACY 1
#define VAULT_VERSION_ENCRYPTED 2
#define VAULT_CURRENT_VERSION 2
```

Version 1 is the legacy unencrypted representation. Version 2 protects the entry payload with the custom cryptographic layer.

### 8.2 Current header

The current header is:

```c
typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t entry_count;
    uint8_t  salt[CRYPTO_SALT_SIZE];
    uint8_t  nonce[CRYPTO_NONCE_SIZE];
    uint8_t  auth_tag[CRYPTO_SHA256_HASH_SIZE];
} VaultHeader;
```

The field widths sum to 72 bytes on a conventional ABI. `storage.c` serializes this native C structure directly, so it is not yet a deliberately specified portable wire format.

The current magic constant is `VAULT_MAGIC = 0x564B4548`, commented as `KEHV` in the header. It is a numeric `uint32_t`, not an ASCII `KVLT` byte array.

### 8.3 Version 2 protection flow

The current encrypted path is conceptually:

```text
master password
       │
       ▼
PBKDF2-HMAC-SHA256, 100,000 iterations
       │
       ▼
64 derived bytes
 ┌─────┴─────┐
 ▼           ▼
32-byte      32-byte
enc key      auth key
 │            │
 ▼            │
ChaCha20      │
 │            │
 ▼            ▼
ciphertext → HMAC-SHA256
```

The load path checks the expected file marker and version, validates the entry count, verifies the authentication tag, and only then accepts the encrypted payload for decryption.

### 8.4 Current limitations

The native C structure makes the file format ABI-dependent. Integer byte order and structure layout are not separately encoded on disk.

The current HMAC authenticates the encrypted payload, but integrity-sensitive header values are not fully covered. A future format should explicitly serialize fields and authenticate the complete metadata needed to interpret the payload.

Temporary-file replacement can reduce the risk of partially written destination files, but it cannot honestly guarantee that data loss is impossible under every filesystem or power-failure scenario.

## 9. Clipboard – `clipboard.h` / `clipboard.c`

The implementation currently supports:

- native Win32 clipboard APIs on Windows
- `wl-copy`
- `xclip`
- `xsel`
- `pbcopy`

The clipboard module validates its text pointer before use. Timed clipboard clearing is a future application-layer feature; copying data successfully does not imply that the operating system will erase all historical copies of that data.

## 10. Security audit – `audit.h` / `audit.c`

The audit module analyzes the current entry set for password weakness and reuse. It also derives a vault-health score from those application-level indicators.

The score is a heuristic and must not be read as a cryptographic security level, formal risk metric or compliance assessment.

## 11. Import/export – `impex.h` / `impex.c`

The current project supports CSV and JSON import/export.

The implementation is covered by tests for representative escaping and formatting cases. Exported files may contain credentials in plaintext and therefore require the same handling as other sensitive data.

## 12. CLI – `main.cpp`

`main.cpp` provides the current interactive C++20 application. It integrates the C modules into a menu-driven CLI workflow covering the implemented vault capabilities.

The current application includes workflows for:

- listing and searching entries
- creating, editing and deleting entries
- password generation
- clipboard copying
- security auditing
- CSV/JSON import-export
- vault save/load

Password input is masked in the terminal workflow.

The planned desktop GUI/controller architecture is not part of the current codebase.

## 13. Automated tests – `tests/test_vault.cpp`

The committed test program exercises a broad set of current modules, including:

- EntryList CRUD, resizing and bounds behavior
- SHA-256, HMAC, ChaCha20 and constant-time comparison
- persistence and encrypted/legacy file handling
- wrong-password and corruption rejection
- password validation and generation
- secure random-byte acquisition
- clipboard error paths
- password audit and reuse detection
- CSV and JSON import/export

These tests are regression and integration checks. They are not a substitute for independent cryptographic review or exhaustive platform testing.

## 14. Continuous integration

The current software CI is intended to follow:

```text
Checkout
   ↓
CMake configure
   ↓
Build application + tests
   ↓
CTest
   ↓
CLI smoke test
```

This makes the committed test suite part of the automated engineering loop rather than merely a source file that happens to exist.

## 15. Hardening roadmap

The most important remaining engineering tasks are:

1. Replace custom cryptographic primitives with a maintained cryptographic library.
2. Remove modulo bias from character selection in the password generator.
3. Define a portable binary format with explicit field widths and byte order.
4. Authenticate integrity-sensitive header metadata.
5. Establish deliberate secret-memory zeroization and minimize secret copies.
6. Add sanitizer, static-analysis and fuzzing CI.
7. Review clipboard and import/export exposure paths.
8. Complete the planned C++20 RAII/controller layer.
9. Add the planned SDL3/Dear ImGui GUI once the core interfaces are stable.
10. Obtain an independent security review before any real-secret use.

## 16. Documentation provenance

The associated documentation site follows three distinct evidence levels:

- **Source evidence:** the referenced file or implementation exists in `kehl_vault@master`.
- **Build/test evidence:** an actual build or automated test run exercised the behavior.
- **Target specification:** a planned architecture or interface that is intentionally not claimed as implemented.

A file existing in Git is therefore never treated as proof of runtime correctness, cryptographic security, portability, or production readiness.
