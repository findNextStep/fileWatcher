#pragma once
#include <functional>
#include <string>

namespace wmj {
namespace  fileWatcher {

/**
 * @brief 单个文件的监视器
 */
class fileWatcher {
public:
    /**
     * @brief fileWatcher 文件监视类的构造函数
     *
     * @param fileName 被监视的文件名称
     * @param call_back 发现文件更改后的回调函数
     */
    fileWatcher(const std::string &fileName, const std::function<void(void)> &call_back);
    ~fileWatcher();

    /**
     * @brief startWatch 开始监视
     *
     * @return 是否调用成功
     */
    bool startWatch();

    /**
     * @brief stopWatch 停止监视
     *
     * @return 是否调用成功
     */
    bool stopWatch();

    /**
     * @brief watchOnce 进行一次文件监视
     * 如果监视过程中发生异常，例如监视被另一个线程终止
     * 会返回false
     * @return 文件是否发生修改
     */
    bool watchOnce();
    /**
     * @brief run 执行监视循环
     * 执行后阻塞线程，如果发生被监视文件修改，调用回调函数
     */
    void run();

private:
    const int inotify_fd;
    const std::string file_name;
    bool need_watching;
    int wd;
    std::function<void(void)>call_back_func;
};

}
}
