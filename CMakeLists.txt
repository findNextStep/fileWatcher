cmake_minimum_required(VERSION 3.4)

project(fileWatcher)

include_directories(include)

# see https://cliutils.gitlab.io/modern-cmake/chapters/features/cpp11.html
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(file_watcher ./src/fileWatcher.cpp)
add_library(exe_watcher ./src/exeWatcher.cpp)

target_link_libraries(example_exeWatcher exe_watcher)
target_link_libraries(example_fileWatcher file_watcher pthread)

get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
    # 作为一个子目录，不产生测试用例以及相关测试用例的子模块加载
    # 但是产生声明相关链接库和头文件位置的变量
    get_property(include_dir DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
    set(FILE_WATCHER_INCLUDE ${include_dir} PARENT_SCOPE)
    set(FILE_WATCHER_LIB file_watcher exe_watcher PARENT_SCOPE)
else()
    add_compile_options(-std=c++14)
    add_compile_options(-g)
    # 搜索git模块
    find_package(Git QUIET)
    if(GIT_FOUND)
        message(STATUS "git found: ${GIT_EXECUTABLE}-v${GIT_VERSION_STRING}")
        message(STATUS "start submodule update")
        # 下载git子模块
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            # 下载失败的处理
            message(WARNING "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        else()
            # 没有父工程，且子模块更新完成，构建本工程的自动化测试用例。
            include_directories(3rdparty)

            add_executable(test_fileWatcher test/test_case/test_fileWatcher.cpp)
            target_link_libraries(test_fileWatcher file_watcher pthread)

            add_executable(test_fileWatcherSync test/test_case/test_fileWatcherSync.cpp)
            target_link_libraries(test_fileWatcherSync pthread)

            # 构建测试程序
            add_executable(example_fileWatcher ./test/example_fileWatcher.cpp)
            add_executable(example_exeWatcher ./test/example_exeWatcher.cpp)

        endif()
    else()
        # git no found
        message(WARNING "require git for test framework")
    endif()
endif()