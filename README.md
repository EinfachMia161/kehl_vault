# Kehl Vault

Kehl Vault is a beginner-friendly C17/C++20 learning project for building a local password-vault application step by step.

> **Educational notice:** The project is not security-reviewed and should not be used to store real passwords or other sensitive information.

## Current project state

The current repository contains:

- C17 core modules with C-compatible headers
- C++20 CLI application integration
- Heap-backed `EntryList` with CRUD operations and automatic growth
- Password validation, strength scoring, password generation and passphrase generation
- OS-backed random-byte acquisition for password generation
- Persistent vault files
- Legacy unencrypted vault format (version 1)
- Encrypted vault format (version 2)
- Custom SHA-256, HMAC-SHA256, PBKDF2-HMAC-SHA256 and ChaCha20 implementations
- Constant-time byte comparison for authentication tags
- Clipboard integration
- Password reuse / weakness auditing
- CSV and JSON import/export
- Committed C++ integration tests in `tests/test_vault.cpp`

The encrypted storage path uses a 16-byte salt, a 12-byte ChaCha20 nonce, PBKDF2-HMAC-SHA256 with 100,000 iterations, separate 32-byte encryption/authentication keys, ChaCha20 payload encryption, and HMAC-SHA256 over the ciphertext.

This cryptographic design is implemented for learning and has **not** received an independent security review. It is therefore not equivalent to a production password manager using a mature, externally reviewed cryptographic library.

The current file format also serializes native C structures directly. That makes the representation dependent on the build ABI and does not yet constitute a deliberately specified, portable wire format.

## Project structure

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
├── CODEBASE_DOCUMENTATION.md
└── tests/
    └── test_vault.cpp
```

## Module overview

### Entry management
`entry.c` provides the `Entry` data model and heap-backed `EntryList`, including initialization, insertion, lookup, update, removal and capacity growth.

### Password handling
`password.c` provides rule-based password analysis, strength scoring, OS-backed random-byte acquisition, password generation and passphrase generation. Generated character indexes currently use modulo reduction, so the generator has not been presented as mathematically unbiased.

### Persistence
`storage.c` saves and loads `EntryList` data. Version 1 stores entries without encryption. Version 2 derives two 32-byte keys from the master password and uses ChaCha20 plus HMAC-SHA256 for the payload.

### Cryptography
`crypto.c` contains the current educational implementations of SHA-256, HMAC-SHA256, PBKDF2-HMAC-SHA256, ChaCha20 and constant-time comparison. The long-term hardening direction is to replace application-owned cryptographic primitives with a maintained cryptographic library.

### Clipboard
`clipboard.c` uses native Win32 clipboard APIs on Windows and common command-line clipboard tools on POSIX systems (`wl-copy`, `xclip`, `xsel`, `pbcopy`).

### Security audit
`audit.c` reports password weakness and reused-password indicators across entries. Its health score is an application heuristic, not a cryptographic assurance metric.

### Import/export
`impex.c` provides CSV and JSON import/export. Exported credentials may be written in plaintext and must therefore be handled like sensitive data.

## Building

Requirements:

- CMake 4.3+
- C compiler with C17 support
- C++ compiler with C++20 support
- CLion is suitable for local development

Configure and build:

```bash
cmake -S . -B build
cmake --build build --parallel
```

Run the CLI:

```bash
./build/kehl_vault
```

Run the test suite:

```bash
ctest --test-dir build --output-on-failure
```

The test executable is also available directly as `build/test_vault` on typical Unix-like builds.

## Security roadmap

The main hardening tasks are:

1. Replace custom cryptographic primitives with a maintained cryptographic library.
2. Define an explicit portable binary format with fixed-width fields and explicit byte order.
3. Authenticate integrity-sensitive format metadata, not only the ciphertext payload.
4. Introduce deliberate secure-zeroization and minimize secret copies in memory.
5. Improve password-generator index selection to avoid modulo bias.
6. Add sanitizer, static-analysis and fuzzing pipelines.
7. Review import/export handling and secret exposure paths.
8. Obtain an independent security review before any real-secret use.

## License / status

This repository is a personal educational project and is currently evolving. The implementation and format may change without backward-compatibility guarantees.
