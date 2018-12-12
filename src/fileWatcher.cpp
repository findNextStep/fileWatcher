#include <fileWatcher.hpp>
#include <sys/inotify.h>
#include <unistd.h>

namespace wmj {
namespace fileWatcher {

fileWatcher::fileWatcher(const std::string &fileName,
                         const std::function<void(void)> &call_back):
    inotify_fd(inotify_init()), file_name(fileName) {
    this->call_back_func = call_back;
}

bool fileWatcher::startWatch() {
    wd = inotify_add_watch(inotify_fd, file_name.c_str(), IN_CLOSE_WRITE);
    return true;
}

bool fileWatcher::stopWatch() {
    return true;
}

void fileWatcher::watchOnce() {
    char* a = new char[(10 * (sizeof(struct inotify_event) + file_name.size() + 1))];
    auto readnum = read(inotify_fd, a, 100);
    if(readnum > 0) {
        std::string str(a, readnum);
        for(auto c = a; c < a + readnum;) {
            auto event = (struct inotify_event *) c;
            {
                if(event->mask & IN_CLOSE_WRITE) {
                    return;
                }
            }
            c += sizeof(struct inotify_event) + event->len;
        }
    } else {
        return;
    }
    delete a;
}

void fileWatcher::run(){
    while (true){
        watchOnce();
        this->call_back_func();
    }

}

}
}
