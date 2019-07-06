cmake_minimum_required(VERSION 3.4)

project(fileWatcher)

include_directories(include)

set(CMAKE_CXX_STANDARD 14)
add_compile_options(-std=c++14)

add_library(file_watcher ./src/fileWatcher.cpp)

add_library(exe_watcher ./src/exeWatcher.cpp)

add_executable(example_fileWatcher ./test/example_fileWatcher.cpp)

add_executable(example_exeWatcher ./test/example_exeWatcher.cpp)

target_link_libraries(example_exeWatcher exe_watcher)

target_link_libraries(example_fileWatcher file_watcher pthread)

get_property(include_dir DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
set(FILE_WATCHER_INCLUDE ${include_dir} PARENT_SCOPE)