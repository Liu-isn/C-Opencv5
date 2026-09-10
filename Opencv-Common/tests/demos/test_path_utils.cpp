#include "demos/test_api.h"

#include <iostream>
#include <cassert>
#include <fstream>
#include <filesystem>
#include "common/path_utils.h"

namespace demo
{
    // 辅助函数：清理测试生成的临时文件和文件夹
    void cleanupTempFiles()
    {
        std::error_code ec;
        std::filesystem::remove("temp_test_file.txt");
        std::filesystem::remove_all("test_dir");
        std::filesystem::remove_all("中en合并");
    }

    // 1. 测试 exeDirectory
    void test_exeDirectory()
    {
        std::cout << "[Test] 正在测试 exeDirectory...\n";
        std::string path = common::exeDirectory();

        /* 
            利用 assert（断言）：断言的意思是“我认为这里的结果一定是这个，
            如果不是，程序立刻报错中断”。它是新手写测试最好的帮手。
        */

        // 验证：不为空
        assert(!path.empty());

        // 验证：末尾不包含斜杠（除非是盘符根目录，如 C:\，一般 parent_path 不会带，但做个保险判断）
        if (!path.empty())
        {
            char last = path.back();
            assert(last != '\\' && last != '/');
        }
        std::cout << "-> exeDirectory 测试通过！路径: " << path << "\n\n";
    }

    // 2. 测试 imageDirectory & outputDirectory
    void test_subDirectories()
    {
        std::cout << "[Test] 正在测试 imageDirectory 和 outputDirectory...\n";
        std::string imgDir = common::imageDirectory();
        std::string outDir = common::outputDirectory();

        assert(!imgDir.empty());
        assert(!outDir.empty());

        // 预期应包含 data\img 和 output
        assert(imgDir.find("data") != std::string::npos);
        assert(outDir.find("output") != std::string::npos);

        std::cout << "-> imageDirectory: " << imgDir << "\n";
        std::cout << "-> outputDirectory: " << outDir << "\n";
        std::cout << "-> 子目录测试通过！\n\n";
    }

    // 3. 测试 joinPath
    void test_joinPath()
    {
        std::cout << "[Test] 正在测试 joinPath...\n";

        // 正确输入
        assert(common::joinPath("C:\\dir", "sub") == "C:\\dir\\sub");

        // 边界输入（带重复斜杠）
        assert(common::joinPath("C:\\dir\\", "sub") == "C:\\dir\\sub");

        // 错误/边界输入（dir 为空）
        assert(common::joinPath("", "sub") == "sub");

        std::cout << "-> joinPath 测试通过！\n\n";
    }

    // 4. 测试 pathExists
    void test_pathExists()
    {
        std::cout << "[Test] 正在测试 pathExists...\n";

        // 准备一个真实存在的临时文件
        std::string tempFile = "temp_test_file.txt";
        {
            std::ofstream outfile(tempFile);
            outfile << "test data";
        }

        // 正确输入：文件存在
        assert(common::pathExists(tempFile) == true);

        // 错误输入：文件不存在
        assert(common::pathExists("C:\\not_exist_file_99999.txt") == false);

        // 边界输入：空串
        assert(common::pathExists("") == false);

        // 清理
        std::filesystem::remove(tempFile);

        std::cout << "-> pathExists 测试通过！\n\n";
    }

    // 5. 测试 createDirectories
    void test_createDirectories()
    {
        std::cout << "[Test] 正在测试 createDirectories...\n";

        std::string normalDir = ".\\test_dir\\sub_dir\\deep";
        std::string mixedDir = ".\\中en合并\\sub_dir\\deep";

        // 打印出它的绝对路径，你直接复制这个路径去文件管理器就能找到
        std::cout << "绝对路径是: " << std::filesystem::absolute(normalDir) << "\n";

        // 正确输入：创建多级深层目录
        assert(common::createDirectories(normalDir) == true);
        assert(common::pathExists(normalDir) == true);

        // 错误/特殊输入：中英文混合路径创建
        assert(common::createDirectories(mixedDir) == true);
        assert(common::pathExists(mixedDir) == true);

        // 边界输入：连续调用两次相同的目录（测试已存在的容错）
        assert(common::createDirectories(normalDir) == true);

        // 边界输入：空串
        assert(common::createDirectories("") == false);

        std::cout << "-> createDirectories 测试通过！\n\n";
    }

    // 6. 测试 getFileSize
    void test_getFileSize()
    {
        std::cout << "[Test] 正在测试 getFileSize...\n";

        std::string tempFile = "temp_test_file.txt";
        const std::string content = "123456789012345"; // 15 字节
        {
            std::ofstream outfile(tempFile);
            outfile << content;
        }

        // 正确输入：获取真实文件大小
        unsigned long long size = common::getFileSize(tempFile);
        assert(size == 15);

        // 错误输入：文件不存在
        assert(common::getFileSize("C:\\not_exist_file.txt") == 0);

        // 边界输入：空串
        assert(common::getFileSize("") == 0);

        // 清理
        std::filesystem::remove(tempFile);

        std::cout << "-> getFileSize 测试通过！\n\n";
    }

    // 7. 测试 formatFileSize
    void test_formatFileSize()
    {
        std::cout << "[Test] 正在测试 formatFileSize...\n";

        // 边界输入：0 字节
        assert(common::formatFileSize(0) == "0 Bytes");

        // 正确输入：Bytes
        assert(common::formatFileSize(500) == "500 Bytes");

        // 正确输入：KB
        auto s500 = common::formatFileSize(1024);
        assert(common::formatFileSize(1024) == "1.00 KB");

        // 正确输入：MB
        assert(common::formatFileSize(5ULL * 1024 * 1024) == "5.00 MB");

        // 正确输入：GB
        assert(common::formatFileSize(2ULL * 1024 * 1024 * 1024) == "2.00 GB");

        std::cout << "-> formatFileSize 测试通过！\n\n";
    }

    int test_path_utils()
    {
        std::cout << "========== 开始执行 path_utils 单元测试 ==========\n\n";

        // 运行前清理环境
        cleanupTempFiles();

        // 依次执行每个单独的测试函数
        test_exeDirectory();
        test_subDirectories();
        test_joinPath();
        test_pathExists();
        test_createDirectories();
        test_getFileSize();
        test_formatFileSize();

        // 运行后清理环境
        cleanupTempFiles();

        std::cout << "==================================================\n";
        std::cout << "🎉 恭喜！所有 path_utils 单元测试用例全部通过！\n";
        
        std::cout << "按回车键退出..." << std::endl;
        std::cin.get();
        return 0;
    }
}