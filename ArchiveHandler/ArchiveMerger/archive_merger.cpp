#include "archive_merger.h"

#include <fstream>

 void ArchiveMerger::MergeArchives(const std::string& result_archive_path,
                                         const std::vector<std::string>& archives_path) {
    std::ofstream output(result_archive_path, std::ios::out | std::ios::binary);
    const uint16_t buffer_size = 8192;
    char buffer[buffer_size]{};

    for (auto& archive: archives_path) {
        std::ifstream input(archive, std::ios::in | std::ios::binary);
        while (input.read(buffer, buffer_size)) {
            output.write(buffer, buffer_size);
        }
        output.write(buffer, input.gcount());
        input.close();
    }
}

