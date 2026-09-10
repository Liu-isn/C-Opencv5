#include "common/mat_info.h"

namespace common
{
	void printImageInfo(const std::string& label, const cv::Mat& img)
	{
		std::cout << label << ":\n";
		std::cout << "  尺寸 (WxH) : " << img.cols << " x " << img.rows << "\n";
		std::cout << "  通道数     : " << img.channels() << "\n";
		std::cout << "  类型 type(): " << cv::typeToString(img.type()) << "\n\n";
	}

} // namespace common