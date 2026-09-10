#pragma once
#ifndef READ_IMAGE_H
#define READ_IMAGE_H

// ============================================================
// 实验 01：读取图像
// ============================================================
//
// 教程来源：
//   https://learnopencv.com/read-display-and-write-an-image-using-opencv/#read-image
//
// 学习目标：
//   1. 理解 cv::imread(filename, flags) 的参数与返回值
// 
//   2. 掌握三种读取模式及其区别：
//        IMREAD_COLOR     (1)  —— 始终转为 3 通道 BGR
//        IMREAD_GRAYSCALE (0)  —— 始终转为单通道灰度
//        IMREAD_UNCHANGED (-1) —— 保持原始通道数与深度
// 
//   3. 掌握读取失败检查：Mat::empty()
//      （imread 读不到文件不抛异常，只返回空 Mat）
//   4. 理解不同模式返回的 Mat 类型（CV_8UC1 / CV_8UC3 等）
//
// 图像路径约定：
//   <.exe 所在目录>\data\img\test.jpg（输入图像地址）
//   <.exe 所在目录>\output\test.jpg（输出图像地址） 
//   首次运行若图像不存在，由 common 库自动生成。
// ============================================================

// 运行读取图像实验
// 返回值：0 成功，-1 失败
int demo_ReadImage();

#endif // READ_IMAGE_H

