project(fileWatcher)

include_directories(./include)

add_compile_options(-std=c++11)

add_library(file_watcher ./src/fileWatcher.cpp)

add_executable(example ./test/example.cpp)

target_link_libraries(example file_watcher pthread)
