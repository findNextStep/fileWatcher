#include <exeWatcher.hpp>
#include <sys/inotify.h>
#include <unistd.h>

namespace wmj {
namespace fileWatcher {
exeWatcher::exeWatcher(const std::string &fileName,
                       const std::function<void(bool)> &call_back):
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

bool exeWatcher::catchStop() {
    unsigned int mask = 0;
    while(mask == 0 && need_watching) {
        mask = get_read_state();
    }
    return (mask & IN_CLOSE);
}

bool exeWatcher::catchStart() {
    unsigned int mask = 0;
    while(mask == 0 && need_watching) {
        mask = get_read_state();
    }
    return (mask & IN_OPEN);
}


bool exeWatcher::watchOnce() {
    unsigned int mask = 0;
    while(mask == 0 && need_watching) {
        mask = get_read_state();
    }
    return (mask & IN_OPEN);
}
void exeWatcher::run() {
    while(need_watching) {
        bool result = watchOnce();
        if(need_watching) {
            this->call_back_func(result);
        }
    }
}

unsigned int exeWatcher::get_read_state() {
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
        if(mask) {
            delete[] a;
            return mask;
        }
    }
    delete[] a;
    return 0;

}
} // fileWatcher
} // wmj
