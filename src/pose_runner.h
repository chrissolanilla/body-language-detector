#pragma once
#include <functional>
#include <atomic>
#include <memory>
#include "detectors.h"
#include <opencv2/core.hpp>

namespace Gtk {
	class TextView;
}

void run_pose_on_monitor(int monitorIndex, Gtk::TextView* outputBox, std::shared_ptr<std::atomic_bool> stopFlag);
void run_pose_on_video(const std::string& videoPath, Gtk::TextView* outputBox, std::shared_ptr<std::atomic_bool> stopFlag);
std::vector<std::string> detect_gestures(const std::vector<cv::Point3f>& landmarks);



