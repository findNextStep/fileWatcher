#include <exeWatcher.hpp>
#include <iostream>

int main(int argc, char **argv) {
    std::string file_name;
    if(argc == 2) {
        file_name = argv[1];
    } else {
        return 1;
    }
    wmj::fileWatcher::exeWatcher ew(file_name, [file_name](bool if_open) {
        if(if_open) {
            std::cout << file_name << " launched" << std::endl;
        } else {
            std::cout << file_name << " close" << std::endl;
        }
    });
    ew.startWatch();
    ew.run();
    return 0;
}
