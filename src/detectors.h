#pragma once
#include <string>
#include <vector>
#include <opencv2/core.hpp>


std::vector<std::string> detect_gestures(const std::vector<cv::Point3f>& landmarks);

