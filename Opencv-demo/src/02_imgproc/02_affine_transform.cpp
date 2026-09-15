// ============================================================
// 实验 02：图像旋转与平移
//
//   rotateImage()         — Step 1: 基本旋转
//   rotateWithScale()     — Step 2: 旋转 + 缩放
//   translateImage()      — Step 3: 平移
//   borderDemo()          — Step 4: 边界处理
//   rotateFillAll()       — Step 5: 旋转并保留完整图像
//   runRotateTranslateExperiment() — 实验总入口
// ============================================================

#include "02_affine_transform.h"

// 公共库头文件
#include "common/mat_info.h"
#include "common/path_utils.h"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp> // 包含图像处理相关函数
#include <opencv2/geometry.hpp> // OpenCV 5 新增或调整的几何相关头文件
#include <iostream>
#include <string>
#include <cmath>

using namespace std;
using namespace cv;

// ============================================================
// 数学背景：放射变换矩阵
// ============================================================
//
// 什么是放射变换 (Affine Transformation)？
//   一种保持"平行关系"的几何变换
//   原图中的平行线，变换后仍然是平行线
//   但角度和长度可能改变
//
// 变换矩阵 M 是 2×3 矩阵：
//   M = [a  b  c]      [x']   [a b c] [x]
//       [d  e  f]  →  [y'] = [d e f] [y]
//                                     [1]
//
//   (x', y') = (a·x + b·y + c, d·x + e·y + f)
//
// M 的不同取值对应不同变换：
//   恒等变换: [1 0 0]   平移:  [1 0 tx]
//             [0 1 0]          [0 1 ty]
//
//   旋转 θ:   [cosθ  -sinθ  0]
//              [sinθ   cosθ  0]
//
//   缩放 s:   [sx  0  0]    剪切: [1  shx   0]
//             [0  sy  0]          [shy  1   0]
// ============================================================

// ============================================================
// Step 1：基本旋转
// getRotationMatrix2D(center, angle, scale)：输出2×3 变换矩阵
//   center: 旋转中心点 (Point2f)
//   angle:  旋转角度（度），正=逆时针
//   scale:  缩放因子，1.0 = 不缩放
//
// warpAffine(src, dst, M, dsize, flags, borderMode, borderValue)
//   src:         输入图像
//   dst:         输出图像（自动分配）
//   M:           2×3 变换矩阵
//   dsize:       输出图像尺寸
//   flags:       插值方法（默认 INTER_LINEAR）
//   borderMode:  边界填充方式（默认 BORDER_CONSTANT）
//   borderValue: 边界填充值（默认 0，即黑色）
//
// 注意：旋转后图像四角可能超出边界
//       dsize 保持原图尺寸时，超出部分被裁剪
//       这也是为什么旋转 45° 后图像看起来变小了
// ============================================================
static void rotateImage(const Mat& img, const string& outDir)
{
	// 旋转中心点：图像中心
	Point2f center((img.cols - 1) / 2.0f, (img.rows - 1) / 2.0f);

	cout << "  旋转中心: (" << center.x << ", " << center.y << ")\n";

	// 测试多个角度
	const vector<int> angles = { 0, 45, 90, -45, 180 };

	for (int angle : angles)
	{
		// 1. 生成2×3 变换矩阵
		Mat T = getRotationMatrix2D(center, angle, 1.0);

		// 2. 应用变换
		Mat img_rotated;
		warpAffine(img, img_rotated, T, img.size(), INTER_LINEAR, BORDER_CONSTANT, Scalar(255, 255, 255)/* 用白色填充 */);

		cout << "    输出: " << img_rotated.cols << "x" << img_rotated.rows << "\n";

		// 3. 保存图像到本地
		string filename = "rotate_" + to_string(angle) + "deg.jpg";
		imwrite(common::joinPath(outDir, filename), img_rotated);
	}

	{
		// 显示 45° 旋转结果
		Mat T = getRotationMatrix2D(center, 45, 1.0);
		Mat img_rotated;
		warpAffine(img, img_rotated, T, img.size(), INTER_LINEAR, BORDER_CONSTANT, Scalar(255, 255, 255));

		imshow("Original", img);
		imshow("Rotated 45° CCW", img_rotated);
		waitKey(0);
		destroyAllWindows();
	}
}

// ============================================================
// Step 2：旋转 + 缩放
// ============================================================
static void rotateWithScale(const Mat& img, const string& outDir)
{
	// 旋转中心点：图像中心
	Point2f center((img.cols - 1) / 2.0f, (img.rows - 1) / 2.0f);

	// 测试不同缩放因子
	const vector<double> scales = { 0.5, 0.75, 1.0, 1.5 };

	Mat displayRow;
	for (size_t i = 0; i < scales.size(); ++i)
	{
		// 1. 生成2×3 变换矩阵(旋转角度 45°，进行缩放)
		double scale = scales[i];
		Mat T = getRotationMatrix2D(center, 45, scale);

		// 2. 应用变换
		Mat img_rotated;
		warpAffine(img, img_rotated, T, img.size(), INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0)/*用黑色填充*/);

		cout << "  scale=" << scale << ": 输出 " << img_rotated.cols << "x" << img_rotated.rows << "\n";

		// 3. 横向拼接显示
		if (i == 0)
		{
			displayRow = img_rotated;
		}
		else
		{
			hconcat(displayRow, img_rotated, displayRow);
		}

		// 3.1.保存图像到本地
		string filename = "rotate45_scale" + to_string(static_cast<int>(scale * 100)) + ".jpg";
		imwrite(common::joinPath(outDir, filename), img_rotated);
	}

	// 显示对比
	{
		namedWindow("Scale 0.5 | 0.75 | 1.0 | 1.5", WINDOW_NORMAL);
		imshow("Scale 0.5 | 0.75 | 1.0 | 1.5", displayRow);
		cout << "  按任意键继续...\n";
		waitKey(0);
		destroyAllWindows();
	}

	cout << "  观察：scale 越小，旋转后四角裁剪越少\n";
}

// ============================================================
// Step 3：平移
// 构造平移矩阵的两种方式：
//   1. 手动构造 Mat（适合平移）
//   2. 先用 getRotationMatrix2D 再修改第三列
// ============================================================
static void translateImage(const Mat& img, const string& outDir)
{
	cout << "\n--- Step 3: 平移 ---\n";
	cout << "  原图: " << img.cols << "x" << img.rows << "\n";

	const int tx = img.cols / 4;   // 水平平移：宽度的 1/4
	const int ty = img.rows / 4;   // 垂直平移：高度的 1/4

	// 方式 1：手动构造平移矩阵
	float warp_values[] = { 1.0, 0.0, tx, 0.0, 1.0, ty };
	Mat T = Mat(2, 3, CV_32F, warp_values);
	
	// 打印相关信息
	cout << "  平移矩阵 M (tx=" << tx << ", ty=" << ty << "):\n";
	for (int i = 0; i < T.rows; ++i)
	{
		cout << "    [";
		for (int j = 0; j < T.cols; ++j)
		{
			cout << format("%8.1f", T.at<double>(i, j));
		}
		cout << "]\n";
	}

	// 应用平移
	Mat translated;
	warpAffine(img, translated, T, img.size(), INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));

	// 显示
	imshow("Original", img);
	imshow("Translated (right+down)", translated);

	// 在原图上标记移动方向（箭头）
	Mat arrowImg = img.clone();
	Point2f start(img.cols / 2.0f, img.rows / 2.0f);
	Point2f endPt(start.x + tx, start.y + ty);
	arrowedLine(arrowImg, start, endPt, Scalar(0, 255, 0), 2);
	imshow("Translation Direction", arrowImg);

	waitKey(0);
	destroyAllWindows();

	// 保存
	imwrite(common::joinPath(outDir, "translate_right_down.jpg"), translated);

	// --- 测试不同方向的平移 ---
	cout << "\n  测试不同平移方向：\n";
	const vector<pair<int, int>> directions = {
		{ tx,  0},   // 右
		{-tx,  0},   // 左
		{  0,  ty},  // 下
		{  0, -ty},  // 上
		{ tx,  ty},  // 右下
		{-tx, -ty},  // 左上
	};

	Mat gridRow, grid;
	for (size_t i = 0; i < directions.size(); ++i)
	{
		int dx = directions[i].first;
		int dy = directions[i].second;

		cv::Matx23d M2_x(1.0, 0.0, dx,
			0.0, 1.0, dy);
		cv::Mat M2 = cv::Mat(M2_x);

		Mat trans;
		warpAffine(img, trans, M2, img.size());

		cout << "    tx=" << (dx >= 0 ? "" : "") << dx
			<< ", ty=" << (dy >= 0 ? "" : "") << dy
			<< " → " << trans.cols << "x" << trans.rows << "\n";

		if (i == 0) gridRow = trans;
		else if (i < 3) hconcat(gridRow, trans, gridRow);
		else if (i == 3) { grid = gridRow; gridRow = trans; }
		else hconcat(gridRow, trans, gridRow);

		if (i == 5) { vconcat(grid, gridRow, grid); }
	}

	imshow("6 Translation Directions (right,left,down,up,down-right,up-left)", grid);
	cout << "  按任意键继续...\n";
	waitKey(0);
	destroyAllWindows();
}

// ============================================================
// Step 4：边界处理方式
// warpAffine 的 borderMode 参数决定了：
//   变换后落在图像范围外的像素怎么处理
//
// 常用 borderMode：
//   BORDER_CONSTANT    - 用常数填充（默认，黑色）
//   BORDER_REPLICATE   - 复制边缘像素
//   BORDER_REFLECT     - 镜像反射
//   BORDER_WRAP        - 平铺（环形）
//
// borderValue 仅在 BORDER_CONSTANT 时生效
//   Scalar(0,0,0) → 黑色
//   Scalar(255,255,255) → 白色
//   Scalar(128,128,128) → 灰色
//
// 选择建议：
//   旋转/平移预览 → BORDER_CONSTANT（黑色填充，对比明显）
//   图像拼接       → BORDER_REPLICATE 或 BORDER_REFLECT
//   纹理合成       → BORDER_WRAP
// ============================================================
static void borderDemo(const Mat& img, const string& outDir)
{
	cout << "\n--- Step 4: 边界处理方式 ---\n";

	Point2f center((img.cols - 1) / 2.0f, (img.rows - 1) / 2.0f);
	Mat T_Mat = getRotationMatrix2D(center, 45, 0.8);  // 缩小+旋转，边缘有空白

	// 4 种边界模式
	Mat resConstant, resReplicate, resReflect, resWrap;

	warpAffine(img, resConstant, T_Mat, img.size(), INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));// 黑色
	warpAffine(img, resReplicate, T_Mat, img.size(), INTER_LINEAR, BORDER_REPLICATE);// 边缘复制
	warpAffine(img, resReflect, T_Mat, img.size(), INTER_LINEAR, BORDER_REFLECT);// 镜像反射
	warpAffine(img, resWrap, T_Mat, img.size(), INTER_LINEAR, BORDER_WRAP);// 环形平铺

	imshow("Original", img);
	imshow("CONSTANT   → 四角黑色填充", resConstant);
	imshow("REPLICATE  → 四角是边缘像素的延伸", resReplicate);
	imshow("REFLECT    → 四角是镜像反射", resReflect);
	imshow("WRAP       → 四角是对侧像素（平铺", resWrap);

	// 拼接显示
	Mat row1, row2, combined;
	hconcat(resConstant, resReplicate, row1);//水平方向拼接
	hconcat(resReflect, resWrap, row2);//水平方向拼接
	vconcat(row1, row2, combined);//垂直方向拼接

	imshow("CONSTANT(black)  |  REPLICATE\nREFLECT         |  WRAP", combined);
	cout << "  观察：\n"
		<< "  CONSTANT   → 四角黑色填充\n"
		<< "  REPLICATE  → 四角是边缘像素的延伸\n"
		<< "  REFLECT    → 四角是镜像反射\n"
		<< "  WRAP       → 四角是对侧像素（平铺）\n"
		<< "  按任意键继续...\n";
	waitKey(0);
	destroyAllWindows();

	// 保存
	imwrite(common::joinPath(outDir, "border_constant.jpg"), resConstant);
	imwrite(common::joinPath(outDir, "border_replicate.jpg"), resReplicate);
	imwrite(common::joinPath(outDir, "border_reflect.jpg"), resReflect);
	imwrite(common::joinPath(outDir, "border_wrap.jpg"), resWrap);
}

// ============================================================
// Step 5：旋转并保留完整图像
// 问题：当旋转 45° 时，图像四角超出边界被裁剪
//       即使 scale=1.0，旋转后的外接矩形比原图大
//
// 解决方案：
//   1. 计算旋转后的外接矩形尺寸
//   2. 调整旋转矩阵的平移分量，使旋转中心偏移
//   3. 设置 dsize 为新的外接矩形尺寸
//
// 数学推导：
//   原图宽 W, 高 H
//   旋转 θ 度后：
//     new_W = |W·cos θ| + |H·sin θ|
//     new_H = |W·sin θ| + |H·cos θ|
//
//   需要在 M 的第三列加上偏移量：
//     offsetX = (new_W - W) / 2
//     offsetY = (new_H - H) / 2
//
// 这样旋转后的完整图像就能全部显示
// ============================================================
static void rotateFillAll(const Mat& img, const string& outDir)
{
	cout << "\n--- Step 5: 旋转并保留完整图像 ---\n";

	const double angle = 30.0;
	const double rad = angle * CV_PI / 180.0;

	// 1. 计算旋转后的外接矩形尺寸
	double cosA = abs(cos(rad));
	double sinA = abs(sin(rad));
	int newW = static_cast<int>(img.cols * cosA + img.rows * sinA + 0.5);
	int newH = static_cast<int>(img.cols * sinA + img.rows * cosA + 0.5);

	cout << "  原图: " << img.cols << "x" << img.rows << "\n";
	cout << "  旋转后外接矩形: " << newW << "x" << newH << "\n";

	// 2. 先生成标准旋转矩阵
	Point2f center((img.cols - 1) / 2.0f, (img.rows - 1) / 2.0f);
	Mat T_Mat = getRotationMatrix2D(center, angle, 1.0);

	// 3. 在 M 上添加偏移量，使旋转后的中心对齐到新画布中心
	//    偏移量 = (新尺寸 - 原尺寸) / 2
	T_Mat.at<double>(0, 2) += (newW - img.cols) / 2.0;
	T_Mat.at<double>(1, 2) += (newH - img.rows) / 2.0;

	// 打印信息
	cout << "  调整后的矩阵 M:\n";
	for (int i = 0; i < T_Mat.rows; ++i)
	{
		cout << "    [";
		for (int j = 0; j < T_Mat.cols; ++j)
		{
			cout << format("%8.3f", T_Mat.at<double>(i, j));
		}
		cout << "]\n";
	}

	// 4. 设置新的 dsize 并应用
	Mat rotatedFull;
	warpAffine(img, rotatedFull, T_Mat, Size(newW, newH), INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));

	cout << "  输出: " << rotatedFull.cols << "x" << rotatedFull.rows << "\n";

	// 对比：普通旋转 vs 完整旋转
	Mat Mnormal = getRotationMatrix2D(center, angle, 1.0);
	Mat rotatedNormal;
	warpAffine(img, rotatedNormal, Mnormal, img.size());

	imshow("Normal Rotation (clipped)", rotatedNormal);
	imshow("Full Rotation (complete)", rotatedFull);
	cout << "  按任意键继续...\n";
	waitKey(0);
	destroyAllWindows();

	// 保存
	imwrite(common::joinPath(outDir, "rotate_full_45.jpg"), rotatedFull);
	imwrite(common::joinPath(outDir, "rotate_normal_45.jpg"), rotatedNormal);

	cout << "\n  观察：完整旋转的图像四周没有被裁剪\n";
}

// ============================================================
// 实验总入口
// ============================================================
int demo_RotateTranslate()
{
	cout << "========== 实验 05：图像旋转与平移 ==========\n\n";

	// --- 准备工作 ---
	const string outDir = common::outputDirectory();
	common::createDirectories(outDir);

	// 生成测试图像
	const string imgDir = common::imageDirectory();
	const string imgPath = common::joinPath(imgDir, "test.jpg");

	Mat img_color = imread(imgPath, IMREAD_UNCHANGED);
	if (img_color.empty())
	{
		cerr << "[错误] 图像读取失败: " << imgPath << "\n";
		return -1;
	}

	cout << "测试图像: " << img_color.cols << "x" << img_color.rows << "\n";
	common::printImageInfo("test image", img_color);
	cout << "输出目录: " << outDir << "\n";

	// --- 依次执行各步骤 ---
	rotateImage(img_color, outDir);          // Step 1: 基本旋转
	rotateWithScale(img_color, outDir);      // Step 2: 旋转+缩放
	translateImage(img_color, outDir);       // Step 3: 平移
	borderDemo(img_color, outDir);           // Step 4: 边界处理（旋转 + 缩放的 边界处理）
	rotateFillAll(img_color, outDir);        // Step 5: 完整旋转（此时图像大小会改变）

	return 0;
}