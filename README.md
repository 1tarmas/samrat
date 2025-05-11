# VCrypt - A VeraCrypt-like Encryption Tool

![VCrypt Logo](docs/logo.png) *(optional)*

A cross-platform file and disk encryption tool with similar functionality to VeraCrypt.

## Features
- AES-256, Serpent, and Twofish encryption
- XTS mode for disk encryption
- Secure password-based key derivation
- Basic GUI interface (Qt)

## Installation

### Requirements
- OpenSSL 1.1.1+
- Qt5 (for GUI)
- C++17 compiler

### Build Instructions
```bash
git clone https://github.com/yourusername/vcrypt.git
cd vcrypt
mkdir build && cd build
cmake .. && make