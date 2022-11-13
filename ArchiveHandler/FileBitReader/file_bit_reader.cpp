#include "file_bit_reader.h"
#include <iostream>

FileBitReader::FileBitReader(const std::string& file_path) {
    file_input_ = std::ifstream(file_path, std::ios::in | std::ios::binary);
}

bool FileBitReader::ReadBit(bool& dest) {
    current_++;
    if (current_ == 8) {
        buffer_ = static_cast<int8_t>(file_input_.get());
        if (file_input_.eof()) {
            return false;
        }
        current_ = 0;
    }
    dest = buffer_ >> (8 - current_ - 1) & 1;
    return true;
}

