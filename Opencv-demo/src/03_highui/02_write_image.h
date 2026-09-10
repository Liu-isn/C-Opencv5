#pragma once
#ifndef WRITE_IMAGE_H
#define WRITE_IMAGE_H

// ============================================================
// 实验 02：保存图像 (cv::imwrite)
// ============================================================
//	教程来源：
//   https://learnopencv.com/read-display-and-write-an-image-using-opencv/#read-image
// 
// 学习目标：
//   1. 掌握 cv::imwrite(filename, mat, params) 的用法
//   2. 理解不同图像格式（JPG / PNG / BMP）的特点
//   3. 学习压缩质量参数对文件大小和图像质量的影响
//   4. 掌握 imwrite 的错误处理（返回值 + 异常安全）
//
// 图像格式对比：
//   JPG  - 有损压缩，文件小，适合照片/自然图像，不支持透明通道		（有损）
//   PNG  - 无损压缩，支持透明通道，适合图标/图形/带 Alpha 的图像	（无损 + 透明）
//   BMP  - 无压缩，文件大，兼容性好，不支持透明通道				（无损）
// ============================================================

// 运行保存图像实验
// 返回值：0 成功，-1 失败
int demo_WriteImage();


#endif // WRITE_IMAGE_H
