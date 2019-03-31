#include "fileWatcherSync.hpp"

#include <bandit/bandit.h>
#include <fstream>
#include <chrono>


go_bandit([]() {
    // 创建测试使用的文件
    const std::string file_name = ".test.txt";
    {
        std::ofstream of(file_name);
        of << "some txt" << std::endl;
        of.close();
    }
    wmj::fileWatcherSync fws({file_name});
    auto run_case = [&]() {
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
        bandit::it("vim写入", [&]() {
            system(("vim +wq " + file_name).c_str());
        });
        bandit::describe("多次写入", [&]() {
            bandit::it("vim写入+常规写入", [&]() {
                system(("vim +wq " + file_name).c_str());

                std::ofstream of(file_name);
                of << "some txt" << std::endl;
            });
        });
        bandit::it("常规写入", [&]() {
            // 确定可以继续跟踪
            std::ofstream of(file_name);
            of << "some txt" << std::endl;
        });
    };
    bandit::describe("写入测试 - checkFile 函数测试", [&]() {
        bandit::after_each([&]() {
            // 等待监视线程
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            AssertThat(fws.checkFile(file_name), snowhouse::Is().EqualTo(true));
            // 调用之后应当清除列表
            AssertThat(fws.checkFile(file_name), snowhouse::Is().EqualTo(false));
        });
        run_case();
    });
    bandit::describe("写入测试 - hasChange 函数测试", [&]() {
        bandit::after_each([&]() {
            // 等待监视线程
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            AssertThat(fws.hasChange().size(), snowhouse::Is().EqualTo(1));
            // 调用之后应当清除列表
            AssertThat(fws.hasChange().size(), snowhouse::Is().EqualTo(0));
        });
        run_case();
    });
});

int main(int argc, char *argv[]) {
    return bandit::run(argc, argv);
}
