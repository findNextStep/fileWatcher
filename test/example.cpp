#include "fileWatcher.hpp"
#include <iostream>
#include <thread>
#include <termio.h>
#include <memory>

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
    // 创建文件监视对象，设置回调函数
    auto fw = std::make_shared<wmj::fileWatcher::fileWatcher>(argv[1], []() {
        std::cout << "file change" << std::endl;
    });
    // 开始文件监视
    fw->startWatch();
    // 建立文件监视进程
    std::thread watch_t([&fw]() {
        fw->run();
    });
    char a;
    while(a = getch()) {
        // 按q结束程序
        if(a == 'q' || a > 127) {
            break;
        }
        // 按o开始文件监视
        if(a == 'o') {
            fw->startWatch();
        }
        // 按p结束文件监视
        if(a == 'p') {
            fw->stopWatch();
        }
    }
    // 释放资源停止run函数
    fw.reset();
    // 等待文件监视进程结束
    watch_t.join();
    return 0;
}
