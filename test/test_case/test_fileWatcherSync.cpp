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
    bandit::describe("写入测试", [file_name]() {
        wmj::fileWatcherSync fws({file_name});
        bandit::after_each([&]() {
            // 等待监视线程
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            std::vector<std::string> file_list = fws.hasChange();
            AssertThat(file_list.size(), snowhouse::Is().EqualTo(1));
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
    });
});

int main(int argc, char *argv[]) {
    return bandit::run(argc, argv);
}
