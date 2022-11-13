cmake_minimum_required(VERSION 3.23)
project(labwork_4_OHtuzh)

set(CMAKE_CXX_STANDARD 17)

add_executable(labwork_4_OHtuzh main.cpp)

add_subdirectory(Program-Argument-Parser)
add_subdirectory(ArchiveHandler)

target_link_libraries(labwork_4_OHtuzh PRIVATE archive-handler program-arguments)