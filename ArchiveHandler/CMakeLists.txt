add_subdirectory(FileBitReader)
add_subdirectory(FileBitWriter)



add_library(
        archive-handler
        ArchiveCreator/archive_creator.h
        ArchiveCreator/archive_creator.cpp
        ArchiveExtractor/archive_extractor.h
        ArchiveExtractor/archive_extractor.cpp
        ArchiveMerger/archive_merger.h
        ArchiveMerger/archive_merger.cpp
        ArchiveUpdater/archive_updater.h
        ArchiveUpdater/archive_updater.cpp
        FileHeader/file_header.h
        FileHeader/file_header.cpp
)

target_link_libraries(
        archive-handler
        PRIVATE
        file-bit-reader
        file-bit-writer
)

