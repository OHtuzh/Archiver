#include "file_bit_writer.h"

FileBitWriter::FileBitWriter(const std::string& filename, uint64_t size) {
    output_ = std::ofstream(filename, std::ios::out | std::ios::binary);
    size_ = size;
}

bool FileBitWriter::WriteBit(bool bit) {
    byte_ = (byte_ << 1) + bit;
    current_bit_++;

    if (current_bit_ == 8) {
        output_.write(reinterpret_cast<char*>(&byte_), 1);
        current_bit_ = 0;
        current_byte_++;
        if (current_byte_ == size_) {
            output_.close();
            return false;
        }
    }
    return true;
}
