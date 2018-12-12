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
     * @brief watchOnce 进行一次监视
     * 这个函数会阻塞线程直至被监视文件被修改
     */
    void watchOnce();
    /**
     * @brief run 执行监视循环
     * 执行后阻塞线程，如果发生被监视文件修改，调用回调函数
     */
    void run();

private:
    const int inofity_fd;
    const std::string file_name;
    int wd;
    std::function<void(void)>call_back_func;
};

}
}
