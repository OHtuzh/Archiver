#pragma once

#include <vector>
#include <string>

class ArchiveMerger {
 public:
    void MergeArchives(const std::string& result_archive_path, const std::vector<std::string>& archives_path);
};