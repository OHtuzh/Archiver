#pragma once

#include "../FileHeader/file_header.h"

#include <fstream>
#include <string>
#include <vector>

 class broken_archive_exception : public std::exception {
 private:
    std::string message_;
  public:
    explicit broken_archive_exception(const std::string& msg);
    const char * what() const noexcept override;
};

class ArchiveExtractor {
 private:
    std::ifstream input_;
    uint8_t* bytes_;
    uint64_t number_of_bits_;
    uint64_t number_of_bytes_;
    FileHeader* header_ = nullptr;

    void DataCorrection();
 public:
    void ExtractAllFiles(const std::string& archive_name);

    void ExtractFiles(const std::string& archive_name, const std::vector<std::string>& files);
};
