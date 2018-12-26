#include <exeWatcher.hpp>
#include <iostream>

int main(int argc, char **argv) {
    std::string file_name;
    if(argc == 2) {
        file_name = argv[1];
    } else {
        return 1;
    }
    wmj::fileWatcher::exeWatcher ew(file_name, []() {});
    ew.startWatch();
    while(1) {
        if(ew.watchOnce()) {
            std::cout << "true" << std::endl;
        } else {
            std::cout << "false" << std::endl;
        }
    }
    return 0;
}
