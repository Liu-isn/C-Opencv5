// 注意：Opencv5不支持32位系统，32位系统请使用Opencv4.5.5


#include "common/mat_info.h"

int main() {
		
	cv::Mat img = cv::imread("test.jpg");
	common::printImageInfo("Test Image", img);
	return 0;
}