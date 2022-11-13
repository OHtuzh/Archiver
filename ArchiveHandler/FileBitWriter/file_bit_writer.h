#pragma once

#include <string>
#include <fstream>

class FileBitWriter {
 private:
    std::ofstream output_;
    uint64_t size_;
    uint64_t current_byte_ = 0;
    uint8_t current_bit_ = 0;
    uint8_t byte_ = 0;
 public:
    FileBitWriter(const std::string& filename, uint64_t size);
    bool WriteBit(bool bit);
};
