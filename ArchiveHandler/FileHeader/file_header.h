#pragma once

#include <fstream>

struct FileHeader {
    uint8_t number_of_parity_bits = 0;
    uint16_t file_name_length = 0;
    char* file_name = nullptr;
    uint64_t file_size = 0;
};

FileHeader* CreateFileHeader(std::ifstream& input);

inline void EncodeFileHeaderByte(uint8_t& byte) {
    for (uint8_t i = 3; i < 8; i++) {
        if ((byte << i & 0x80) != 0 && i != 4) {
            for (uint8_t parity_bits = 1; parity_bits < 8; parity_bits <<= 1) {
                if ((i & parity_bits) != 0) {
                    byte ^= (1 << (8 - parity_bits - 1));
                }
            }
        }
    }
    for (uint8_t i = 1; i < 8; i++) {
        if ((byte << i & 0x80) != 0) {
            byte ^= 0x80;
        }
    }
}

inline uint64_t FileSize(const std::string& file_name) {
    std::ifstream file = std::ifstream(file_name);
    file.seekg(0, std::ios::end);
    uint64_t res = file.tellg();
    file.close();
    return res * 8;
}

void WriteFileHeader(std::ofstream& output, const std::string& file_path, uint8_t number_of_parity_bits);