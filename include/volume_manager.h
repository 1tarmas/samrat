// volume_manager.h
#pragma once
#include "encryption_engine.h"
#include <string>
#include <fstream>
#include <vector>

class VolumeManager {
public:
    struct VolumeHeader {
        char magic[8] = {'V','C','R','Y','P','T','1','0'};
        uint32_t version = 1;
        uint64_t dataSize;
        uint8_t salt[64];
        uint8_t headerKey[64];
        uint32_t iterations = 500000;
        uint16_t algo;
        uint8_t reserved[454] = {0};
        uint8_t hmac[64];
    };
    
    bool createVolume(const std::string& path, uint64_t size, 
                    EncryptionEngine::Algorithm algo, const std::string& password);
    bool mountVolume(const std::string& path, const std::string& password);
    void dismountVolume();
    
    ssize_t read(uint64_t offset, void* buffer, size_t size);
    ssize_t write(uint64_t offset, const void* buffer, size_t size);
    
private:
    void deriveKey(const std::string& password, const uint8_t* salt, 
                 uint32_t iterations, uint8_t* key, size_t keySize);
    bool verifyHeader(const VolumeHeader& header, const uint8_t* key);
    void secureWipe(void* data, size_t size);
    
    std::fstream volumeFile;
    std::unique_ptr<EncryptionEngine> engine;
    VolumeHeader currentHeader;
    uint64_t dataOffset = sizeof(VolumeHeader);
};