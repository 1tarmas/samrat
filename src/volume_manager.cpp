// volume_manager.cpp
#include "volume_manager.h"
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>
#include <stdexcept>
#include <algorithm>

bool VolumeManager::createVolume(const std::string& path, uint64_t size, 
                               EncryptionEngine::Algorithm algo, const std::string& password) {
    // Initialize random salt
    if (!RAND_bytes(currentHeader.salt, sizeof(currentHeader.salt))) {
        throw std::runtime_error("Failed to generate salt");
    }
    
    // Derive master key
    uint8_t masterKey[EncryptionEngine::KEY_SIZE];
    deriveKey(password, currentHeader.salt, currentHeader.iterations, 
             masterKey, sizeof(masterKey));
    
    // Initialize encryption engine
    engine = std::make_unique<EncryptionEngine>(algo);
    engine->setKey(std::vector<uint8_t>(masterKey, masterKey + sizeof(masterKey)));
    
    // Create file and write header
    volumeFile.open(path, std::ios::binary | std::ios::out | std::ios::trunc);
    if (!volumeFile) return false;
    
    // Write header
    currentHeader.algo = static_cast<uint16_t>(algo);
    currentHeader.dataSize = size;
    volumeFile.write(reinterpret_cast<char*>(&currentHeader), sizeof(currentHeader));
    
    // Fill with encrypted zeros
    std::vector<uint8_t> block(EncryptionEngine::BLOCK_SIZE, 0);
    std::vector<uint8_t> encryptedBlock(EncryptionEngine::BLOCK_SIZE);
    
    uint64_t numBlocks = (size + EncryptionEngine::BLOCK_SIZE - 1) / EncryptionEngine::BLOCK_SIZE;
    for (uint64_t i = 0; i < numBlocks; i++) {
        engine->encryptBlock(i, block.data(), encryptedBlock.data());
        volumeFile.write(reinterpret_cast<char*>(encryptedBlock.data()), encryptedBlock.size());
    }
    
    volumeFile.close();
    return true;
}

bool VolumeManager::mountVolume(const std::string& path, const std::string& password) {
    volumeFile.open(path, std::ios::binary | std::ios::in | std::ios::out);
    if (!volumeFile) return false;
    
    // Read header
    volumeFile.read(reinterpret_cast<char*>(&currentHeader), sizeof(currentHeader));
    if (memcmp(currentHeader.magic, "VCRYPT10", 8) != 0) {
        volumeFile.close();
        return false;
    }
    
    // Derive key and verify HMAC
    uint8_t masterKey[EncryptionEngine::KEY_SIZE];
    deriveKey(password, currentHeader.salt, currentHeader.iterations, 
             masterKey, sizeof(masterKey));
    
    if (!verifyHeader(currentHeader, masterKey)) {
        secureWipe(masterKey, sizeof(masterKey));
        volumeFile.close();
        return false;
    }
    
    // Initialize encryption engine
    engine = std::make_unique<EncryptionEngine>(
        static_cast<EncryptionEngine::Algorithm>(currentHeader.algo));
    engine->setKey(std::vector<uint8_t>(masterKey, masterKey + sizeof(masterKey)));
    
    secureWipe(masterKey, sizeof(masterKey));
    return true;
}

void VolumeManager::deriveKey(const std::string& password, const uint8_t* salt, 
                            uint32_t iterations, uint8_t* key, size_t keySize) {
    PKCS5_PBKDF2_HMAC(
        password.data(), password.size(),
        salt, sizeof(currentHeader.salt),
        iterations,
        EVP_sha512(),
        keySize, key
    );
    
    // Additional hardening
    for (int i = 0; i < 1000; i++) {
        HMAC(EVP_sha512(), key, keySize, 
            salt, sizeof(currentHeader.salt), 
            key, NULL);
    }
}