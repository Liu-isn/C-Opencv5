// ============================================================
// 实验 01：读取图像 (cv::imread) 
// 
//   readImageModes()  —— Step 1: 三种模式读取 + empty() 检查
//   displayImages()   —— Step 2: namedWindow + imshow + waitKey
//   runReadImageExperiment() —— 实验流程编排
// ============================================================

#include "01_read_image.h"
#include "src/common/demo_utils.h"

// 公共库头文件
#include "common/mat_info.h"
#include "common/path_utils.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace cv;

// ============================================================
// Step 1：三种模式读取图像
//
// imread 三种常用 读取模式：
//   IMREAD_COLOR     —— 总是转成 3 通道 BGR，最常用（也是默认值）
//   IMREAD_GRAYSCALE —— 总是转成 1 通道灰度
//   IMREAD_UNCHANGED —— 保留原始通道数和位深（如 PNG 的 Alpha）
//
// 关键点（容易踩坑）：
//   imread 读不到文件【不会抛异常】，只返回一个空 Mat！
//   所以必须用 Mat::empty() 检查，
//   否则后面 imshow 会触发断言错误，掩盖真正原因（路径错/格式不支持）。
// ============================================================
static bool readImageModes(const string& path, Mat& color, Mat& grayscale, Mat& unchanged)
{
	color = imread(path, IMREAD_COLOR);
	grayscale = imread(path, IMREAD_GRAYSCALE);
	unchanged = imread(path, IMREAD_UNCHANGED);

	if (color.empty() || grayscale.empty() || unchanged.empty())
	{
		cerr << "[错误] 图像读取失败: " << path << "\n";
		return false;
	}
	return true;
}


// ============================================================
// Step 2：显示图像
// 显示三件套：imshow + waitKey + destroyAllWindows
//
//   namedWindow(name, WINDOW_AUTOSIZE) —— 创建窗口，
//       WINDOW_AUTOSIZE：窗口自动适应图像大小，不可手动缩放
// ============================================================
static void displayImages(const Mat& color, const Mat& grayscale, const Mat& unchanged)
{
	// 创建窗口
	namedWindow("color image", WINDOW_AUTOSIZE);
	namedWindow("grayscale image", WINDOW_AUTOSIZE);
	namedWindow("unchanged image", WINDOW_AUTOSIZE);

	// 显示图像
	imshow("color image", color);
	imshow("grayscale image", grayscale);
	imshow("unchanged image", unchanged);

	cout << "已显示 3 个窗口，按任意键继续...\n";

	// 等待按键（0 = 无限等待），然后销毁所有窗口
	waitKey(0);
	destroyAllWindows();//关闭全部窗口并释放内存
}

int demo_ReadImage()
{
	// --- 准备：common 库负责路径管理与测试图像生成 ---
	const string imgPath = ensureTestImage();
	if (imgPath.empty())
	{
		return -1;
	}

	// --- Step 1：三种模式读取 ---
	Mat img_color, img_grayscale, img_unchanged;
	if (!readImageModes(imgPath, img_color, img_grayscale, img_unchanged))
	{
		return -1;
	}

	// --- 观察三种模式返回的 Mat 差异（common 库提供打印工具） ---
	common::printImageInfo("IMREAD_COLOR", img_color);
	common::printImageInfo("IMREAD_GRAYSCALE", img_grayscale);
	common::printImageInfo("IMREAD_UNCHANGED", img_unchanged);

	// --- Step 2：显示（阻塞，按任意键继续） ---
	displayImages(img_color, img_grayscale, img_unchanged);

	return 0;
}
