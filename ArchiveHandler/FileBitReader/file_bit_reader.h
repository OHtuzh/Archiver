#pragma once

#include <fstream>

class FileBitReader {
 private:
    std::ifstream file_input_;
    int8_t buffer_{};
    int8_t current_{7};
 public:
    explicit FileBitReader(const std::string& file_path);

    bool ReadBit(bool& dest);
};
