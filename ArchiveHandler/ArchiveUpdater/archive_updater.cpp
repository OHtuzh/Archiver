#include "archive_updater.h"
#include "../FileBitReader/file_bit_reader.h"

std::vector<std::string> ArchiveUpdater::GetAllFiles(const std::string& archive_name) {
    std::ifstream input(archive_name, std::ios::in | std::ios::binary);

    FileHeader* header = CreateFileHeader(input);
    std::vector<std::string> result;
    while (header != nullptr) {
        result.emplace_back(header->file_name);

        uint64_t number_of_bits = 1 << static_cast<uint64_t>(header->number_of_parity_bits);
        uint64_t number_of_bytes = number_of_bits >> 3;
        uint64_t important_bits = number_of_bits - header->number_of_parity_bits - 1;
        input.seekg((header->file_size + important_bits - 1) / important_bits * number_of_bytes, std::ios::cur);

        delete header;
        header = CreateFileHeader(input);
    }

    return result;
}

void ArchiveUpdater::RemoveFile(const std::string& archive_path, const std::string& file_name) {
    std::ofstream output(archive_path + ".tmp", std::ios::out | std::ios::binary);
    std::ifstream input(archive_path, std::ios::in | std::ios::binary);

    uint64_t start = 0;
    uint64_t end = 0;
    FileHeader* header = CreateFileHeader(input);
    bool found = false;
    while (header != nullptr) {
        uint64_t number_of_bits = 1 << static_cast<uint64_t>(header->number_of_parity_bits);
        uint64_t number_of_bytes = number_of_bits >> 3;
        uint64_t important_bits = number_of_bits - header->number_of_parity_bits - 1;
        input.seekg((header->file_size + important_bits - 1) / important_bits * number_of_bytes, std::ios::cur);

        start = end;
        end = input.tellg();
        if (header->file_name == file_name) {
            found = true;
            break;
        }


        delete header;
        header = CreateFileHeader(input);
    }
    input.seekg(0, std::ios::beg);
    char c;
    uint64_t byte = -1;
    while (input.get(c)) {
        byte++;
        if (found && !(byte >= start && byte < end)) {
            output.write(&c, 1);
        }
    }
    input.close();
    output.close();
    std::remove(archive_path.c_str());
    std::rename((archive_path + ".tmp").c_str(), archive_path.c_str());
}
