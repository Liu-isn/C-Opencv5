#include "demo_utils.h"

// 公共库头文件
#include "common/path_utils.h"

using namespace std;
using namespace cv;

Mat generateTestImage(int width, int height)
{
    // 创建 8 位无符号 3 通道（CV_8UC3）黑色图像
    Mat image(height, width, CV_8UC3, Scalar(0, 0, 0));

    // 左半：BGR 渐变（B 通道从 255 递减，R 通道从 0 递增）
    // 用 at<Vec3b>(y, x) 逐像素访问：
    //   Vec3b = 3 个 uchar，下标 [0]=B, [1]=G, [2]=R（注意是 BGR 顺序！）
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width / 2; ++x)
        {
            image.at<cv::Vec3b>(y, x)[0] = static_cast<uchar>(255 - x * 255 / (width / 2)); // B
            image.at<cv::Vec3b>(y, x)[1] = 0;                                                 // G
            image.at<cv::Vec3b>(y, x)[2] = static_cast<uchar>(x * 255 / (width / 2));       // R
        }
    }
    // 右半：灰度垂直渐变（黑 -> 白），B=G=R 构成灰色
    for (int y = 0; y < height; ++y)
    {
        uchar gray = static_cast<uchar>(y * 255 / height);
        for (int x = width / 2; x < width; ++x)
        {
            image.at<cv::Vec3b>(y, x) = cv::Vec3b(gray, gray, gray);
        }
    }
    // 叠加几何图形（锐利边缘，用于观察边缘检测/压缩失真）
    cv::circle(image, cv::Point(width / 4, height / 2), 80, cv::Scalar(255, 255, 255), 2);
    cv::rectangle(image, cv::Point(width * 3 / 4 - 60, height / 2 - 60),
        cv::Point(width * 3 / 4 + 60, height / 2 + 60), cv::Scalar(0, 0, 0), 2);
    return image;
}

string ensureTestImage()
{
    const string imgDir = common:: imageDirectory();

	// 创建目录
    const bool success = common::createDirectories(imgDir); 
    if (!success)
    {
        std::cerr << "[错误] 创建目录失败: " << imgDir << "\n";
        return "";
	}

    const string imgPath = common::joinPath(imgDir, "test.jpg");
    if (!common::pathExists(imgPath))
    {
        // 未找到测试图像，自动生成
        if (!cv::imwrite(imgPath, generateTestImage(100,300)))
        {
            std::cerr << "[错误] 测试图像生成失败: " << imgPath << "\n";

            return "";
        }
    }
    return imgPath;
}
