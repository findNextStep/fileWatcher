#include <functional>
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <mutex>


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
    /**
     * @brief initialized_wd 初始化监视文件列表
     * 这个函数会同时管理文件名最大值
     * 可以在调用后再追加
     * @param fileNames 需要监视的文件列表
     */
    void initialized_wd(const std::vector<std::string> &fileNames) {
        for(auto file_name : fileNames) {
            add_file(file_name);
        }
    }

    /**
     * @brief add_file 添加一个新的需要监视的文件
     *
     * @param fileName 需要监视的文件名
     *
     * @return 文件的监视号
     */
    int add_file(const std::string &fileName) {
        using std::max;
        const int wd = inotify_add_watch(
                           inotify_fd,
                           fileName.c_str(),
                           IN_CLOSE_WRITE | IN_MOVE_SELF |
                           IN_ATTRIB | IN_DELETE_SELF);
        this->wd_to_file_name.insert(std::make_pair(wd, fileName));
        if(wd > 0) {
            max_file_name_length = max<int>(
                                       max_file_name_length,
                                       fileName.size());
        }
        return wd;
    }

    /**
     * @brief remove_wd 停用对一个文件的监视
     *
     * @param wd 文件监视号
     */
    void remove_wd(int wd) {
        inotify_rm_watch(this->inotify_fd, wd);
        this->wd_to_file_name.erase(wd);
    }

    /**
     * @brief watch_once 执行一次文件监视
     *
     * @param result 用于填充的已更改文件列表
     */
    void watch_once(std::vector<std::string> &result) {
        const int information_length =
            10 * (sizeof(struct inotify_event) + max_file_name_length + 1);
        // 开辟用于接受inotify信息的空间
        std::unique_ptr<char[]> information{new char[information_length]};
        int num_read = read(inotify_fd, information.get(), information_length);
        for(char *p = information.get(); p < information.get() + num_read;) {
            struct inotify_event *event = (struct inotify_event *) p;
            // 检查是否是待追踪的文件
            if(wd_to_file_name.find(event->wd) != wd_to_file_name.end()) {
                std::string file_name = this->wd_to_file_name.at(event->wd);
                remove_wd(event->wd);
                add_file(file_name);
                // 避免已更改文件列表重复
                if(std::find(result.begin(), result.end(), file_name) == result.end()) {
                    result.push_back(file_name);
                }
            }
            p += sizeof(struct inotify_event) + event->len;
        }
    }
public:
    /**
     * @brief fileWatcherSync 文件监视器的构造函数
     *
     * @param fileNames 监视文件列表
     */
    fileWatcherSync(const std::vector<std::string> &fileNames):
        inotify_fd(inotify_init()), max_file_name_length(0),
        watch_thread([this]() {
        while(this->inotify_fd) {
            // 文件监视线程
            std::lock_guard<std::mutex>(this->change_file_list_mutex);
            watch_once(this->change_file_list);
        }
    }) {
        initialized_wd(fileNames);
    }

    ~fileWatcherSync() {
        for(auto pair : this->wd_to_file_name) {
            remove_wd(pair.first);
        }
        close(inotify_fd);
        // 停止文件监视线程
        inotify_fd = 0;
        if(watch_thread.joinable()) {
            watch_thread.join();
        }
    }

    /**
     * @brief checkFile 检查单个被监视文件是否被修改
     *
     * @param file_name 需要检查的文件
     *
     * @return 是否被修改
     */
    bool checkFile(const std::string &file_name) {
        std::lock_guard<std::mutex>(this->change_file_list_mutex);
        auto file = std::find(this->change_file_list.begin(), this->change_file_list.end(), file_name);
        if(file != change_file_list.end()) {
            this->change_file_list.erase(file);
            return true;
        }
        return false;
    }

    /**
     * @brief hasChange 获取所有被修改的文件列表
     * 调用后这个文件列表会重置
     * @return 被修改的文件列表
     */
    std::vector<std::string> hasChange() {
        std::lock_guard<std::mutex>(this->change_file_list_mutex);
        auto result = this->change_file_list;
        this->change_file_list.resize(0);
        return result;
    }
};
} // wmj
