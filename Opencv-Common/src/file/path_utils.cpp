#include "common/path_utils.h"

// windows.h 提供路径相关的 Windows API。
// 必须定义以下宏，防止 windows.h 的宏污染其他头文件：
//   NOMINMAX            —— 防止定义 min/max 宏（与 std::min/std::max 冲突）
//   WIN32_LEAN_AND_MEAN —— 精简 windows.h，减少编译时间
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>// 必须包含此头文件以支持 WIN32_FILE_ATTRIBUTE_DATA
#include <sstream>    // 必需：为了使用 std::ostringstream
#include <iomanip>    // 必需：为了使用 std::fixed 和 std::setprecision
#include <string>    

namespace common 
{
	std::string exeDirectory()
	{
		// GetModuleFileNameA：返回当前进程 exe 的完整路径（ANSI 窄字符版）。
		//   参数 1: nullptr 表示取"当前进程"的 exe
		//   参数 2: 接收路径的缓冲区
		//   参数 3: 缓冲区大小（字符数）
		//
		// 注意：MAX_PATH = 260，超长路径会被截断。学习项目可接受；
		//       若将来要支持超长路径，需改用 GetModuleFileNameW + 动态缓冲区。
		char buffer[MAX_PATH] = {};
		GetModuleFileNameA(nullptr, buffer, MAX_PATH);

		const std::string fullPath(buffer);

		// 去掉最后的文件名，得到 exe 所在目录（不含末尾分隔符）
		const size_t pos = fullPath.find_last_of("\\/");
		if (pos == std::string::npos)
		{
			return ".";   // 异常兜底：找不到分隔符时退回当前目录
		}
		return fullPath.substr(0, pos);
	}

	std::string imageDirectory()
	{
		return joinPath(joinPath(exeDirectory(), "data"), "img");
	}

	std::string outputDirectory()
	{
		return joinPath(exeDirectory(), "output");
	}

	std::string joinPath(const std::string& dir, const std::string& name)
	{
		if (dir.empty())
		{
			return name;
		}
		const char last = dir.back();
		if (last == '\\' || last == '/')
		{
			return dir + name;   // dir 已带分隔符，直接拼接
		}
		return dir + "\\" + name;
	}

	bool pathExists(const std::string& path)
	{
		// GetFileAttributesA 对不存在的路径返回 INVALID_FILE_ATTRIBUTES。
		// 文件和目录都适用。
		const DWORD attrs = GetFileAttributesA(path.c_str());
		return attrs != INVALID_FILE_ATTRIBUTES;
	}

	void createDirectories(const std::string& dir)
	{
		// 逐级创建目录：
		//   D:\a\b\c → 依次尝试创建 D:\a、D:\a\b、D:\a\b\c
		// CreateDirectoryA 只能创建"最后一级"，所以逐级调用；
		// 已存在的中间级会返回失败（ERROR_ALREADY_EXISTS），直接忽略。
		std::string path = dir;

		// 去掉末尾分隔符，统一处理
		while (!path.empty() && (path.back() == '\\' || path.back() == '/'))
		{
			path.pop_back();
		}
		if (path.empty())
		{
			return;
		}

		for (size_t i = 1; i <= path.size(); ++i)
		{
			if (i == path.size() || path[i] == '\\' || path[i] == '/')
			{
				const std::string prefix = path.substr(0, i);

				// 跳过盘符根目录（如 "D:"），无需创建
				if (prefix.size() == 2 && prefix[1] == ':')
				{
					continue;
				}
				CreateDirectoryA(prefix.c_str(), nullptr);
			}
		}
	}

	// ============================================================
	// 辅助函数：获取文件大小（字节）
	// ============================================================
	// Windows API: GetFileAttributesExA
	// 用于对比不同格式/参数下的输出文件体积
	// ============================================================
	unsigned long long getFileSize(const std::string& path)
	{
		WIN32_FILE_ATTRIBUTE_DATA fad = {};
		if (GetFileAttributesExA(path.c_str(), GetFileExInfoStandard, &fad))
		{
			// 文件大小 = 高 32 位 << 32 | 低 32 位
			return (static_cast<unsigned long long>(fad.nFileSizeHigh) << 32)
				| fad.nFileSizeLow;
		}
		return 0;
	}

	std::string formatFileSize(unsigned long long bytes)
	{
		if (bytes < 1024)
		{
			return std::to_string(bytes) + " Bytes";
		}

		double size = static_cast<double>(bytes);

		const char* unit = "KB";
		size /= 1024.0;

		if (size >= 1024.0)
		{
			size /= 1024.0;
			unit = "MB";

			if (size >= 1024.0)
			{
				size /= 1024.0;
				unit = "GB";
			}
		}

		std::ostringstream oss;
		oss << std::fixed << std::setprecision(2) << size << " " << unit;
		return oss.str();
	}


} // namespace common
