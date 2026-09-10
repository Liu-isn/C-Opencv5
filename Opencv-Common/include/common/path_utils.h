#pragma once
#ifndef COMMON_PATH_UTILS_H
#define COMMON_PATH_UTILS_H

// ============================================================
// 公共工具：路径管理（ISO C++14，不使用 std::filesystem）
// ============================================================
//
// 为什么不用 std::filesystem？
//   项目约束：使用 ISO C++14 标准。
//   路径与目录操作基于 Windows API 实现：
//     GetModuleFileNameA  —— 取 exe 完整路径
//     CreateDirectoryA    —— 创建单级目录
//     GetFileAttributesA  —— 判断路径是否存在
//
// 目录约定（均相对于 exe 所在目录）：
//   <exe目录>\data\img\    输入图像地址
//   <exe目录>\output\      输出图像地址
// ============================================================

#include <string>

namespace common
{
	// .exe 所在目录（路径基准，不含末尾分隔符）
	// .exe 所在目录 不能超过 MAX_PATH（260）字符，否则会被截断。
	std::string exeDirectory();

	// 图像目录：<exe目录>\data\img
	std::string imageDirectory();

	// 输出目录：<exe目录>\output
	std::string outputDirectory();

	// 拼接路径：dir + 分隔符 + name（自动处理末尾分隔符）
	std::string joinPath(const std::string& dir, const std::string& name);

	// 判断文件或目录是否存在
	bool pathExists(const std::string& path);

	// 逐级创建目录（类似 std::filesystem::create_directories，
	// 中间级不存在会一并创建；目录已存在时无副作用）
	void createDirectories(const std::string& dir);

	// 获取文件大小（字节）
	// 返回 0 表示文件不存在或读取失败
	// 使用 Windows API GetFileAttributesExA，支持大文件（>4GB）
	unsigned long long getFileSize(const std::string& path);

	// 将字节数格式化为人类可读的字符串
	// 自动选择单位：Bytes / KB / MB / GB
	// 保留 2 位小数（当单位 >= KB 时）
	// 示例：formatFileSize(0) -> "0 Bytes"
	//       formatFileSize(1024) -> "1.00 KB"
	//       formatFileSize(1572864) -> "1.50 MB"
	std::string formatFileSize(unsigned long long bytes);

} // namespace common

#endif // COMMON_PATH_UTILS_H
