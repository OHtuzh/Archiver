#pragma once

#include "../FileHeader/file_header.h"

#include <vector>
#include <string>

class ArchiveUpdater {
 public:
    static std::vector<std::string> GetAllFiles(const std::string& archive_name);
    static void RemoveFile(const std::string& archive_name, const std::string& file_path);
};
