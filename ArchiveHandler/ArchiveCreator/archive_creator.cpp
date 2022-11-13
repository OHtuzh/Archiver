#include "archive_creator.h"
#include "../FileBitReader/file_bit_reader.h"
#include "../FileHeader/file_header.h"

#include <iostream>
#include <filesystem>

void ArchiveCreator::Encode() {
    for (uint64_t i = 3; i < number_of_bits_; i++) {
        if ((i & (i - 1)) != 0 && bits_[i] == true) { // skipping parity bits
            for (uint64_t current_parity_bit = 1; current_parity_bit < i; current_parity_bit <<= 1) {
                if ((i & current_parity_bit) != 0) {
                    bits_[current_parity_bit] = !bits_[current_parity_bit] & 1;
                }
            }
        }
    }
    for (uint64_t i = 1; i < number_of_bits_; i++) {
        bits_[0] = !bits_[0] & bits_[i] | bits_[0] & !bits_[i];
    }
}

void ArchiveCreator::CreateArchive(const std::string& archive_name,
                                   const std::vector<std::string>& files,
                                   uint8_t number_of_parity_bits) {
    output_ = std::ofstream(archive_name, std::ios::out | std::ios::binary);
    number_of_bits_ = (1 << static_cast<uint64_t>(number_of_parity_bits));
    number_of_bytes_ = number_of_bits_ >> 3;

    bits_ = std::vector<bool>(number_of_bits_, false);
    bytes_ = new uint8_t[number_of_bytes_];
    current_bit_ = 3;
    for (auto& file_path : files) {
        WriteFileHeader(output_, file_path, number_of_parity_bits);

        FileBitReader reader(file_path);
        bool bit;
        while (reader.ReadBit(bit)) {
            WriteBit(bit);
        }

        if (current_bit_ != 3) {
            for (uint64_t i = current_bit_; i < number_of_bits_; i++) {
                bits_[current_bit_] = false;
            }
            Encode();
            FlushData();
            bits_[0] = bits_[1] = bits_[2] = false;
            current_bit_ = 3;
        }
    }

    delete[] bytes_;
    output_.close();
}

void ArchiveCreator::AddFile(const std::string& archive_name,
                             const std::string& file_path,
                             uint8_t number_of_parity_bits) {
    output_ = std::ofstream(archive_name, std::ios::out | std::ios::app | std::ios::binary);

    number_of_bits_ = (1 << static_cast<uint64_t>(number_of_parity_bits));
    number_of_bytes_ = number_of_bits_ >> 3;

    bits_ = std::vector<bool>(number_of_bits_, false);
    bytes_ = new uint8_t[number_of_bytes_];
    current_bit_ = 3;

    WriteFileHeader(output_, file_path, number_of_parity_bits);

    FileBitReader reader(file_path);
    bool bit;
    while (reader.ReadBit(bit)) {
        WriteBit(bit);
    }

    if (current_bit_ != 3) {
        for (uint64_t i = current_bit_; i < number_of_bits_; i++) {
            bits_[i] = false;
        }
        Encode();
        FlushData();
    }

    delete[] bytes_;
    output_.close();
}



