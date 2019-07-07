cmake_minimum_required(VERSION 3.4)

project(fileWatcher)

find_package(Git QUIET)

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

message(STATUS "Submodule update")
execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT)
if(NOT GIT_SUBMOD_RESULT EQUAL "0")
    message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
else()
    include_directories(3rdparty)
    add_executable(test_fileWatcher test/test_case/test_fileWatcher.cpp)
    target_link_libraries(test_fileWatcher file_watcher pthread)
endif()