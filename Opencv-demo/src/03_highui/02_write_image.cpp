// ============================================================
// 实验 02：保存图像 (cv::imwrite)
//
//   saveAsJpg()       — Step 1: 以不同 JPEG 质量保存
//   saveAsPng()       — Step 2: 以不同 PNG 压缩级别保存
//   saveAsBmp()       — Step 3: 保存为无压缩 BMP
// ============================================================


#include "02_write_image.h"
#include "src/common/demo_utils.h"

// 公共库头文件
#include "common/mat_info.h"
#include "common/path_utils.h"

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <chrono>

using namespace std;
using namespace cv;

// ============================================================
// Step 1：以不同 JPEG 质量保存（不会改变图像分辨率和通道数）
//
// imwrite 采用 JPEG 质量参数：
//   IMWRITE_JPEG_QUALITY (键值 1)
//   取值范围：0 ~ 100，默认 95
//   值越高 → 画质越好，文件越大
//   值越低 → 画质越差，文件越小
//
// 典型值：
//   50  - 低质量，文件很小，明显压缩伪影
//   75  - 中等质量，几乎看不出差异（默认）
//   95  - 高质量，接近无损
//   100 - 最高质量，文件较大
//
// JPEG 是有损压缩：
//   降低质量会不可逆地损失图像信息
//   每次保存都会累计损失（世代损失）
//   不适合反复编辑保存的场景
// ============================================================
static void saveAsJpg(const Mat& img, const string& outDir)
{
	cout << "\n--- Step 1: JPEG 格式保存 ---\n";

	// 测试 4 个质量等级
	const vector<int> qualities = { 50, 75, 95, 100 };

	for (int q : qualities)
	{
		// 构造输出文件名：output\test_q50.jpg 等
		const string filename = "test_q" + to_string(q) + ".jpg";
		const string outPath = common::joinPath(outDir, filename);

		// 构造参数：JPEG 质量
		// params 是 vector<int>，格式为 {键, 值}
		vector<int> params = { IMWRITE_JPEG_QUALITY, q };

		// imwrite 返回 bool：成功 true，失败 false
		const bool ok = imwrite(outPath, img, params);

		const unsigned long long size = common::getFileSize(outPath);

		if (!ok)
		{
			cerr << "  [失败] JPEG quality=" << q << "  保存失败: " << outPath << "\n";
		}
		else
		{
			cout << "  JPEG quality=" << q << "  ->  " << outPath
				<< "  (" << common::formatFileSize(size) << " )\n";

			// 读取保存的 JPEG 文件，验证是否能正常读回
			Mat jpgImg = imread(outPath, IMREAD_UNCHANGED);
			if (jpgImg.empty())
			{
				cerr << "  [失败] JPEG 读回失败: " << outPath << "\n";
			}
			else
				common::printImageInfo("  JPG 读回", jpgImg);
		}
	}
}

// ============================================================
// Step 2：以不同 PNG 压缩级别保存
// 
// PNG 压缩参数：
//   IMWRITE_PNG_COMPRESSION (键值 16)
//   取值范围：0 ~ 9，默认 3
//     0 = 无压缩（类似 BMP，文件大）
//     3 = 默认平衡
//     9 = 最大压缩（压缩慢，文件小）
//
//   IMWRITE_PNG_STRATEGY (键值 17)
//   取值：0~4，默认 0 (DEFAULT_STRATEGY)
//     0 = DEFAULT_STRATEGY  — 默认策略
//     1 = FILTERED          — 适合有大量渐变的图像
//     2 = HUFFMAN_ONLY      — 仅用霍夫曼编码
//     3 = RLE               — 适合简单图形
//     4 = FIXED             — 固定 Huffman 编码
//
// PNG 是无损压缩：
//   无论压缩级别多少，解码后像素值完全一致
//   压缩级别只影响文件大小和压缩耗时
//   支持透明通道（Alpha）
// ============================================================
static void saveAsPng(const Mat& img, const string& outDir)
{
	cout << "\n--- Step 2: PNG 格式保存 ---\n";

	// 测试 3 个压缩级别
	const vector<int> compressions = { 0, 3, 9 };

	for (int c : compressions)
	{
		const string filename = "test_c" + to_string(c) + ".png";
		const string outPath = common::joinPath(outDir, filename);

		vector<int> params = { IMWRITE_PNG_COMPRESSION, c };

		// 计时：观察压缩级别对耗时的影响
		const auto t0 = chrono::high_resolution_clock::now();
		const bool ok = imwrite(outPath, img, params);
		const auto t1 = chrono::high_resolution_clock::now();
		const double ms = chrono::duration<double, milli>(t1 - t0).count();

		const unsigned long long size = common::getFileSize(outPath);

		if (!ok)
		{
			cerr << "  [失败] PNG compression=" << c << "  保存失败: " << outPath << "\n";
		}
		else
		{
			cout << "  PNG compression=" << c << "  ->  " << outPath
				<< "  (" << common::formatFileSize(size) << ", " << ms << " ms)\n";

			// 读取保存的 PNG 文件，验证是否能正常读回
			Mat jpgImg = imread(outPath, IMREAD_UNCHANGED);
			if (jpgImg.empty())
			{
				cerr << "  [失败] PNG 读回失败: " << outPath << "\n";
			}
			else
			{
				common::printImageInfo("  PNG 读回", jpgImg);
			}
		}
	}
}

// ============================================================
// Step 3：保存为 BMP（无压缩）
// 
// BMP 是最简单的图像格式之一：
//   - 无压缩，文件 = 文件头 + 像素数据
//   - 文件头 54 字节（BITMAPFILEHEADER + BITMAPINFOHEADER）
//   - 像素数据按行存储，每行 4 字节对齐
//   - 不支持压缩、不支持透明通道
//
// BMP 的优缺点：
//   优点：读写快、结构简单、兼容性好
//   缺点：文件大、不支持透明、无压缩
// ============================================================
static void saveAsBmp(const Mat& img, const string& outDir)
{
	cout << "\n--- BMP 格式保存 ---\n";

	const string outPath = common::joinPath(outDir, "test.bmp");

	// BMP 不需要额外参数
	const bool ok = imwrite(outPath, img);
	const unsigned long long size = common::getFileSize(outPath);

	if (!ok)
	{
		cerr << "  [失败] BMP 保存失败: " << outPath << "\n";
	}
	else
	{
		cout << "  BMP  ->  " << outPath
			<< "  (" << common::formatFileSize(size) << " )\n";

		// 读取保存的 BMP 文件，验证是否能正常读回
		Mat jpgImg = imread(outPath, IMREAD_UNCHANGED);
		if (jpgImg.empty())
		{
			cerr << "  [失败] BMP 读回失败: " << outPath << "\n";
		}
		else
		{
			common::printImageInfo("  BMP 读回", jpgImg);
		}
	}
}


int demo_WriteImage()
{
	cout << "========== 实验 02：保存图像 (cv::imwrite) ==========\n\n";

	// 准备图像
	const string imgPath = ensureTestImage();
	if (imgPath.empty())
	{
		cerr << "[错误] 获取测试图像失败\n";
		return -1;
	}

	Mat testImg = imread(imgPath, IMREAD_UNCHANGED);
	if (testImg.empty())
	{
		cerr << "[错误] 图像读取失败: " << imgPath << "\n";
		return -1;
	}

	// 获取输出目录：<exe目录>\output
	const string outDir = common::outputDirectory();
	bool success = common::createDirectories(outDir);
	if (!success)
	{
		cerr << "[错误] 创建输出目录失败: " << outDir << "\n";
		return -1;
	}

	cout << "测试图像信息：\n";
	common::printImageInfo("test image", testImg);
	cout << "输出目录：" << outDir << "\n";

	// --- Step 1：JPEG 保存 ---
	saveAsJpg(testImg, outDir);

	// --- Step 2：PNG 保存 ---
	saveAsPng(testImg, outDir);

	// --- Step 3：BMP 保存 ---
	saveAsBmp(testImg, outDir);

	cout << "\n========== 实验 02 完成 ==========\n";
	return 0;
}