# Kehl Vault
Kehl Vault is a beginner-friendly C and C++ learning project.
The long-term goal is to build a local, offline password vault while learning how to design and develop a larger C/C++ application step by step.
Educational notice: This project is not ready for storing real passwords or other sensitive information.## Project Status
The project is currently in the early C learning phase.
Implemented so far:
- C17 and C++20 configuration with CMake
- C and C++ interoperability
- C-compatible headers
- Password length validation
- Password character analysis
- Basic password strength calculation
- Password generator core using externally provided test bytes
- Entry data model using a C `struct`
- Fixed-size entry array
- Entry creation
- Adding entries
- Accessing entries through pointers
- Updating entries
- Removing entries
- Basic input validation
- Basic entry capacity checks
- Basic error handling through return values

The current implementation stores entries only in memory. No data is written to files yet.
## Project Goals
The long-term goal is to create a local, offline password vault with:
- Vault creation
- Vault opening
- Vault locking
- Entry creation
- Entry editing
- Entry deletion
- Entry search
- Password generation
- Clipboard support
- Auto-lock
- Settings
- Graphical user interface
- Optional command-line interface
- Cross-platform support

The project is intentionally developed step by step instead of implementing all features at once.
## Learning Goals
This project is designed to teach the fundamentals of C and C++.
### C
- Variables
- Data types
- Conditions
- Loops
- Functions
- Arrays
- Strings
- `printf`
- Return values
- `enum`
- `struct`
- Pointers
- Dynamic memory
- File I/O
- Serialization
- Error codes
- C-compatible APIs

### C++
- Namespaces
- `std::string`
- `std::vector`
- `std::array`
- References
- `const`
- Classes
- Constructors
- Destructors
- RAII
- Smart pointers
- STL containers
- Application architecture
- C and C++ interoperability

### Software Engineering
- CMake
- Git
- Unit testing
- CTest
- Debugging
- AddressSanitizer
- UndefinedBehaviorSanitizer
- Static analysis
- Memory safety
- Cross-platform development

## Current Architecture
The project is intentionally split between C and C++.``` text
C++ application
      |
      v
C-compatible header
      |
      v
C core module
```

The current project structure is:``` text
kehl-vault/
├── CMakeLists.txt
├── main.cpp
├── entry.h
├── entry.c
├── password.h
├── password.c
└── scripts/
```

main.cpp
The C++ entry point of the application.
It currently demonstrates how the C modules can be used from C++.
entry.h
The public C interface for the Entry module.
It contains:
the Entry structure,
Entry-related function declarations,
C/C++ compatibility handling.
entry.c
The implementation of the Entry module.
It currently provides:
Entry creation,
Entry insertion,
Entry access,
Entry editing,
Entry removal,
Entry list printing.
password.h
The public C interface for password-related functionality.
password.c
The implementation of password-related functionality.
It currently provides:
Password length validation
Lowercase detection
Uppercase detection
Digit detection
Special-character detection
Basic password strength calculation
Password generator core using externally provided bytes
 
Building the Project
Requirements
CLion
CMake
A C compiler
A C++ compiler
C17 support
C++20 support
The project is currently configured for the MinGW toolchain provided by CLion on Windows.
Build with CLion
Open the project in CLion.
Wait for CMake to finish loading.
Select the kehl_vault run configuration.
Choose Build → Build Project.
Alternatively, use Ctrl+F9.
Run with CLion
Run the project using the green Run button or Shift+F10.
The executable is created inside the CMake build directory.
 
Current Example Output
The current program demonstrates Entry creation, pointer access, editing, and removal.
Example output:``` text
Entries vor der Bearbeitung:
----------------------------
Entry 1
-------
Titel: Beispielkonto
Benutzername: mimi@example.com

Entry 2
-------
Titel: Schulkonto
Benutzername: mimi@schule.example

Entry 3
-------
Titel: Testkonto
Benutzername: test@example.com

Entry mit Index 1 wurde gefunden.

Entries nach der Bearbeitung:
-----------------------------
Entry 1
-------
Titel: Beispielkonto
Benutzername: mimi@example.com

Entry 2
-------
Titel: Geaendertes Schulkonto
Benutzername: neuer-benutzername@example.com

Entry 3
-------
Titel: Testkonto
Benutzername: test@example.com

Index 5 wurde korrekt abgelehnt.

Entry mit Index 0 loeschen: erfolgreich.
```

Passwords are intentionally not printed.
 
Security Disclaimer
This project is not a production-ready password manager.
Do not store important real-world passwords in it.
The current project does not provide:
Encryption
Secure password storage
Secure memory handling
Authenticated file storage
Cryptographically secure password generation
Security review
Protection against memory inspection
Clipboard protection
Auto-lock
The password generator currently uses test input bytes and is intended only for learning and testing the generator logic.
For future security-related functionality, established libraries will be used instead of implementing cryptography manually.
 
Development Roadmap
The planned development order is:``` text
1. C basics
2. Structs
3. Pointers
4. Dynamic memory
5. C string handling
6. Entry system
7. Vault system
8. Password generator
9. File I/O
10. Serialization
11. Vault file format
12. C++ basics
13. C++ classes
14. RAII
15. STL
16. C and C++ integration
17. C++ application layer
18. SDL3
19. Dear ImGui
20. GUI
21. Search
22. Master password
23. libsodium integration
24. Authenticated encryption
25. Clipboard
26. Auto-lock
27. Settings
28. Platform layer
29. CLI
30. Unit tests
31. Sanitizers
32. Static analysis
33. Fuzzing
34. Cross-platform support
35. Packaging
36. Release preparation
```

The project should follow this order as closely as possible.
 
Current Learning Phase
The project is currently focusing on:
C structures
Arrays of structures
Functions
Pointers
Pointer validation
Fixed-size entry lists
Error codes
C and C++ interoperability
Updating and removing entries
Returning pointers from functions
The next planned subjects are:
More pointer exercises
Pointer ownership and lifetime
Dynamic memory
malloc
realloc
free
Memory safety
 
Development Principles
The project follows these principles:
Keep the code beginner-friendly.
Prefer readable code over clever code.
Introduce new concepts gradually.
Avoid unnecessary abstractions.
Keep C and C++ responsibilities clear.
Do not implement future features prematurely.
Do not implement cryptography manually.
Do not use insecure randomness for real passwords.
Do not log passwords or other secrets.
Test each step before continuing.
Keep the project local and offline.
Preserve the existing project structure where possible.
 
Git Commit Style
Commits should use short, descriptive messages.
Examples:``` text
feat: add password length validation
feat: analyze password character types
feat: add basic password strength evaluation
feat: add password generator core
feat: add entry data model
feat: add entry management functions
feat: add entry update and delete operations
feat: add entry pointer access
fix: reject invalid entry indexes
test: add entry management tests
```

The most recent planned commit is:``` text
feat: add entry pointer access
```

 
Future Architecture
The planned long-term architecture is:``` text
                 Kehl Vault
                     |
        +------------+------------+
        |                         |
       GUI                       CLI
        |                         |
        +------------+------------+
                     |
             C++ Application Layer
                     |
                 Core API
                     |
        +------------+------------+
        |                         |
      C Core                  C++ Layer
        |                         |
   Storage / Crypto       Application / GUI
```

The C core will handle low-level functionality such as:
Entry data
Storage
Serialization
C-compatible APIs
Low-level validation
The C++ layer will later handle:
Application logic
Controllers
Modern containers
RAII
GUI integration
Platform abstractions
 
License
This project is currently a personal learning project.
A license will be added later.
