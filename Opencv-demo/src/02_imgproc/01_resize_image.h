#pragma once
#ifndef RESIZE_IMAGE_H
#define RESIZE_IMAGE_H

// ============================================================
// 实验 03：图像缩放 (cv::resize)
// ============================================================
// image.rows	给出图像的 高度
// image.columns给出图像的 宽度
// 
// 学习目标：
//   1. 掌握 resize(src, dst, dsize) 指定宽高缩放
//   2. 掌握 resize(src, dst, Size(), fx, fy) 按比例缩放
//   3. 理解 4 种常用插值方法的区别和适用场景
//   4. 学会保持宽高比缩放（避免图像变形）
//   5. 学会 letterbox 填充（适合模型输入）
//
//
// 函数签名：
//   resize(src, dst, dsize, fx=0, fy=0, interpolation=INTER_LINEAR)
//
//   src             - 输入图像
//   dst             - 输出图像（可自动分配）
//   dsize           - 目标尺寸 (宽, 高)，指定后 fx/fy 自动计算
//   fx, fy          - 缩放因子，dsize 为 Size() 时生效
//   interpolation   - 插值方法，默认 INTER_LINEAR
//
// 插值方法对比：
//   INTER_NEAREST - 最近邻，速度最快，质量最差（块状感）
//   INTER_LINEAR  - 双线性，速度较快，通用（默认）
//   INTER_CUBIC   - 双三次，4x4 邻域，更平滑但慢
//   INTER_AREA    - 像素区域重采样，缩小时质量最好
//
// 选择原则：
//   缩小时 → INTER_AREA（避免混叠）
//   放大时 → INTER_CUBIC 或 INTER_LINEAR
//   标签图 → INTER_NEAREST（保持整数值）
//
// ============================================================

// 运行图像缩放实验
// 返回值：0 成功，-1 失败
int demo_ResizeImage();
#endif // RESIZE_IMAGE_H