#pragma once
#ifndef AFFINE_TRANSLATE_IMAGE_H
#define AFFINE_TRANSLATE_IMAGE_H

// ============================================================
// 实验 02：图像旋转与平移 (Affine Transform)
// ============================================================
//
// 学习目标：
//   1. 理解放射变换 (Affine Transformation) 的数学原理
//   2. 掌握 getRotationMatrix2D() 获取旋转矩阵
//   3. 掌握 warpAffine() 应用变换
//   4. 理解旋转矩阵 M 的结构和含义
//   5. 学会平移矩阵的构造
//   6. 理解 borderMode 和 borderValue 的作用
//
// 核心函数：
//   getRotationMatrix2D(center, angle, scale) → 2×3 矩阵
//   warpAffine(src, dst, M, dsize, flags, borderMode, borderValue)
//
// 数学原理：
//   旋转矩阵（无缩放）：
//     M = [cos θ  -sin θ  0]
//         [sin θ   cos θ  0]
//
//   绕任意点 (cx, cy) 旋转并缩放：
//     α = scale · cos θ,  β = scale · sin θ
//     M = [α  -β  (1-α)·cx - β·cy]
//         [β   α  β·cx + (1-α)·cy]
//
//   平移矩阵：
//     M = [1  0  tx]
//         [0  1  ty]
//
// 角度约定：
//   正角度 = 逆时针方向（数学约定）
//   负角度 = 顺时针方向
// ============================================================

// 运行图像旋转与平移实验
// 返回值：0 成功，-1 失败
int demo_RotateTranslate();

#endif // AFFINE_TRANSLATE_IMAGE_H

