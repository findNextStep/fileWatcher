#include <bandit/bandit.h>
#include <fstream>
#include <thread>
#include <chrono>
#include <memory>

#include "fileWatcher.hpp"

go_bandit([]() {
    // 创建测试使用的文件
    const std::string file_name = ".test.txt";
    {
        std::ofstream of(file_name);
        of << "some txt" << std::endl;
        of.close();
    }
    bandit::describe("写入测试", [file_name]() {
        bool has_change = false;
        auto exe_ptr = std::make_unique<wmj::fileWatcher::fileWatcher>(file_name, [&has_change]() {
            has_change = true;
        });
        exe_ptr->startWatch();
        std::thread run_thread([&exe_ptr]() {
            exe_ptr->run();
        });
        bandit::before_each([&]() {
            // 每一次测试前重置一次测试标记
            has_change = false;
        });
        bandit::after_each([&]() {
            // 等待监视线程
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            AssertThat(has_change, snowhouse::Is().EqualTo(true));
        });
        bandit::it("尾部写入", [&]() {
            // 使用文件流直接向文件尾写入
            std::ofstream of(file_name, std::ios::app);
            of << "some txt" << std::endl;
        });
        bandit::it("常规写入", [&]() {
            std::ofstream of(file_name);
            of << "some txt" << std::endl;
        });
        bandit::it("mv覆盖", [&]() {
            const std::string swp_file_name = file_name + ".swp";
            std::ofstream of(swp_file_name);
            of << "some txt" << std::endl;
            system(("mv " + swp_file_name + " " + file_name).c_str());
        });
        bandit::it("vim写入j",[&](){
            system(("vim +wq " + file_name).c_str());
        });
        exe_ptr.reset();
        if(run_thread.joinable()) {
            run_thread.join();
        }
    });
});

int main(int argc, char *argv[]) {
    return bandit::run(argc, argv);
}