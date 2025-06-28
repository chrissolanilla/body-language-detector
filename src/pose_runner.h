#pragma once
#include <functional>
#include <atomic>
#include <memory>

namespace Gtk {
	class TextView;
}

void run_pose_on_monitor(int monitorIndex, Gtk::TextView* outputBox, std::shared_ptr<std::atomic_bool> stopFlag);
