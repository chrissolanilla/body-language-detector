#include "pose_runner.h"
#include "detectors.h"
#include "gdkmm/monitor.h"
#include "glibmm/main.h"

#include <opencv2/opencv.hpp>
#include <gtkmm/textview.h>
#include <gtkmm/textbuffer.h>
#include <thread>
#include <chrono>

void run_pose_on_monitor(int monitorIndex, Gtk::TextView* outputBox, std::shared_ptr<std::atomic_bool> stopFlag) {
	std::thread([monitorIndex, outputBox, stopFlag]() {
		auto display = Gdk::Display::get_default();
		auto monitor = display->get_monitor(monitorIndex);

		Gdk::Rectangle geometry;
		monitor->get_geometry(geometry);
		int x = geometry.get_x();
		int y = geometry.get_y();
		int w = geometry.get_width();
		int h = geometry.get_height();

		std::stringstream inputString;
		inputString << "-video_size " << w << "x" << h
			<< " -framerate 30 -f x11grab -i :0.0+" << x << "," << y;

		cv::VideoCapture cap(inputString.str(), cv::CAP_FFMPEG);

		if(!cap.isOpened()){

			Glib::signal_idle().connect_once([outputBox] {
				outputBox->get_buffer()->insert(outputBox->get_buffer()->end(), "Failed to capture screen\n");
			});
			return;
		}

		while(!stopFlag->load()){
			cv::Mat frame;
			cap >> frame;
			if(frame.empty()) {
				break;
			}
			//TODO put gestures
			std::string gesture = "Detected: TODO";
			//update GUI safely from main thread
			Glib::signal_idle().connect_once([outputBox, gesture] {
				outputBox->get_buffer()->insert(outputBox->get_buffer()->end(), gesture + "\n");
			 });

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		Glib::signal_idle().connect_once([outputBox] {
			outputBox->get_buffer()->insert(outputBox->get_buffer()->end(), "Detection thread stopped\n");
		});
	}).detach();
}
