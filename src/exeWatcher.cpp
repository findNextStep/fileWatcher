#include <exeWatcher.hpp>
#include <sys/inotify.h>
#include <unistd.h>

namespace wmj {
namespace fileWatcher {
exeWatcher::exeWatcher(const std::string &fileName,
                       const std::function<void(void)> &call_back):
    inotify_fd(inotify_init()), file_name(fileName), need_watching(true) {
    this->call_back_func = call_back;
}

exeWatcher::~exeWatcher() {
    need_watching = false;
    startWatch();
    stopWatch();
}
bool exeWatcher::startWatch() {
    wd = inotify_add_watch(inotify_fd, file_name.c_str(),
                           IN_CLOSE | IN_OPEN);
    return true;
}

bool exeWatcher::stopWatch() {
    inotify_rm_watch(this->inotify_fd, this->wd);
    return true;
}

bool exeWatcher::watchOnce() {
    char *a = new char[(10 * (sizeof(struct inotify_event) + file_name.size() + 1))];
    auto readnum = read(inotify_fd, a, 100);
    if(readnum > 0) {
        unsigned int mask  = 0;
        for(auto c = a; c < a + readnum;) {
            auto event = (struct inotify_event *) c;
            if(event->wd == this->wd) {
                mask |= event->mask;
            }
            c += sizeof(struct inotify_event) + event->len;
        }
        if (mask & IN_CLOSE){
            delete[] a;
            return true;
        }
    }
    delete[] a;
    return false;
}
void exeWatcher::run() {
    while(need_watching) {
        if(watchOnce()) {
            this->call_back_func();
        }
    }

}
} // fileWatcher
} // wmj
