// 注意：Opencv5不支持32位系统，32位系统请使用Opencv4.5.5

#include <iostream>
#include <string>

#define READ_IMAGE_DEMO  1

#include "src/03_highui/01_read_image.h"

int main()
{
#ifdef READ_IMAGE_DEMO

	int result = demo_ReadImage();
	if (result != 0)
	{
		std::cerr << "[错误] demo_ReadImage() 执行失败，返回值: " << result << "\n";
		return -1;
	}

#endif

	return 0;
}