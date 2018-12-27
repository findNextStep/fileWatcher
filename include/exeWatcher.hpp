#pragma once
#include <string>
#include <functional>

namespace wmj {
namespace fileWatcher {
class exeWatcher {
public:
    /**
     * @brief exeWatcher 文件监视类的构造函数
     *
     * @param fileName 被监视的文件名称
     * @param call_back 发现文件更改后的回调函数
     */
    exeWatcher(const std::string &fileName, const std::function<void(bool)> &call_back);
    ~exeWatcher();

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
     * 会在程序启动的时候返回ture
     * 在程序关闭，或者这个类的析构函数被调用的时候返回false
     * @return 文件是否发生修改
     */
    bool watchOnce();
    /**
     * @brief catchStart 截获程序开始
     *
     * @return 是否截获到程序开始
     */
    bool catchStart();
    /**
     * @brief catchStop 截获程序结束
     *
     * @return 是否截获程序结束
     */
    bool catchStop();
    /**
     * @brief run 执行监视循环
     * 执行后阻塞线程，如果发生被监视文件修改，调用回调函数
     */
    void run();
protected:
    unsigned int get_read_state();
private:
    const int inotify_fd;
    const std::string file_name;
    bool need_watching;
    int wd;
    std::function<void(bool)>call_back_func;
};
} // fileWatcher
} // wmj
