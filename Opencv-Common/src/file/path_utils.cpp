#include "common/path_utils.h"

#define WIN32_LEAN_AND_MEAN   // 精简 windows.h，加快编译
#define NOMINMAX              // 防止 min/max 宏与 std::min/std::max 冲突
#include <windows.h>	// 必须包含此头文件以支持 WIN32_FILE_ATTRIBUTE_DATA
#include <sstream>		// 必需：为了使用 std::ostringstream
#include <string>   
#include <vector> 
#include <filesystem> 

using namespace std;
using namespace std::filesystem;

namespace common
{
	string exeDirectory()
	{
		// 1. 使用动态缓冲区循环获取宽路径，防止路径超长截断（吸收 AI 的严谨设计）
		DWORD size = 1024;
		wstring fullPathW;// 用宽字符串 解决 中文路径乱码问题

		// 用循环动态扩容的方式去问系统要
		while (true)
		{
			vector<wchar_t> buffer(size);
			DWORD copied = GetModuleFileNameW(nullptr, buffer.data(), size);

			if (copied == 0)
			{
				// 获取失败：返回空串，并可按需记录日志（如：GetLastError()）
				// std::cerr << "GetModuleFileNameW failed, error: " << GetLastError() << std::endl;
				return "";
			}

			if (copied < size)
			{
				fullPathW.assign(buffer.data(), copied);
				break;
			}

			// 防御性上限检查，防止无限循环
			if (size > 65536)
			{
				// std::cerr << "Exe path is abnormally too long." << std::endl;
				return "";
			}
			size *= 2;
		}

		// 2. 利用 C++17 std::filesystem 处理路径
		path exePath(fullPathW);

		// 3. 获取父目录，并转为 std::string (使用系统 ANSI 代码页（中文系统为 GBK），全链路窄字符串需保持同一种编码，切勿混入 UTF-8`。)
		path parentPath = exePath.parent_path();

		if (parentPath.empty())
		{
			return "";
		}

		return parentPath.string();
	}

	string imageDirectory()
	{
		string exeDir = exeDirectory();
		if (exeDir.empty()) 
			return ""; // 如果 exe 目录获取失败，直接返回空

		return joinPath(joinPath(exeDir, "data"), "img");
	}

	string outputDirectory()
	{
		string exeDir = exeDirectory();
		if (exeDir.empty())
			return ""; // 如果 exe 目录获取失败，直接返回空

		return joinPath(exeDir, "output");
	}

	string joinPath(const string& dir, const string& name)
	{
		// 在 C++17 中，std::filesystem::path 重载了 / 运算符，它会自动处理所有系统的斜杠问题
		return (path(dir) / name).string();
	}

	bool pathExists(const string& filepath)
	{
		if (filepath.empty())
			return false;
		try
		{
			return std::filesystem::exists(path(filepath));
		}
		catch (...)
		{
			return false;
		}
	}

	bool createDirectories(const string& dir)
	{
		if (dir.empty())
		{
			return false;
		}

		try
		{
			// path 从窄字符串构造时按系统 ANSI 代码页（中文系统为 GBK）解析，
			// 全链路窄字符串保持同一种编码，切勿混入 UTF-8
			path p(dir);

			// 核心函数：递归创建所有不存在的父目录
			// 注意：不要直接把它的返回值当成函数返回值！
			std::filesystem::create_directories(p);

			// 只要没有抛出异常，就说明文件夹最终可用（无论是刚创建的，还是本来就存在的）
			return true;
		}
		catch (const filesystem_error& e)
		{
			// 如果遇到权限不足、路径非法等真实错误，会在这里被捕获
			// 建议在这里打个日志，方便排查：std::cout << e.what() << std::endl;
			return false;
		}
	}

	// ============================================================
	// 辅助函数：获取文件大小（字节）
	// ============================================================
	unsigned long long getFileSize(const string& filepath)
	{
		if (filepath.empty()) return 0;

		try 
		{
			return std::filesystem::file_size(path(filepath));
		}
		catch (...) 
		{
			return 0;
		}
	}

	string formatFileSize(unsigned long long bytes)
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
