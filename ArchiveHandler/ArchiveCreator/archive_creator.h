#pragma once

#include <vector>
#include <string>
#include <fstream>

class ArchiveCreator {
 private:
    std::ofstream output_;
    std::ifstream input_;
    uint64_t number_of_bits_;
    uint64_t number_of_bytes_;
    std::vector<bool> bits_;
    uint8_t* bytes_;
    uint64_t current_bit_;

    void Encode();

    inline void FlushData() {
        for (uint64_t i = 0; i < number_of_bytes_; i++) {
            for (uint64_t j = i * 8; j < i * 8 + 8; j++) {
                bytes_[i] = (bytes_[i] << 1) + bits_[j];
            }
        }
        output_.write(reinterpret_cast<char*>(bytes_), static_cast<std::streamsize>(number_of_bytes_));
    }

    inline void WriteBit(bool bit) {
        if ((current_bit_ & (current_bit_ - 1)) == 0) { // skipping parity bits
            bits_[current_bit_++] = false;
        }
        bits_[current_bit_++] = bit;
        if (current_bit_ == number_of_bits_) {
            Encode();
            FlushData();
            bits_[0] = bits_[1] = bits_[2] = false;
            current_bit_ = 3;
        }
    }

 public:
    void CreateArchive(const std::string& archive_name,
                       const std::vector<std::string>& files,
                       uint8_t number_of_parity_bits);

    void AddFile(const std::string& archive_name,
                 const std::string& file_path,
                 uint8_t number_of_parity_bits);
};

