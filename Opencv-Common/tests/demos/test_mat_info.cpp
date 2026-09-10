#include "demos/test_api.h"
#include <opencv2/opencv.hpp>
#include "common/mat_info.h"


namespace demo
{
    // 1.测试 常规彩色图像 (CV_8UC3)
    void test_ColorImage() 
    {
        std::cout << "[Test] 正在测试 ColorImage...\n";
        cv::Mat color_img(200, 100, CV_8UC3, cv::Scalar(0, 0, 0));

        bool no_exception = true;
        try {
            common::printImageInfo("M1_ColorImage", color_img);
        }
        catch (...) {
            no_exception = false;
        }

        // 使用 assert 判断是否顺利执行（没有崩溃或抛异常）
        assert(no_exception && "M1 Failed: Threw an exception!");
        std::cout << "-> ColorImage 测试通过！"  << "\n\n";
    }

    // 2.测试 单通道灰度图像 (CV_8UC1)
    void test_GrayscaleImage() 
    {
        std::cout << "[Test] 正在测试 GrayscaleImage...\n";
        cv::Mat gray_img(50, 50, CV_8UC1, cv::Scalar(128));

        bool no_exception = true;
        try {
            common::printImageInfo("M2_GrayImage", gray_img);
        }
        catch (...) {
            no_exception = false;
        }

        assert(no_exception && "M2 Failed: Threw an exception!");
        std::cout << "-> GrayscaleImage 测试通过！" << "\n\n";
    }

    // 3.测试 浮点型单通道图像 (CV_32FC1)
    void test_FloatImage() 
    {
        std::cout << "[Test] 正在测试 FloatImage...\n";
        cv::Mat float_img(30, 30, CV_32FC1, cv::Scalar(1.0f));

        bool no_exception = true;
        try {
            common::printImageInfo("M3_FloatImage", float_img);
        }
        catch (...) {
            no_exception = false;
        }

        assert(no_exception && "M3 Failed: Threw an exception!");
        std::cout << "-> FloatImage 测试通过！" << "\n\n";
    }

    // 4.测试 边界测试 - 空图像 (cv::Mat())
    void test_EmptyImage() 
    {
        std::cout << "[Test] 正在测试 EmptyImage...\n";
        cv::Mat empty_img; // 默认构造，为空矩阵

        bool no_exception = true;
        try {
            common::printImageInfo("M4_EmptyImage", empty_img);
        }
        catch (...) {
            no_exception = false;
        }

        assert(no_exception && "M4 Failed: Threw an exception!");
        std::cout << "-> EmptyImage 测试通过！" << "\n\n";
    }

    // 测试用例 1：测试常规的 3 通道彩色图像
    int test_mat_info()
    {
        std::cout << "========== 开始执行 mat_info 单元测试 ==========\n\n";

        // 依次执行每个单独的测试函数
        test_ColorImage();
        test_GrayscaleImage();
        test_FloatImage();
        test_EmptyImage();

        std::cout << "==================================================\n";
        std::cout << "🎉 恭喜！所有 mat_info 单元测试用例全部通过！\n";

        std::cout << "按回车键退出..." << std::endl;
        std::cin.get();
        return 0;
    }
}
