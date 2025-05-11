// encryption_engine.cpp
#include "encryption_engine.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdexcept>

class EncryptionEngine::Impl {
public:
    EVP_CIPHER_CTX* encryptCtx;
    EVP_CIPHER_CTX* decryptCtx;
    Algorithm currentAlgo;
    uint8_t key[KEY_SIZE];
    
    const EVP_CIPHER* getCipher() {
        switch(currentAlgo) {
            case AES256: return EVP_aes_256_xts();
            case SERPENT: return EVP_serpent_256_xts();
            case TWOFISH: return EVP_twofish_256_xts();
            case CASCADE: return EVP_aes_256_xts(); // Simplified
            default: throw std::runtime_error("Unknown algorithm");
        }
    }
    
    void generateIV(uint64_t blockIndex, uint8_t* iv) {
        // Convert block index to IV (XTS mode requires tweak)
        for (int i = 0; i < 16; i++) {
            iv[i] = (blockIndex >> (8 * (i % 8))) & 0xFF;
        }
    }
};

EncryptionEngine::EncryptionEngine(Algorithm algo) : pImpl(new Impl()) {
    pImpl->currentAlgo = algo;
    pImpl->encryptCtx = EVP_CIPHER_CTX_new();
    pImpl->decryptCtx = EVP_CIPHER_CTX_new();
    if (!pImpl->encryptCtx || !pImpl->decryptCtx) {
        throw std::runtime_error("Failed to create cipher context");
    }
}

EncryptionEngine::~EncryptionEngine() {
    if (pImpl->encryptCtx) {
        EVP_CIPHER_CTX_free(pImpl->encryptCtx);
    }
    if (pImpl->decryptCtx) {
        EVP_CIPHER_CTX_free(pImpl->decryptCtx);
    }
    // Securely wipe key from memory
    OPENSSL_cleanse(pImpl->key, KEY_SIZE);
}

void EncryptionEngine::setKey(const std::vector<uint8_t>& key) {
    if (key.size() != KEY_SIZE) {
        throw std::runtime_error("Invalid key size");
    }
    memcpy(pImpl->key, key.data(), KEY_SIZE);
    
    const EVP_CIPHER* cipher = pImpl->getCipher();
    if (!EVP_EncryptInit_ex(pImpl->encryptCtx, cipher, NULL, pImpl->key, NULL)) {
        throw std::runtime_error("Encrypt init failed");
    }
    if (!EVP_DecryptInit_ex(pImpl->decryptCtx, cipher, NULL, pImpl->key, NULL)) {
        throw std::runtime_error("Decrypt init failed");
    }
}

void EncryptionEngine::encryptBlock(uint64_t blockIndex, const uint8_t* plaintext, uint8_t* ciphertext) {
    uint8_t iv[16];
    pImpl->generateIV(blockIndex, iv);
    
    int outLen;
    if (!EVP_EncryptInit_ex(pImpl->encryptCtx, NULL, NULL, NULL, iv) ||
        !EVP_EncryptUpdate(pImpl->encryptCtx, ciphertext, &outLen, plaintext, BLOCK_SIZE)) {
        throw std::runtime_error("Encryption failed");
    }
}

void EncryptionEngine::decryptBlock(uint64_t blockIndex, const uint8_t* ciphertext, uint8_t* plaintext) {
    uint8_t iv[16];
    pImpl->generateIV(blockIndex, iv);
    
    int outLen;
    if (!EVP_DecryptInit_ex(pImpl->decryptCtx, NULL, NULL, NULL, iv) ||
        !EVP_DecryptUpdate(pImpl->decryptCtx, plaintext, &outLen, ciphertext, BLOCK_SIZE)) {
        throw std::runtime_error("Decryption failed");
    }
}