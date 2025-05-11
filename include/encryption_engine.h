// encryption_engine.h
#pragma once
#include <vector>
#include <memory>
#include <openssl/evp.h>

class EncryptionEngine {
public:
    enum Algorithm { AES256, SERPENT, TWOFISH, CASCADE };
    
    EncryptionEngine(Algorithm algo);
    ~EncryptionEngine();
    
    void setKey(const std::vector<uint8_t>& key);
    void encryptBlock(uint64_t blockIndex, const uint8_t* plaintext, uint8_t* ciphertext);
    void decryptBlock(uint64_t blockIndex, const uint8_t* ciphertext, uint8_t* plaintext);
    
    static const size_t BLOCK_SIZE = 512;
    static const size_t KEY_SIZE = 64; // 512-bit for XTS mode

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};