#include <iostream>

#include "Program-Argument-Parser/program_arguments.h"
#include "ArchiveHandler/ArchiveCreator/archive_creator.h"
#include "ArchiveHandler/ArchiveExtractor/archive_extractor.h"
#include "ArchiveHandler/ArchiveMerger/archive_merger.h"
#include "ArchiveHandler/ArchiveUpdater/archive_updater.h"

int main(int argc, char* argv[]) {
    program_arguments::Parser parser(argc, argv);
    if (parser.CheckFlag("-c", "--create")) {
        ArchiveCreator creator;
        creator.CreateArchive(parser.GetArgument("-f", "--file"),
                              parser.GetAllFreeArguments(),
                              std::stoi(parser.GetArgument("-p", "--parity")));
    } else if (parser.CheckFlag("-l", "--list")) {
        for (auto& v : ArchiveUpdater::GetAllFiles(parser.GetArgument("-f", "--files"))) {
            std::cout << v << std::endl;
        }
    } else if (parser.CheckFlag("-a", "--append")) {
        ArchiveCreator creator;
        auto& files = parser.GetAllFreeArguments();
        if (files.empty()) {
            std::cerr << "There ain't no files" << std::endl;
            return -1;
        }
        creator.AddFile(parser.GetArgument("-f", "--file"),
                        files[0],
                        std::stoi(parser.GetArgument("-p", "--parity")));
    } else if (parser.CheckFlag("-d", "--delete")) {
        auto& files = parser.GetAllFreeArguments();
        if (files.empty()) {
            std::cerr << "There ain't no files" << std::endl;
            return -1;
        }
        ArchiveUpdater::RemoveFile(parser.GetArgument("-f", "--file"), files[0]);
    } else if (parser.CheckFlag("-A", "--concatenate")) {
        ArchiveMerger merger;
        merger.MergeArchives(parser.GetArgument("-f", "--file"), parser.GetAllFreeArguments());
    } else if (parser.CheckFlag("-x", "--extract")) {
        ArchiveExtractor extractor;
        auto& files = parser.GetAllFreeArguments();
        if (files.empty()) {
            extractor.ExtractAllFiles(parser.GetArgument("-f", "--file"));
        } else {
            extractor.ExtractFiles(parser.GetArgument("-f", "--file"), files);
        }
    }

    return 0;
}
