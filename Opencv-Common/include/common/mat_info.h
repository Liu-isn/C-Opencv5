#pragma once

#ifndef COMMON_MAT_INFO_H
#define COMMON_MAT_INFO_H

// ============================================================
// 公共工具：cv::Mat 信息打印
// ============================================================
//
// 学习 cv::Mat 时最常做的事：确认一张 Mat 的
//   尺寸（cols x rows）、通道数（channels）、类型（type）
// type() 编码了【深度 + 通道数】，常见值：
//   CV_8UC1  = 8位无符号 + 1通道   (灰度图)
//   CV_8UC3  = 8位无符号 + 3通道   (BGR 彩色图)
//   CV_8UC4  = 8位无符号 + 4通道   (BGRA，含 Alpha)
//   CV_16UC1 = 16位无符号 + 1通道  (医学影像/深度图)
//   CV_32FC1 = 32位浮点 + 1通道   (梯度/傅里叶变换结果)
// ============================================================

#include <opencv2/opencv.hpp>
#include <string>

namespace common
{

	// 打印一张 Mat 的尺寸、通道数、类型
	void printImageInfo(const std::string& label, const cv::Mat& img);

} // namespace common

#endif // COMMON_MAT_INFO_H#pragma once
