// 注意：Opencv5不支持32位系统，32位系统请使用Opencv4.5.5

#include <iostream>
#include <string>

//#define READ_IMAGE_DEMO  
//#define WRITE_IMAGE_DEMO 
#define RESIZE_IMAGE_DEMO 

#include "src/03_highui/01_read_image.h"
#include "src/03_highui/02_write_image.h"
#include "src/02_imgproc/01_resize_image.h"

int main()
{
	static int result = 0;
#ifdef READ_IMAGE_DEMO

	result = demo_ReadImage();
	if (result != 0)
	{
		std::cerr << "[错误] demo_ReadImage() 执行失败，返回值: " << result << "\n";
		return -1;
	}

#endif

#ifdef WRITE_IMAGE_DEMO

	result = demo_WriteImage();
	if (result != 0)
	{
		std::cerr << "[错误] demo_WriteImage() 执行失败，返回值: " << result << "\n";
		return -1;
	}

#endif

#ifdef RESIZE_IMAGE_DEMO

	result = demo_ResizeImage();
	if(!result)
	{
		std::cerr << "[错误] demo_ResizeImage() 执行失败，返回值: " << result << "\n";
		return -1;
	}

#endif

	return 0;
}