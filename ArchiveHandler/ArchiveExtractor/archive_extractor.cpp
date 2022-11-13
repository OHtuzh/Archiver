#include "archive_extractor.h"
#include "../FileBitWriter/file_bit_writer.h"

#include <iostream>
#include <algorithm>

broken_archive_exception::broken_archive_exception(const std::string& msg) {
    message_ = msg;
}

const char* broken_archive_exception::what() const noexcept {
    return message_.c_str();
}

void ArchiveExtractor::DataCorrection() {
    uint64_t ones_positions_xor = 0;
    bool total_xor = false;

    for (uint64_t i = 0; i < number_of_bits_; i++) {
        bool current_bit_value = (bytes_[i / 8] >> (8 - i % 8 - 1)) & 1;
        total_xor ^= current_bit_value;
        ones_positions_xor ^= (current_bit_value ? i : 0);
    }

    if (ones_positions_xor == 0 && total_xor == 0) {
        return;
    }
    if (total_xor == 1) {
        bytes_[ones_positions_xor / 8] = bytes_[ones_positions_xor / 8] ^ (1 << (ones_positions_xor % 8));
        return;
    }
    throw broken_archive_exception("Archive is too damaged");
}

void ArchiveExtractor::ExtractAllFiles(const std::string& archive_name) {
    input_ = std::ifstream(archive_name, std::ios::in | std::ios::binary);

    header_ = CreateFileHeader(input_);
    while (header_ != nullptr) {

        number_of_bits_ = 1 << static_cast<uint64_t>(header_->number_of_parity_bits);
        number_of_bytes_ = number_of_bits_ >> 3;

        FileBitWriter bit_output(header_->file_name, header_->file_size >> 3);

        bytes_ = new uint8_t[number_of_bytes_];
        bool work = true;
        while (work) {
            input_.read(reinterpret_cast<char*>(bytes_), static_cast<std::streamsize>(number_of_bytes_));
            DataCorrection(); // TODO CHECK CORRECTION
            for (uint64_t i = 3; i < number_of_bits_; i++) {
                if ((i & (i - 1)) != 0) {
                    if (!bit_output.WriteBit(bytes_[i / 8] >> (8 - i % 8 - 1) & 1)) {
                        work = false;
                        break;
                    }
                }
            }
        }

        delete header_;
        delete[] bytes_;

        header_ = CreateFileHeader(input_);
    }
}

void ArchiveExtractor::ExtractFiles(const std::string& archive_name, const std::vector<std::string>& files) {
    input_ = std::ifstream(archive_name, std::ios::in | std::ios::binary);

    header_ = CreateFileHeader(input_);
    while (header_ != nullptr) {
        if (std::find(files.begin(), files.end(), header_->file_name) != files.end()) {
            number_of_bits_ = 1 << static_cast<uint64_t>(header_->number_of_parity_bits);
            number_of_bytes_ = number_of_bits_ >> 3;
            uint64_t important_bits = number_of_bits_ - header_->number_of_parity_bits - 1;

            input_.seekg((header_->file_size + important_bits - 1) / important_bits * number_of_bytes_, std::ios::cur);

            delete header_;
            header_ = nullptr;
            header_ = CreateFileHeader(input_);
            continue;
        }
        number_of_bits_ = 1 << static_cast<uint64_t>(header_->number_of_parity_bits);
        number_of_bytes_ = number_of_bits_ >> 3;

        FileBitWriter bit_output(header_->file_name, header_->file_size >> 3);

        bytes_ = new uint8_t[number_of_bytes_];
        bool work = true;
        while (work) {
            input_.read(reinterpret_cast<char*>(bytes_), static_cast<std::streamsize>(number_of_bytes_));
            DataCorrection();
            for (uint64_t i = 3; i < number_of_bits_; i++) {
                if ((i & (i - 1)) != 0) {
                    if (!bit_output.WriteBit(bytes_[i / 8] >> (8 - i % 8 - 1) & 1)) {
                        work = false;
                        break;
                    }
                }
            }
        }

        delete[] bytes_;
        delete header_;
        header_ = nullptr;
        header_ = CreateFileHeader(input_);
    }
}

