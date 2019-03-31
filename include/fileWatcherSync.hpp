#include <functional>
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <mutex>

#include <iostream>

#include <cmath>
#include <algorithm>
#include <sys/inotify.h>
#include <unistd.h>
#include <memory>

namespace wmj {
class fileWatcherSync {
private:
    int inotify_fd;
    std::map<int, std::string> wd_to_file_name;
    unsigned int max_file_name_length;
    std::vector<std::string> change_file_list;
    std::mutex change_file_list_mutex;
    std::thread watch_thread;
protected:
    void initialized_wd(const std::vector<std::string> &fileNames) {
        using std::max;
        max_file_name_length = 0;
        for(auto file_name : fileNames) {
            const int wd = inotify_add_watch(
                               inotify_fd,
                               file_name.c_str(),
                               IN_CLOSE_WRITE | IN_MOVE_SELF |
                               IN_ATTRIB | IN_DELETE_SELF);
            this->wd_to_file_name.insert(std::make_pair(wd, file_name));
            max_file_name_length = max<int>(
                                       max_file_name_length,
                                       file_name.size());
        }
    }
    void watch_once(std::vector<std::string> &result)const {
        int information_length =
            10 * (sizeof(struct inotify_event) + max_file_name_length + 1);
        std::unique_ptr<char[]> information{new char[information_length]};
        int num_read = read(inotify_fd, information.get(), information_length);
        for(char *p = information.get(); p < information.get() + num_read;) {
            struct inotify_event *event = (struct inotify_event *) p;
            std::string file_name = this->wd_to_file_name.at(event->wd);
            if(std::find(result.begin(), result.end(), file_name) == result.end()) {
                result.push_back(file_name);
            }
            p += sizeof(struct inotify_event) + event->len;
        }
    }
public:
    fileWatcherSync(const std::vector<std::string> &fileNames):
        inotify_fd(inotify_init()),
        watch_thread([this]() {
        while(inotify_fd) {
            std::lock_guard<std::mutex>(this->change_file_list_mutex);
            watch_once(this->change_file_list);
        }
    }) {
        initialized_wd(fileNames);
    }
    ~fileWatcherSync() {
        int fd = this->inotify_fd;
        inotify_fd = 0;
        for (auto pair:this->wd_to_file_name){
            inotify_rm_watch(fd,pair.first);
        }
        close(fd);
        if(watch_thread.joinable()) {

            watch_thread.join();
        }
    }
    std::vector<std::string> hasChange() {
        std::lock_guard<std::mutex>(this->change_file_list_mutex);
        auto result = this->change_file_list;
        this->change_file_list.resize(0);
        return result;
    }
};
} // wmj
