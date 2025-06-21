# 🔐 CLI Password Manager in C

This is a simple and lightweight command-line password manager written in C. It provides functionality for securely generating, encrypting, storing, and retrieving passwords using a master password for encryption. It is intended for educational purposes and demonstrates concepts such as password generation, basic encryption using the XOR cipher, file I/O, and basic user input handling.

## 📌 Features

- Secure password generation using random characters (letters, numbers, symbols).
- XOR-based encryption and decryption using a user-defined master password.
- Persistent storage of encrypted passwords in a file along with their purpose and timestamp.
- Command-line interface for password creation and retrieval.
- Initializes storage directory and master password file automatically on first run.

## 🛠️ Requirements

- GCC or any C compiler
- A POSIX-compliant operating system (Linux, macOS, or Windows with WSL or MinGW)


## 🚀 How to Build and Run

### 1. Compile the program:

```bash
gcc main.c -o password_manager
```
### 2. Run Program
```bash
password_manager.exe
```

