#include "fileWatcher.hpp"
#include <iostream>
#include <thread>
#include <termio.h>

char getch() {
    // https://blog.csdn.net/gaopu12345/article/details/30467099
    struct termios tm;
    int fd = 0, ch;
    if(tcgetattr(fd, &tm) < 0) {
        return -1;
    }
    auto tm_old = tm;
    cfmakeraw(&tm);
    if(tcsetattr(fd, TCSANOW, &tm) < 0) {
        return -1;
    }
    ch = getchar();
    if(tcsetattr(fd, TCSANOW, &tm_old) < 0) {
        return -1;
    }
    return ch;
}

int main(int argc,char **argv) {
    if (argc < 2){
        std::cerr << "please give me the file name" << std::endl;
        return 1;
    }
    wmj::fileWatcher::fileWatcher fw(argv[1], []() {
        std::cout << "file change" << std::endl;
    });
    fw.startWatch();
    std::thread watch_t([&fw]() {
        fw.run();
    });
    char a;
    while(a = getch()) {
        if(a == 'q' || a > 127) {
            break;
        }
        if(a == 'o') {
            fw.startWatch();
        }
        if(a == 'p') {
            fw.stopWatch();
        }
    }
    fw.stopWatch();
    watch_t.join();
}
