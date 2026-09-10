#pragma once
#ifndef COMMON_PATH_UTILS_H
#define COMMON_PATH_UTILS_H

// ============================================================
// 公共工具：路径管理（ISO C++17，使用 std::filesystem）
// ============================================================
//
// 目录约定（均相对于 exe 所在目录）：
//   <exe目录>\data\img\    输入图像地址
//   <exe目录>\output\      输出图像地址
// ============================================================

#include <string>

namespace common
{
	// .exe 所在目录（路径基准，不含末尾分隔符）
	std::string exeDirectory();

	// 图像目录：<exe目录>\data\img
	std::string imageDirectory();

	// 输出目录：<exe目录>\output
	std::string outputDirectory();

	// 拼接路径：dir + 分隔符 + name（自动处理末尾分隔符）
	std::string joinPath(const std::string& dir, const std::string& name);

	// 判断文件或目录是否存在
	bool pathExists(const std::string& filepath);

	// 逐级创建目录（类似 std::filesystem::create_directories，
	// 中间级不存在会一并创建；目录已存在时无副作用）
	bool createDirectories(const std::string& dir);

	// 获取文件大小（字节）
	// 返回 0 表示文件不存在或读取失败
	// 使用 Windows API GetFileAttributesExA，支持大文件（>4GB）
	unsigned long long getFileSize(const std::string& filepath);

	// 将字节数格式化为人类可读的字符串
	// 自动选择单位：Bytes / KB / MB / GB
	// 保留 2 位小数（当单位 >= KB 时）
	// 示例：formatFileSize(0) -> "0 Bytes"
	//       formatFileSize(1024) -> "1.00 KB"
	//       formatFileSize(1572864) -> "1.50 MB"
	std::string formatFileSize(unsigned long long bytes);

} // namespace common

#endif // COMMON_PATH_UTILS_H
