#include "detectors.h"
#include <opencv2/core.hpp>
#include <vector>
#include <string>
#include <cmath>

float distance(const cv::Point3f& a, const cv::Point3f& b) {
    return std::sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}

std::vector<std::string> detect_gestures(const std::vector<cv::Point3f>& landmarks) {
    std::vector<std::string> output;

    const auto& left_wrist = landmarks[15];
    const auto& right_wrist = landmarks[16];
    const auto& left_ear = landmarks[7];
    const auto& right_ear = landmarks[8];
    const auto& left_shoulder = landmarks[11];
    const auto& right_shoulder = landmarks[12];

    if (distance(right_wrist, right_ear) < 0.15f) {
        output.push_back("Scratching head with right hand");
    } else if (distance(left_wrist, left_ear) < 0.15f) {
        output.push_back("Scratching head with left hand");
    }

    if (distance(right_wrist, left_shoulder) < 0.25f &&
        distance(left_wrist, right_shoulder) < 0.25f) {
        output.push_back("Arms crossed");
    }

    if (distance(right_shoulder, right_wrist) < 0.1f &&
        distance(left_shoulder, left_wrist) < 0.1f) {
        output.push_back("Shrugging");
    }

    if (left_wrist.y < left_shoulder.y - 0.1f) {
        output.push_back("Left hand raised");
    }
    if (right_wrist.y < right_shoulder.y - 0.1f) {
        output.push_back("Right hand raised");
    }

    return output;
}

