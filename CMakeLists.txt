cmake_minimum_required(VERSION 3.10)

project(fileWatcher)

include_directories(./include)

add_compile_options(-Wall)
add_compile_options(-std=c++11)
add_compile_options(-g)

add_library(file_watcher ./src/fileWatcher.cpp)

add_executable(example_fileWatcher ./test/example_fileWatcher.cpp)

target_link_libraries(example_fileWatcher file_watcher pthread)

get_property(include_dir DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
set(FILE_WATCHER_INCLUDE ${include_dir} PARENT_SCOPE)
