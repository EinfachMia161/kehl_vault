# Kehl Vault - Release Artifact Guide (v1.0.0)

### Project Name
**Kehl Vault** (`kehl_vault`)

### Release Version
`v1.0.0`

### Short Description
**Kehl Vault** is a secure, high-performance, and lightweight local password manager and multi-factor authenticator. It features authenticated encryption at rest (ChaCha20 + HMAC-SHA256 + PBKDF2 with 100,000 iterations), RFC 6238 TOTP 2FA code generation, Diceware passphrase generation, dynamic fuzzy search, automatic encrypted backup rotation, and volatile memory zeroization.

This release bundle includes both the interactive command-line application (`kehl-vault`) and the native desktop graphical application (`kehl-vault-gui`).

---

### Supported Operating System & Architecture
- **Target OS**: Windows 10, Windows 11, Windows Server 2016 or newer (64-bit)
- **Target Architecture**: x86_64 (AMD64 / 64-bit Intel & AMD processors)

---

### Included Release Artifacts

| Filename | Target / Role | Size | SHA-256 Checksum |
|---|---|---|---|
| `kehl-vault-v1.0.0-windows-x86_64.exe` | Standalone Interactive CLI Application | 1,062,400 bytes (~1.01 MB) | `f0ee774f09fa6cf0169f90104ec5bda22d80cf2cda3ccf06a27991c6aeeed254` |
| `kehl-vault-gui-v1.0.0-windows-x86_64.exe` | Native DirectX 11 / Win32 Desktop GUI Application | 1,154,048 bytes (~1.10 MB) | `03858691fce2f0cbe638873bab75fbed6ee73c489c088fd71ace73c89eb8e1a7` |
| `libwinpthread-1.dll` | POSIX Threading Companion DLL | 57,344 bytes (~56 KB) | `e65ea1ac1e8c1b02ad84cd61ed035eeb73a338953cad3d55e7966292869f509b` |
| `kehl-vault-v1.0.0-windows-x86_64.exe.sha256` | Checksum verification file for CLI | Text | `f0ee774f...` |
| `kehl-vault-gui-v1.0.0-windows-x86_64.exe.sha256` | Checksum verification file for GUI | Text | `03858691...` |
| `RELEASE_README.md` | User guide & release documentation | Text | This document |

---

### Installation Instructions
1. Download all release files from the GitHub Release page into a folder of your choice (e.g., `C:\Program Files\KehlVault\` or `C:\Users\<YourUser>\KehlVault\`).
2. Ensure `libwinpthread-1.dll` remains in the **same directory** as the executables.
3. No installer or administrative privileges are required; Kehl Vault is fully portable.

---

### How to Run the Software

#### 1. Running the Graphical Desktop Application (GUI)
- Double-click `kehl-vault-gui-v1.0.0-windows-x86_64.exe` in Windows File Explorer.
- The dark-themed desktop interface will launch, providing interactive credential management, clipboard copy buttons, real-time password strength indicators, and live TOTP 30-second timers.

#### 2. Running the Command-Line Interface (CLI)
Open PowerShell or Command Prompt in the folder and execute:
```powershell
# Interactive console menu
.\kehl-vault-v1.0.0-windows-x86_64.exe

# Quick single-command tasks
.\kehl-vault-v1.0.0-windows-x86_64.exe --version
.\kehl-vault-v1.0.0-windows-x86_64.exe --gen-pass 24
.\kehl-vault-v1.0.0-windows-x86_64.exe --gen-words 5
.\kehl-vault-v1.0.0-windows-x86_64.exe --smoke-test
```

---

### First-Run Instructions
1. When you run Kehl Vault for the first time, it detects that no vault database exists yet.
2. You will be prompted to set up a new **Master Password**. Choose a strong, memorable passphrase.
3. Kehl Vault will initialize `vault.dat` using PBKDF2 key derivation (100,000 iterations) and 256-bit ChaCha20 encryption.
4. **Important**: Your Master Password is the only key to your vault. If lost, your encrypted data cannot be recovered.

---

### Data and Configuration Storage Locations
Kehl Vault stores its files in the directory from which it is executed:
- `vault.dat`: The encrypted primary vault database.
- `vault_<profile>.dat`: Profile-specific vault databases when using multi-profile management (e.g., `vault_work.dat`, `vault_personal.dat`).
- `backups/`: Directory containing timestamped encrypted backup snapshots (`vault_backup_YYYYMMDD_HHMMSS.dat`).
- `imgui.ini` / `kehl_vault_gui.ini`: Optional UI layout preference configuration.

---

### Required Runtime Dependencies
Kehl Vault is built with statically linked C/C++ runtimes (`-static-libgcc -static-libstdc++`) and standard Universal C Runtime (`UCRT`) bindings:
- **Windows System Libraries** (preinstalled on Windows 10/11):
  - `bcrypt.dll` (Windows Cryptographic Primitives / CSPRNG)
  - `kernel32.dll`, `user32.dll`, `gdi32.dll`, `dwmapi.dll`
  - `d3d11.dll`, `dxgi.dll` (DirectX 11 graphics for GUI)
  - `api-ms-win-crt-*` / `ucrtbase.dll` (Microsoft Universal C Runtime)
- **Included Companion Library**:
  - `libwinpthread-1.dll` (must reside in the application directory)

---

### Security Considerations
- **Authenticated Encryption**: ChaCha20 stream cipher combined with HMAC-SHA256 (Encrypt-then-MAC) ensures privacy and tamper resistance.
- **Key Derivation**: PBKDF2-HMAC-SHA256 with 100,000 rounds and a 16-byte cryptographic salt protects against offline brute-force and rainbow table attacks.
- **RAM Protection**: Sensitive plaintext credentials and encryption keys are scrubbed from RAM using compiler-barrier memory zeroization (`secure_memzero`).
- **Clipboard Scrubbing**: Credential copies to the system clipboard are queued for automatic wiping.
- **Offline & Private**: Kehl Vault makes no network connections and does not transmit any telemetry or passwords over the internet.

---

### How to Verify the SHA-256 Checksum

#### Using PowerShell:
```powershell
Get-FileHash -Algorithm SHA256 .\kehl-vault-v1.0.0-windows-x86_64.exe
Get-FileHash -Algorithm SHA256 .\kehl-vault-gui-v1.0.0-windows-x86_64.exe
Get-FileHash -Algorithm SHA256 .\libwinpthread-1.dll
```

#### Using Windows Command Prompt:
```cmd
certutil -hashfile kehl-vault-v1.0.0-windows-x86_64.exe SHA256
certutil -hashfile kehl-vault-gui-v1.0.0-windows-x86_64.exe SHA256
certutil -hashfile libwinpthread-1.dll SHA256
```

Compare the calculated hash with the hashes listed in the table above or the `.sha256` files.

---

### Known Limitations
- Supported exclusively on 64-bit Windows environments (x86_64).
- The Desktop GUI requires a graphics adapter supporting DirectX 11 or newer.
- Data synchronization across devices is manual or managed via user-configured file sync tools (e.g. Git, OneDrive), as Kehl Vault is fully offline by design.

---

### Troubleshooting Common Problems

#### 1. "The code execution cannot proceed because libwinpthread-1.dll was not found"
- **Solution**: Ensure `libwinpthread-1.dll` was extracted into the same directory as `kehl-vault-v1.0.0-windows-x86_64.exe` and `kehl-vault-gui-v1.0.0-windows-x86_64.exe`.

#### 2. Windows Defender / SmartScreen "Windows protected your PC"
- **Solution**: Because this release binary is an open-source binary without an expensive commercial code-signing certificate, Windows SmartScreen may display a warning on initial launch. Click **"More info"** and then select **"Run anyway"**.

#### 3. "Decryption Failed: Invalid master password or corrupted file"
- **Solution**: Verify that Caps Lock is off. If the vault file was corrupted, check the `backups/` directory for recent encrypted snapshots and rename the latest snapshot to `vault.dat`.

---

### Build Information
- **Compiler**: GCC 15.2.0 (`x86_64-w64-mingw32`, Universal C Runtime `-mcrtdll=ucrt`)
- **Build System**: CMake 3.25+ with Ninja generator
- **Build Flags**: `-O3 -DNDEBUG -static-libgcc -static-libstdc++ -ffunction-sections -fdata-sections -Wl,--gc-sections`
- **Symbol Stripping**: All debug symbols stripped using GNU `strip --strip-all`
- **Build Date**: 2026-09-17

---

### License Information
- **Kehl Vault**: Open Source.
- **Dear ImGui**: MIT License (Copyright &copy; 2014-2024 Omar Cornut).
- **stb libraries**: Public Domain / MIT License (Sean Barrett).

---

### GitHub Repository
Source code, issue tracking, and future releases:
- [https://github.com/EinfachMia161/kehl_vault](https://github.com/EinfachMia161/kehl_vault)





<img width="1010" height="693" alt="image" src="https://github.com/user-attachments/assets/2e6ba54d-d0c8-4ae7-8789-85721b495897" />
<img width="1010" height="693" alt="image" src="https://github.com/user-attachments/assets/d40867eb-6ed9-4b3d-b682-4650ffa8ea4f" />
<img width="1010" height="693" alt="image" src="https://github.com/user-attachments/assets/5b143eb5-70cc-4c64-a559-ee28b518f133" />
<img width="1010" height="693" alt="image" src="https://github.com/user-attachments/assets/d94e2636-5ba0-41cd-b994-283cf41b8780" />
<img width="1010" height="693" alt="image" src="https://github.com/user-attachments/assets/2198d5fa-c31e-43d3-ac83-54020d5e4ffc" />
<img width="1010" height="693" alt="image" src="https://github.com/user-attachments/assets/a4e80d25-cdee-4b4b-a1dd-2d91328968f7" />
<img width="1010" height="693" alt="image" src="https://github.com/user-attachments/assets/7a87eea3-88b8-4c79-a9ee-e6c4558afe46" />
<img width="1010" height="693" alt="image" src="https://github.com/user-attachments/assets/079f01d3-e415-4386-812a-65c7cb033b79" />
<img width="1010" height="693" alt="image" src="https://github.com/user-attachments/assets/e5c82725-b87a-499a-a89d-055c5d34da04" />
<img width="1010" height="693" alt="image" src="https://github.com/user-attachments/assets/0d3e0360-8130-44ff-8e99-b81446878a51" />
