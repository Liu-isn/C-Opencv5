// ============================================================
// 实验 03：图像缩放 (cv::resize) — 实现
// ============================================================
//
// 本文件包含"实验03"的核心逻辑：
//   resizeBySize()        — Step 1: 指定宽高缩放（会变形）
//   resizeByScale()       — Step 2: 按比例缩放（保持宽高比）
//   resizeByInterp()      — Step 3: 不同插值方法对比
//   resizeToFit()         — Step 4: 保持比例适配到指定框
//   letterbox()           — Step 5: Letterbox 填充（适合模型输入）
//   runResizeImageExperiment() — 实验总入口
// ============================================================

#include "01_resize_image.h"

// 公共库头文件
#include "common/mat_info.h"
#include "common/path_utils.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace cv;


// ============================================================
// Step 1：指定宽高缩放
// resize(src, dst, dsize, interpolation)
//   dsize = Size(width, height)  — 注意顺序：宽在前，高在后
//   输出图像的尺寸就是 dsize，不管原始比例
// ============================================================
static void resizeBySize(const Mat& img, const string& outDir)
{
	cout << "\n--- Step 1: 指定宽高缩放 ---\n";
	cout << "  原图: " << img.cols << " x " << img.rows << "\n";

	// 缩小到 300x200
	int w = 300, h = 200;
	Mat resizedDown;
	resize(img, resizedDown, Size(w, h), INTER_LINEAR);

	// 放大到 800x600
	int upW = 800, upH = 600;
	Mat resizedUp;
	resize(img, resizedUp, Size(upW, upH), INTER_LINEAR);

	// 显示
	cout << "  缩小: " << img.cols << "x" << img.rows
		<< " -> " << resizedDown.cols << "x" << resizedDown.rows << "\n";
	cout << "  放大: " << img.cols << "x" << img.rows
		<< " -> " << resizedUp.cols << "x" << resizedUp.rows << "\n";

	imshow("Original", img);
	imshow("Resized Down (300x200)", resizedDown);
	imshow("Resized Up (800x600)", resizedUp);
	cout << "  按任意键继续...\n";
	waitKey(0);
	destroyAllWindows();

	// 保存结果
	imwrite(common::joinPath(outDir, "resize_down.jpg"), resizedDown);
	imwrite(common::joinPath(outDir, "resize_up.jpg"), resizedUp);
}

// ============================================================
// Step 2：按比例缩放（保持宽高比）
// resize(src, dst, Size(), fx, fy, interpolation)
//   dsize = Size()  — 空尺寸，表示由 fx/fy 决定输出大小
//   fx  = 水平缩放因子
//   fy  = 垂直缩放因子
//
// 这是推荐的缩放方式：
//   1. 宽高比保持不变
//   2. 输出尺寸自动计算
//   3. 不会产生变形
// ============================================================
static void resizeByScale(const Mat& img, const string& outDir)
{
	cout << "\n--- Step 2: 按比例缩放 ---\n";
	cout << "  原图: " << img.cols << " x " << img.rows << "\n";

	// 缩小 0.5 倍
	double scaleDown = 0.5;
	Mat scaledDown;
	resize(img, scaledDown, Size(), scaleDown, scaleDown, INTER_LINEAR);

	// 放大 1.5 倍
	double scaleUp = 1.5;
	Mat scaledUp;
	resize(img, scaledUp, Size(), scaleUp, scaleUp, INTER_LINEAR);

	// 非等比缩放（故意变形，对比用）
	double fx = 1.0, fy = 0.5;
	Mat distorted;
	resize(img, distorted, Size(), fx, fy, INTER_LINEAR);

	cout << "  缩小 0.5x: " << img.cols << "x" << img.rows
		<< " -> " << scaledDown.cols << "x" << scaledDown.rows << "\n";
	cout << "  放大 1.5x: " << img.cols << "x" << img.rows
		<< " -> " << scaledUp.cols << "x" << scaledUp.rows << "\n";
	cout << "  变形 1.0x/0.5x: " << img.cols << "x" << img.rows
		<< " -> " << distorted.cols << "x" << distorted.rows << "\n";

	// 显示对比
	imshow("Scale 0.5x", scaledDown);
	imshow("Scale 1.5x", scaledUp);
	imshow("Distorted (fx=1.0, fy=0.5)", distorted);
	cout << "  按任意键继续...\n";
	waitKey(0);
	destroyAllWindows();

	// 保存
	imwrite(common::joinPath(outDir, "scale_0.5x.jpg"), scaledDown);
	imwrite(common::joinPath(outDir, "scale_1.5x.jpg"), scaledUp);
	imwrite(common::joinPath(outDir, "distorted.jpg"), distorted);
}

// ============================================================
// Step 3：不同插值方法对比
// 4 种常用插值方法：
//
// INTER_NEAREST (最近邻):						|	INTER_LINEAR (双线性):				|	INTER_CUBIC (双三次):					|	INTER_AREA (像素区域):
//   - 取最近的一个像素值						|		- 2x2 邻域加权平均				|		- 4x4 邻域加权平均（16 个像素）		|		- 基于像素区域关系重采样
//   - 速度最快，质量最差						|		- 速度较快，通用				|		- 4x4 邻域加权平均（16 个像素）		|		- 缩小时效果最好（避免 aliasing）
//   - 放大时有明显块状感（像素艺术风格）		|		- OpenCV 默认方法				|		- 放大效果比 LINEAR 好				|		- 放大时退化为 NEAREST
//   - 缩小时有 aliasing（混叠）				|		- 适合：自然图像的一般缩放		|		- 适合：需要高质量放大的场景		|		- 适合：缩小（降采样）的首选	
//   - 适合：二值图、标签图、像素艺术			|										|											|
//
// 经验法则：
//   缩小 → INTER_AREA
//   放大 → INTER_CUBIC 或 INTER_LINEAR
//   标签 → INTER_NEAREST
// ============================================================
static void resizeByInterp(const Mat& img, const string& outDir)
{
	cout << "\n--- Step 3: 不同插值方法对比 ---\n";

	const double scale = 0.3;   // 缩小到 30%，方便观察差异
	const int newW = static_cast<int>(img.cols * scale);
	const int newH = static_cast<int>(img.rows * scale);
	const Size dsize(newW, newH);

	// 4 种插值方法
	Mat resNearest, resLinear, resCubic, resArea;
	resize(img, resNearest, dsize, INTER_NEAREST);
	resize(img, resLinear, dsize, INTER_LINEAR);
	resize(img, resCubic, dsize, INTER_CUBIC);
	resize(img, resArea, dsize, INTER_AREA);

	// 打印信息
	cout << "  缩放后尺寸: " << newW << "x" << newH << "\n";
	cout << "  各方法效果对比（放大显示局部细节）：\n";

	// 为了清晰对比，把缩小的图再放大回原尺寸显示
	// 这样就能看到不同插值方法的"质量"差异
	Mat enlargeNearest, enlargeLinear, enlargeCubic, enlargeArea;
	const Size origSize(img.cols, img.rows);
	resize(resNearest, enlargeNearest, origSize, INTER_NEAREST);
	resize(resLinear, enlargeLinear, origSize, INTER_LINEAR);
	resize(resCubic, enlargeCubic, origSize, INTER_CUBIC);
	resize(resArea, enlargeArea, origSize, INTER_AREA);

	// 拼接显示：4 种方法横向排列
	Mat row1, row2, combined;
	hconcat(enlargeNearest, enlargeLinear, row1);
	hconcat(enlargeCubic, enlargeArea, row2);
	vconcat(row1, row2, combined);

	imshow("INTER_NEAREST  |  INTER_LINEAR\nINTER_CUBIC    |  INTER_AREA", combined);
	cout << "  左上: NEAREST  右上: LINEAR\n"
		<< "  左下: CUBIC    右下: AREA\n"
		<< "  观察：NEAREST 有块状感，CUBIC 最平滑\n"
		<< "  按任意键继续...\n";
	waitKey(0);
	destroyAllWindows();

	// 保存对比图
	imwrite(common::joinPath(outDir, "inter_nearest.jpg"), enlargeNearest);
	imwrite(common::joinPath(outDir, "inter_linear.jpg"), enlargeLinear);
	imwrite(common::joinPath(outDir, "inter_cubic.jpg"), enlargeCubic);
	imwrite(common::joinPath(outDir, "inter_area.jpg"), enlargeArea);
}

// ============================================================
// Step 4：保持比例适配到指定框
// 目标：把图像缩放到不超过 maxW x maxH 的框内
//       同时保持宽高比，不使其变形
//
// 算法：
//   scale = min(maxW / width, maxH / height)
//   newW  = round(scale * width)
//   newH  = round(scale * height)
//
// 选择 min 的原因：要同时满足宽和高的限制
//   如果 maxW/width < maxH/height，说明宽度是限制因素
//   按较小的 scale 缩放，两边都不会超出
//
// 示例：
//   原图 640x420，目标框 320x320
//   scale = min(320/640, 320/420) = min(0.5, 0.762) = 0.5
//   newW = 320, newH = 210
//   结果：320x210，完全在框内
// ============================================================
static void resizeToFit(const Mat& img, const string& outDir)
{
	cout << "\n--- Step 4: 保持比例适配 ---\n";
	cout << "  原图: " << img.cols << " x " << img.rows << "\n";

	const int maxW = 320;
	const int maxH = 320;
	cout << "  目标框: " << maxW << " x " << maxH << "\n";

	// 计算 scale：取宽高限制中的较小值
	double scaleW = static_cast<double>(maxW) / img.cols;
	double scaleH = static_cast<double>(maxH) / img.rows;
	double scale = min(scaleW, scaleH);

	// 计算新尺寸（四舍五入）
	int newW = static_cast<int>(scale * img.cols + 0.5);
	int newH = static_cast<int>(scale * img.rows + 0.5);

	// 选择插值方法
	// 缩小时用 INTER_AREA，放大时用 INTER_CUBIC
	int interp = (scale < 1.0) ? INTER_AREA : INTER_CUBIC;

	Mat resized;
	resize(img, resized, Size(newW, newH), 0, 0, interp);

	cout << "  scale = min(" << scaleW << ", " << scaleH << ") = " << scale << "\n";
	cout << "  结果: " << newW << "x" << newH << " (插值: "
		<< (interp == INTER_AREA ? "INTER_AREA" : "INTER_CUBIC") << ")\n";

	// 显示
	imshow("Original", img);
	imshow("Fitted to " + to_string(maxW) + "x" + to_string(maxH), resized);
	cout << "  按任意键继续...\n";
	waitKey(0);
	destroyAllWindows();

	// 保存
	imwrite(common::joinPath(outDir, "fit_320x320.jpg"), resized);
}

// ============================================================
// Step 5：Letterbox 填充
// 场景：很多深度学习模型需要固定尺寸输入（如 640x640）
//       但原图宽高比不同，直接 resize 会变形
//
// Letterbox 思路：
//   1. 按比例缩放到能放进目标框
//   2. 创建一个目标尺寸的画布（填充灰色/黑色）
//   3. 把缩放后的图居中放到画布上
//
// 示例：
//   原图 640x420，目标 640x640
//   scale = min(640/640, 640/420) = min(1.0, 1.52) = 1.0
//   缩放后: 640x420
//   偏移: offsetX=0, offsetY=(640-420)/2=110
//   在 640x640 的画布上，垂直居中放置
//
// 这样做的好处：
//   - 图像不变形
//   - 满足模型输入尺寸要求
//   - 坐标映射简单：新坐标 = 原坐标 * scale + offset
//
// YOLO 等目标检测模型都使用这种预处理
// ============================================================
static void letterbox(const Mat& img, const string& outDir)
{
	cout << "\n--- Step 5: Letterbox 填充 ---\n";
	cout << "  原图: " << img.cols << " x " << img.rows << "\n";

	const int targetW = 640;
	const int targetH = 640;
	cout << "  目标: " << targetW << " x " << targetH << "\n";

	// 1. 计算 scale
	double scaleW = static_cast<double>(targetW) / img.cols;
	double scaleH = static_cast<double>(targetH) / img.rows;
	double scale = min(scaleW, scaleH);

	// 2. 缩放图像
	int newW = static_cast<int>(scale * img.cols + 0.5);
	int newH = static_cast<int>(scale * img.rows + 0.5);

	Mat resized;
	int interp = (scale < 1.0) ? INTER_AREA : INTER_CUBIC;
	resize(img, resized, Size(newW, newH), 0, 0, interp);

	// 3. 创建画布（用灰色填充，OpenCV 常用 (128,128,128)）
	Scalar paddingColor(128, 128, 128);
	Mat canvas(targetH, targetW, img.type(), paddingColor);

	// 4. 计算偏移（居中）
	int offsetX = (targetW - newW) / 2;
	int offsetY = (targetH - newH) / 2;

	// 5. 把缩放后的图放到画布上
	// ROI: Region of Interest，感兴趣区域
	Mat roi = canvas(Rect(offsetX, offsetY, newW, newH));
	resized.copyTo(roi);

	cout << "  scale = " << scale << "\n";
	cout << "  缩放后: " << newW << "x" << newH << "\n";
	cout << "  偏移: offsetX=" << offsetX << ", offsetY=" << offsetY << "\n";
	cout << "  画布: " << canvas.cols << "x" << canvas.rows << "\n";

	// 验证 ROI 操作不影响原图
	// 坐标映射：新坐标 = 原坐标 * scale + offset
	// x_new = x * scale + offsetX
	// y_new = y * scale + offsetY

	// 显示
	imshow("Letterbox 640x640", canvas);
	cout << "  按任意键继续...\n";
	waitKey(0);
	destroyAllWindows();

	// 保存
	imwrite(common::joinPath(outDir, "letterbox_640x640.jpg"), canvas);
}

// ============================================================
// 实验总入口
// ============================================================
int demo_ResizeImage()
{
	cout << "========== 实验 03：图像缩放 (cv::resize) ==========\n\n";

	// --- 准备工作 ---
	const string outDir = common::outputDirectory();
	bool success = common::createDirectories(outDir);
	if (!success)
	{
		cerr << "[错误] 创建输出目录失败: " << outDir << "\n";
		return -1;
	}

	const string imgDir = common::imageDirectory();
	if (imgDir.empty())
	{
		cerr << "[错误] 图像目录不存在，请检查路径\n";
		return -1;
	}
	const string imgPath = common::joinPath(imgDir, "size.jpg");

	Mat img = imread(imgPath, IMREAD_UNCHANGED);
	if (img.empty())
	{
		cerr << "[错误] 图像读取失败: " << imgPath << "\n";
		return -1;
	}

	common::printImageInfo("test image", img);
	cout << "输出目录: " << outDir << "\n";

	// --- 依次执行各步骤 ---
	resizeBySize(img, outDir);       // Step 1: 指定宽高
	resizeByScale(img, outDir);      // Step 2: 按比例缩放
	resizeByInterp(img, outDir);     // Step 3: 插值方法对比
	resizeToFit(img, outDir);        // Step 4: 保持比例适配
	letterbox(img, outDir);          // Step 5: Letterbox 填充

	cout << "\n========== 实验 03 完成 ==========\n";
	return 0;
}