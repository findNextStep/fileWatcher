#include <fileWatcher.hpp>
#include <sys/inotify.h>
#include <unistd.h>

namespace wmj {
namespace fileWatcher {

fileWatcher::fileWatcher(const std::string &fileName,
                         const std::function<void(void)> &call_back):
    inotify_fd(inotify_init()), file_name(fileName), need_watching(true) {
    this->call_back_func = call_back;
}

fileWatcher::~fileWatcher() {
    need_watching = false;
    startWatch();
    stopWatch();
}

bool fileWatcher::startWatch() {
    wd = inotify_add_watch(inotify_fd, file_name.c_str(), IN_CLOSE_WRITE | IN_ATTRIB);
    return true;
}

bool fileWatcher::stopWatch() {
    inotify_rm_watch(this->inotify_fd, this->wd);
    return true;
}

bool fileWatcher::watchOnce() {
    char *a = new char[(10 * (sizeof(struct inotify_event) + file_name.size() + 1))];
    auto readnum = read(inotify_fd, a, 100);
    if(readnum > 0) {
        for(auto c = a; c < a + readnum;) {
            auto event = (struct inotify_event *) c;
            {
                if((event->mask & IN_IGNORED) == 0) {
                    if(event->mask & IN_CLOSE_WRITE) {
                        delete a;
                        return true;
                    } else if(event->mask & IN_ATTRIB) {
                        // 一些编辑器(比如vim)使用文件覆盖的方法
                        // 写入文件，此时需要重新启动一次读
                        stopWatch();
                        startWatch();
                        delete a;
                        return true;
                    }
                }
            }
            c += sizeof(struct inotify_event) + event->len;
        }
    }
    delete a;
    return false;
}

void fileWatcher::run() {
    while(need_watching) {
        if(watchOnce()) {
            this->call_back_func();
        }
    }

}

}
}
