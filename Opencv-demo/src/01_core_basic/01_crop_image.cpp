// ============================================================
// 实验 04：图像裁剪 — 实现
// ============================================================
//
// 本文件包含"实验04"的核心逻辑：
//   basicCrop()          — Step 1: 基本裁剪 (Rect / Range)
//   viewVsClone()        — Step 2: 视图 vs 深拷贝的区别
//   validateAndCrop()    — Step 3: 边界校验
//   divideIntoPatches()  — Step 4: 分块 (patch)
//   runCropImageExperiment() — 实验总入口
// ============================================================

#include "01_crop_image.h"

// 公共库头文件
#include "common/mat_info.h"
#include "common/path_utils.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>   // std::min

using namespace std;
using namespace cv;


// ============================================================
// Step 1：基本裁剪
// OpenCV 裁剪有两种写法：
//
// 写法 1：使用 Range
//   Mat crop = img(Range(y1, y2), Range(x1, x2));
//   - y1, y2: start_row,end_row，（左闭右开）
//   - x1, x2: start_col,end_col，（左闭右开）
//
// 写法 2：使用 Rect
//   Rect roi(x, y, w, h);
//   Mat crop = img(roi);
//   - x, y: 左上角坐标
//   - w, h: 宽高
//
// 注意区别：
//   Range 用 (起始, 结束)，左闭右开 [start, end)
//   Rect  用 (x, y, width, height)，包含左上角，不包含右下角
//
//   Rect(150, 80, 180, 200)
//   因为: x 从 150 开始，到 330 结束，宽 = 330-150 = 180
//         y 从 80 开始，到 280 结束，高 = 280-80 = 200
//
// 坐标系：
//   OpenCV 使用左上角为原点
//   x 向右增大，y 向下增大
//   像素坐标 (x, y) 对应图像的 (col, row)
// ============================================================
static void basicCrop(const Mat& img, const string& outDir)
{
	cout << "\n--- Step 1: 基本裁剪 ---\n";
	cout << "  原图: " << img.cols << "x" << img.rows << "\n";

	// --- 方式 1：用 Rect 裁剪 ---
	Rect roi1(100, 80, 200, 150);
	Mat crop1 = img(roi1);  // 注意：这是视图，不是拷贝

	cout << "  Rect 裁剪 (" << roi1.x << ", " << roi1.y << ", " << roi1.width << ", " << roi1.height << "): "
		<< crop1.cols << "x" << crop1.rows << "\n";

	// --- 方式 2：用 Range 裁剪 ---
	Mat crop2 = img(Range(80, 230), Range(100, 300));

	cout << "  Range 裁剪: " << crop2.cols << "x" << crop2.rows << "\n";

	// --- 验证两种方式结果一致 ---
	// 比较像素值（应该完全相同）
	double diff = norm(crop1, crop2, NORM_L2);
	cout << "  两种方式差异: " << diff << " (应为 0)\n";

	// --- 显示 ---
	Mat imgWithRect = img.clone();
	rectangle(imgWithRect, roi1, Scalar(0, 255, 0), 2);

	imshow("Original with ROI", imgWithRect);
	imshow("Cropped (Rect)", crop1);
	imshow("Cropped (Range)", crop2);
	cout << "  按任意键继续...\n";
	waitKey(0);
	destroyAllWindows();

	// 保存裁剪结果
	imwrite(common::joinPath(outDir, "crop_rect.jpg"), crop1.clone());
	imwrite(common::joinPath(outDir, "crop_range.jpg"), crop2.clone());
}

// ============================================================
// Step 2：视图 vs 深拷贝
// 这是非常重要的概念：
//
// Mat crop = img(Rect);  // crop 和 img 共享像素数据！
//
// 修改 crop 会影响 img，反之亦然。
// 这是因为 Mat 本质上是"智能指针"，多个 Mat 可以指向同一块数据。
//
// 什么时候会出问题？
//   1. 裁剪后修改了 crop，结果原图也被改了
//   2. 原图释放后，crop 变成悬空引用
//   3. 小 crop 保持了大图内存不被释放（内存泄漏）
//
// 解决方案：使用 .clone() 创建独立拷贝
//   Mat crop = img(Rect).clone();  // 独立存储
// ============================================================
static void viewVsClone(const Mat& img, const string& outDir)
{
	cout << "\n--- Step 2: 视图 vs 深拷贝 ---\n";

	Rect roi(100, 80, 200, 150);

	// --- 1. 视图方式（共享数据）---
	Mat viewCrop = img(roi);

	// 修改（设为红色）
	viewCrop.setTo(Scalar(0, 0, 255));  
	cout << "  裁剪后内存占用: "
		<< common::formatFileSize(viewCrop.total() * viewCrop.elemSize())
		<< " (只有原图的 "
		<< static_cast<double>(viewCrop.total()) / img.total() * 100
		<< "%)\n";

	// --- 2. 深拷贝方式（独立数据）---
	Mat cloneCrop = img(roi).clone();  // 深拷贝

	// 修改（设为绿色）
	cloneCrop.setTo(Scalar(0, 255, 0));
	cout << "  深拷贝后内存占用: "
		<< common::formatFileSize(cloneCrop.total() * cloneCrop.elemSize())
		<< " (只有原图的 "
		<< static_cast<double>(cloneCrop.total()) / img.total() * 100
		<< "%)\n";

	// 恢复原图显示
	imshow("original ", img);
	imshow("viewCrop ", viewCrop);
	imshow("Clone (independent)", cloneCrop);
	cout << "  按任意键继续...\n";
	waitKey(0);
	destroyAllWindows();
}

// ============================================================
// Step 3：边界校验
// 裁剪前必须检查 ROI 是否在图像范围内
// 否则程序会崩溃或产生未定义行为
//
// 典型问题：
//   x + w > img.cols   → 超出右边界
//   y + h > img.rows   → 超出下边界
//   x < 0 或 y < 0     → 负坐标
//   w <= 0 或 h <= 0   → 无效尺寸
//
// 两种处理策略：
//   1. 严格校验：超出则报错（推荐调试阶段）
//   2. 自动裁剪：把 ROI 限制在图像范围内（运行时）
// ============================================================
static void validateAndCrop(const Mat& img, const string& outDir)
{
	cout << "\n--- Step 3: 边界校验 ---\n";
	cout << "  原图: " << img.cols << "x" << img.rows << "\n";

	// --- 测试 1：合法 ROI ---
	Rect validRoi(100, 50, 200, 150);
	bool valid = (validRoi.x >= 0 && validRoi.y >= 0) &&
		(validRoi.x + validRoi.width <= img.cols) &&
		(validRoi.y + validRoi.height <= img.rows);
	cout << "  ROI (" << validRoi.x << "," << validRoi.y
		<< ", " << validRoi.width << "x" << validRoi.height
		<< ") 合法? " << (valid ? "是" : "否") << "\n";
	if (valid)
	{
		Mat crop = img(validRoi).clone();
		cout << "    裁剪成功: " << crop.cols << "x" << crop.rows << "\n";
	}

	// --- 测试 2：超出边界的 ROI ---
	Rect badRoi(1000, 50, 600, 400);  // 宽度超出
	bool isOk = (badRoi.x >= 0 && badRoi.y >= 0) &&
		(badRoi.x + badRoi.width <= img.cols) &&
		(badRoi.y + badRoi.height <= img.rows);
	cout << "\n  ROI (" << badRoi.x << "," << badRoi.y
		<< ", " << badRoi.width << "x" << badRoi.height
		<< ") 合法? " << (isOk ? "是" : "否") << "\n";

	if (!isOk)
	{
		cout << "    → 超出边界！需要处理\n";

		// 策略 B：自动裁剪到有效范围（生产环境常用）
		Rect clipped;
		clipped.x = max(0, badRoi.x);
		clipped.y = max(0, badRoi.y);
		clipped.width = min(badRoi.x + badRoi.width, img.cols) - clipped.x;
		clipped.height = min(badRoi.y + badRoi.height, img.rows) - clipped.y;

		// 宽高不能小于0，小于0代表ROI完全在图像外面
		if (clipped.width <= 0 || clipped.height <= 0)
		{
			std::cout << "    → ROI完全超出图像范围，无有效区域\n";
			return;
		}

		cout << "    → 裁剪到有效范围: ("
			<< clipped.x << "," << clipped.y << ", "
			<< clipped.width << "x" << clipped.height << ")\n";

		Mat crop = img(clipped).clone();
		cout << "    → 裁剪成功: " << crop.cols << "x" << crop.rows << "\n";
	}

	// --- 测试 3：负坐标 ---
	Rect negRoi(-10, 50, 100, 100);
	bool negOk = (negRoi.x >= 0 && negRoi.y >= 0);
	cout << "\n  ROI (" << negRoi.x << "," << negRoi.y
		<< ", " << negRoi.width << "x" << negRoi.height
		<< ") 合法? " << (negOk ? "是" : "否")
		<< " (负坐标不合法)\n";
}

// ============================================================
// Step 4：分块 (Divide into Patches)
// 把大图分成若干个固定大小的小块
// 算法：
//   1. 定义 patch 大小 (M x N)
//   2. 用双重循环遍历图像
//   3. 对边缘不足一块的部分，用 min 裁剪
//
// 边缘处理策略（最后一个 patch 可能不足尺寸）：
//   策略 1: 保留小块（min 裁剪）
//   策略 2: 填充到完整尺寸（padding）
//   策略 3: 丢弃边缘
//   策略 4: 重叠（overlap）
// ============================================================
static void divideIntoPatches(const Mat& img, const string& outDir)
{
	cout << "\n--- Step 4: 分块 ---\n";
	cout << "  原图: " << img.cols << "x" << img.rows << "\n";

	// 定义 patch 大小
	const int patchW = 160;
	const int patchH = 140;
	cout << "  Patch 大小: " << patchW << "x" << patchH << "\n";

	// 计算 patch 数量
	int cols = (img.cols + patchW - 1) / patchW;  // ceil 除法
	int rows = (img.rows + patchH - 1) / patchH;
	int total = 0;

	cout << "  预计: " << cols << " 列 x " << rows
		<< " 行 = " << cols * rows << " 个 patch\n\n";

	// 创建可视化图（在原图上画出分割线）
	Mat visImg = img.clone();

	// 保存所有 patch
	vector<Mat> patches;

	for (int r = 0; r < rows; ++r)
	{
		for (int c = 0; c < cols; ++c)
		{
			// 计算当前 patch 的坐标
			int x = c * patchW;
			int y = r * patchH;

			// 边缘裁剪：处理最后一个 patch 可能不足尺寸
			int w = min(patchW, img.cols - x);
			int h = min(patchH, img.rows - y);

			Rect patchRoi(x, y, w, h);
			Mat patch = img(patchRoi).clone();

			patches.push_back(patch);
			total++;

			// 在可视化图上画矩形
			rectangle(visImg, patchRoi, Scalar(0, 255, 0), 1);

			// 打印最后一行的特殊处理
			if (w < patchW || h < patchH)
			{
				cout << "  Patch [" << r << "," << c << "]: ("
					<< x << "," << y << ", " << w << "x" << h
					<< ") ← 边缘裁剪\n";
			}
		}
	}

	cout << "\n  实际生成: " << total << " 个 patch\n";

	// 显示可视化结果
	imshow("Patches on original", visImg);

	// 显示前 4 个 patch
	if (!patches.empty())
	{
		Mat patchRow;
		hconcat(patches[0], patches[1], patchRow);
		imshow("Patch [0,0] | Patch [0,1]", patchRow);
	}
	if (patches.size() > 2)
	{
		Mat patchRow2;
		hconcat(patches[2], patches[3], patchRow2);
		imshow("Patch [0,2] | Patch [0,3]", patchRow2);
	}

	cout << "  按任意键继续...\n";
	waitKey(0);
	destroyAllWindows();

	// 保存 patch
	string patchDir = common::joinPath(outDir, "patches");
	common::createDirectories(patchDir);
	for (size_t i = 0; i < patches.size(); ++i)
	{
		// 按行列命名: tile_行_列.jpg
		int r = static_cast<int>(i / cols);
		int c = static_cast<int>(i % cols);
		string filename = "patch_" + to_string(r) + "_" + to_string(c) + ".jpg";
		imwrite(common::joinPath(patchDir, filename), patches[i]);
	}

	// 保存可视化图
	imwrite(common::joinPath(outDir, "patches_visual.jpg"), visImg);
	cout << "  Patch 已保存到: " << patchDir << "\n";
}

// ============================================================
// 实验总入口
// ============================================================
int demo_CropImage()
{
	cout << "========== 实验 04：图像裁剪 ==========\n\n";

	// --- 准备工作 ---
	const string outDir = common::outputDirectory();
	if (outDir.empty())
	{
		cerr << "[错误] 输出目录为空，请检查配置！\n";
		return -1;
	}

	bool success =common::createDirectories(outDir);
	if (!success)
	{
		cerr << "[错误] 创建输出目录失败: " << outDir << "\n";
		return -1;
	}

	// 生成测试图像
	const string imgDir = common::imageDirectory();
	if (imgDir.empty())
	{
		cerr << "[错误] 图像目录为空，请检查配置！\n";
		return -1;
	}

	const string imgPath = common::joinPath(imgDir, "test.jpg");

	Mat img_color = imread(imgPath, IMREAD_UNCHANGED);
	if (img_color.empty())
	{
		cerr << "[错误] 图像读取失败: " << imgPath << "\n";
		return -1;
	}

	common::printImageInfo("test image", img_color);

	// --- 依次执行各步骤 ---
	//basicCrop(img_color, outDir);          // Step 1: 基本裁剪
	//viewVsClone(img_color, outDir);        // Step 2: 视图 vs 深拷贝
	validateAndCrop(img_color, outDir);    // Step 3: 边界校验
	divideIntoPatches(img_color, outDir);  // Step 4: 分块

	cout << "\n========== 实验 04 完成 ==========\n";
	return 0;
}