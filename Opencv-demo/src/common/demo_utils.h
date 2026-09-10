#pragma once
#ifndef DEMO_UTILS_H
#define DEMO_UTILS_H

#include <opencv2/opencv.hpp>
#include <string>


cv::Mat generateTestImage(int width, int height);
std::string ensureTestImage(); 

#endif // DEMO_UTILS_H