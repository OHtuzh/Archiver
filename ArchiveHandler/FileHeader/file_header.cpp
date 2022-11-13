#include "file_header.h"
#include "../ArchiveExtractor/archive_extractor.h"

#include <filesystem>

void FileHeaderByteCorrection(uint8_t& byte) {
    uint8_t ones_positions_xor = 0;
    bool total_xor = false;

    for (uint8_t i = 0; i < 8; i++) {
        bool current_bit_value = (byte >> (8 - i - 1)) & 1;
        total_xor ^= current_bit_value;
        ones_positions_xor ^= (current_bit_value ? i : 0);
    }

    if (ones_positions_xor == 0 && total_xor == 0) {
        return;
    }
    if (total_xor == 1) {
        byte ^= (1 << (8 - ones_positions_xor - 1));
        return;
    }

    throw broken_archive_exception("Archive is too damaged");
}

FileHeader* CreateFileHeader(std::ifstream& input) {
    uint8_t encoded_number_of_parity_bits[2];
    if (!input.read(reinterpret_cast<char*>(encoded_number_of_parity_bits), 2)) {
        return nullptr;
    }

    auto* header = new FileHeader;

    FileHeaderByteCorrection(encoded_number_of_parity_bits[0]);
    FileHeaderByteCorrection(encoded_number_of_parity_bits[1]);
    for (int i = 0; i < 16; i++) {
        int position = i % 8;
        if (position != 0 && position != 1 && (position & (position - 1)) != 0) {
            header->number_of_parity_bits =
                (header->number_of_parity_bits << 1)
                    | (encoded_number_of_parity_bits[i / 8] >> (8 - position - 1) & 1);
        }
    }

    uint8_t encoded_file_name_length[4];
    input.read(reinterpret_cast<char*>(encoded_file_name_length), 4);
    for (uint8_t& i : encoded_file_name_length) {
        FileHeaderByteCorrection(i);
    }
    for (int i = 0; i < 32; i++) {
        int position = i % 8;
        if (position != 0 && position != 1 && (position & (position - 1)) != 0) {
            header->file_name_length =
                (header->file_name_length << 1) | (encoded_file_name_length[i / 8] >> (8 - position - 1) & 1);
        }
    }

    header->file_name = new char[header->file_name_length + 1]{}; // TODO DESTRUCTOR
    uint8_t encoded_file_name[header->file_name_length * 2];
    input.read(reinterpret_cast<char*>(encoded_file_name), header->file_name_length * 2);
    for (int i = 0; i < header->file_name_length * 2; i++) {
        FileHeaderByteCorrection(encoded_file_name[i]);
    }
    for (int i = 0; i < header->file_name_length * 2 * 8; i++) {
        int position = i % 8;
        if (position != 0 && position != 1 && (position & (position - 1)) != 0) {
            header->file_name[i / 16] =
                (header->file_name[i / 16] << 1) | (encoded_file_name[i / 8] >> (8 - position - 1) & 1);
        }
    }

    uint8_t encoded_file_size[16];
    input.read(reinterpret_cast<char*>(encoded_file_size), 16);
    for (uint8_t& i : encoded_file_size) {
        FileHeaderByteCorrection(i);
    }

    for (int i = 0; i < 16 * 8; i++) {
        int position = i % 8;
        if (position != 0 && position != 1 && (position & (position - 1)) != 0) {
            header->file_size = (header->file_size << 1) | (encoded_file_size[i / 8] >> (8 - position - 1) & 1);
        }
    }

    return header;
}

void WriteFileHeader(std::ofstream& output, const std::string& file_path, uint8_t number_of_parity_bits) {
    // Writes header for file in 8bit word_length
    uint8_t byte = 0;

    for (int i = 0, j = 0; j < 8; i++) {
        int position = i % 8;
        if (position != 0 && position != 1 && (position & (position - 1)) != 0) {
            byte |= (number_of_parity_bits >> (8 - j - 1) & 1) << (8 - position - 1);
            j++;
            if (++position == 8) {
                EncodeFileHeaderByte(byte);
                output.write(reinterpret_cast<const char*>(&byte), 1);
                byte = 0;
            }
        }
    }

    std::string file_name = std::filesystem::path(file_path).filename().string();
    uint16_t file_name_size = file_name.length();
    for (int i = 0, j = 0; j < 16; i++) {
        int position = i % 8;
        if (position != 0 && position != 1 && (position & (i - 1)) != 0) {
            byte |= (file_name_size >> (16 - j - 1) & 1) << (8 - position - 1);
            j++;
            if (++position == 8) {
                EncodeFileHeaderByte(byte);
                output.write(reinterpret_cast<const char*>(&byte), 1);
                byte = 0;
            }
        }
    }
    for (char c : file_name) {
        for (int i = 0, j = 0; j < 8; i++) {
            int position = i % 8;
            if (position != 0 && position != 1 && (position & (position - 1)) != 0) {
                byte |= (c >> (8 - j - 1) & 1) << (8 - position - 1);
                j++;
                if (++position == 8) {
                    EncodeFileHeaderByte(byte);
                    output.write(reinterpret_cast<const char*>(&byte), 1);
                    byte = 0;
                }
            }
        }
    }
    uint64_t file_size = FileSize(file_path);
    for (int i = 0, j = 0; j < 64; i++) {
        int position = i % 8;
        if (position != 0 && position != 1 && (position & (position - 1)) != 0) {
            byte |= (file_size >> (64 - j - 1) & 1) << (8 - position - 1);
            j++;
            if (++position == 8) {
                EncodeFileHeaderByte(byte);
                output.write(reinterpret_cast<const char*>(&byte), 1);
                byte = 0;
            }
        }
    }
}

