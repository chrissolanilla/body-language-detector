#include "pose_runner.h"
#include "detectors.h"
#include "gdkmm/monitor.h"
#include "glibmm/main.h"

#include <filesystem>
#include <iostream>
// #include <onnxruntime_cxx_api.h>
#include <onnxruntime/core/session/onnxruntime_cxx_api.h>
#include <opencv2/opencv.hpp>
#include <gtkmm/textview.h>
#include <gtkmm/textbuffer.h>
#include <thread>
#include <opencv2/dnn.hpp>

const std::vector<std::string> landmark_names = {
    "nose", "left eye (inner)", "left eye", "left eye (outer)",
    "right eye (inner)", "right eye", "right eye (outer)",
    "left ear", "right ear", "mouth (left)", "mouth (right)",
    "left shoulder", "right shoulder", "left elbow", "right elbow",
    "left wrist", "right wrist", "left pinky", "right pinky",
    "left index", "right index", "left thumb", "right thumb",
    "left hip", "right hip", "left knee", "right knee",
    "left ankle", "right ankle", "left heel", "right heel",
    "left foot index", "right foot index"
};


// static Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "pose_env");
void get_all_output_names(Ort::Session& session, Ort::AllocatorWithDefaultOptions& allocator,

						  std::vector<Ort::AllocatedStringPtr>& out_ptrs,
						  std::vector<const char*>& out_raw) {
	size_t count = session.GetOutputCount();

	out_ptrs.clear();
	out_raw.clear();
	for (size_t i = 0; i < count; ++i) {
		out_ptrs.push_back(session.GetOutputNameAllocated(i, allocator));
		out_raw.push_back(out_ptrs.back().get());
	}
}


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

		cv::VideoCapture cap;

#ifdef __APPLE__
		// On macOS, use AVFoundation. Index 1 might be screen or webcam — test it!
		cap.open(1, cv::CAP_AVFOUNDATION);  // OR cv::VideoCapture("screen-capture.mov") if using ffmpeg output
#elif defined(__linux__)
		std::stringstream inputString;
		inputString << "-video_size " << w << "x" << h
					<< " -framerate 30 -f x11grab -i :0.0+" << x << "," << y;
		cap.open(inputString.str(), cv::CAP_FFMPEG);
#elif defined(_WIN32)
		// Windows DirectShow or Media Foundation (dshow or mf)
		cap.open("video=screen-capture-recorder", cv::CAP_DSHOW);  // Requires extra software like OBS-VirtualCam
#else
		#error "Unsupported platform"
#endif

		namespace fs = std::filesystem;

		fs::path basePath = fs::path(__FILE__).parent_path().parent_path(); // src/ → project root
		fs::path modelPath = basePath / "models/pose_landmark_full.onnx";

		if (!fs::exists(modelPath)) {
			std::cerr << "Model not found at " << modelPath << "\n";
			return;
		}



		// Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "pose_runner");
		Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "pose_env_thread");

		Ort::SessionOptions session_options;
		session_options.SetIntraOpNumThreads(1);
		Ort::Session session(env, modelPath.c_str(), session_options);


		// Names
		Ort::AllocatorWithDefaultOptions allocator;
		std::vector<Ort::AllocatedStringPtr> input_ptrs;
		std::vector<const char*> input_names;
		input_ptrs.push_back(session.GetInputNameAllocated(0, allocator));
		input_names.push_back(input_ptrs.back().get());


		std::vector<Ort::AllocatedStringPtr> output_ptrs;
		std::vector<const char*> output_names;
		get_all_output_names(session, allocator, output_ptrs, output_names);

		if(!cap.isOpened()){

			Glib::signal_idle().connect_once([outputBox] {
				outputBox->get_buffer()->insert(outputBox->get_buffer()->end(), "Failed to capture screen\n");
			});
			return;
		}

		while(!stopFlag->load()){

			cv::Mat frame;
			cv::Mat resized;
			cap >> frame;
			cv::resize(frame, resized, cv::Size(256, 256));
			resized.convertTo(resized, CV_32FC3, 1.0 / 255.0); // make sure it’s 3-channel float
			cv::cvtColor(resized, resized, cv::COLOR_BGR2RGB);

			std::vector<float> input_tensor_values;
			input_tensor_values.reserve(256 * 256 * 3);
			for (int y = 0; y < 256; ++y) {
				for (int x = 0; x < 256; ++x) {
					if (resized.channels() != 3) {
						std::cerr << "Expected 3-channel RGB image but got " << resized.channels() << " channels\n";
						return;
					}
					cv::Vec3f pixel = resized.at<cv::Vec3f>(y, x);
					input_tensor_values.push_back(pixel[0]); // R
					input_tensor_values.push_back(pixel[1]); // G
					input_tensor_values.push_back(pixel[2]); // B
				}
			}

			std::array<int64_t, 4> input_shape{1, 256, 256, 3};
			Ort::MemoryInfo mem_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
			Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
				mem_info, input_tensor_values.data(), input_tensor_values.size(),
				input_shape.data(), input_shape.size()
			);



			std::cout << "input_names.data(): " << input_names.data();
			std::cout << "input_names.data(): " << input_names.data();
			std::cout << "input_names.data(): " << input_names.data();
			std::cout << "input_names.data(): " << input_names.data();
			auto output_tensors = session.Run(
				Ort::RunOptions{nullptr},
				input_names.data(), &input_tensor, 1,
				output_names.data(), output_names.size()
			);

			float* output_data = output_tensors[0].GetTensorMutableData<float>();
			std::vector<cv::Point3f> landmarks;
			for (int i = 0; i < 65; ++i) {
				float x = output_data[i * 3 + 0];
				float y = output_data[i * 3 + 1];
				float z = output_data[i * 3 + 2];
				landmarks.emplace_back(x, y, z);
			}

		}

		Glib::signal_idle().connect_once([outputBox] {
			outputBox->get_buffer()->insert(outputBox->get_buffer()->end(), "Detection thread stopped\n");
		});
	}).detach();
}


void run_pose_on_video(const std::string& videoPath, Gtk::TextView* outputBox, std::shared_ptr<std::atomic_bool> stopFlag) {
	std::cout <<"we dont crash before running the run_pose_on_video function" << std::endl;
	std::thread([videoPath, outputBox, stopFlag]() {
		std::cout << "we dont crash while in the thread" << std::endl;
		Glib::signal_idle().connect_once([outputBox] {
			outputBox->get_buffer()->insert(outputBox->get_buffer()->end(), "Starting test on video");
		});

		namespace fs = std::filesystem;
		fs::path basePath = fs::path(__FILE__).parent_path().parent_path();
		fs::path modelPath = basePath / "models/pose_landmark_full.onnx";

		if (!fs::exists(modelPath)) {
			std::cerr << "Model not found at " << modelPath << "\n";
			return;
		}
		std::cout <<"we dont crash after doing file system stuff and model path" << std::endl;

		Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "pose_env_thread");

		Ort::SessionOptions session_options;
		std::cout << "we dont crash while declaring session options" << std::endl;
		session_options.SetIntraOpNumThreads(1);
		std::cout << "we dont crash after setting intra op num threads" << std::endl;
		std::cout << "will we crash while declaring session with constructor?" << std::endl;
		try {
			std::cerr << "Creating session...\n";
			Ort::Session session(env, modelPath.c_str(), session_options);
			std::cerr << "Session created successfully.\n";
		} catch (const Ort::Exception& e) {
			std::cerr << "ONNX Runtime error: " << e.what() << "\n";
		}

		Ort::Session session(env, modelPath.c_str(), session_options);
		std::cout << "we didnt crash :)" << std::endl;
		std::cout << "we dont crash after getting session" << std::endl;

		Ort::AllocatorWithDefaultOptions allocator;
		auto output_name_ptr = session.GetOutputNameAllocated(0, allocator);
		std::cout << "we dont crash after doing output namae ptr" << std::endl;

		std::vector<Ort::AllocatedStringPtr> input_ptrs;
		std::vector<const char*> input_names;
		input_ptrs.push_back(session.GetInputNameAllocated(0, allocator));
		input_names.push_back(input_ptrs.back().get());
		const char* output_name = output_name_ptr.get();

		std::cout << "we dont crash after doing push backs" << std::endl;
		// Open video
		cv::VideoCapture cap(videoPath);
		if (!cap.isOpened()) {
			std::cerr << "Failed to open video: " << videoPath << "\n";
			Glib::signal_idle().connect_once([outputBox] {
				outputBox->get_buffer()->insert(outputBox->get_buffer()->end(), "Failed to open test video\n");
			});
			return;
		} else {
			std::cerr << "Opened video successfully.\n";
		}
		std::cout <<"we dont crash after opening video" << std::endl;

		while (!stopFlag->load()) {
			cv::Mat frame;
			cap >> frame;
			if (frame.empty()) break;

			cv::Mat resized;
			cv::resize(frame, resized, cv::Size(256, 256));
			resized.convertTo(resized, CV_32FC3, 1.0 / 255.0); // Normalize and set float type
			cv::cvtColor(resized, resized, cv::COLOR_BGR2RGB); // Convert BGR to RGB

			std::vector<float> input_tensor_values;
			input_tensor_values.reserve(256 * 256 * 3);
			for (int y = 0; y < 256; ++y) {
				for (int x = 0; x < 256; ++x) {
					cv::Vec3f pixel = resized.at<cv::Vec3f>(y, x);
					input_tensor_values.push_back(pixel[0]); // R
					input_tensor_values.push_back(pixel[1]); // G
					input_tensor_values.push_back(pixel[2]); // B
				}
			}
			std::vector<Ort::AllocatedStringPtr> output_ptrs;
			std::vector<const char*> output_names;
			get_all_output_names(session, allocator, output_ptrs, output_names);



			std::array<int64_t, 4> input_shape{1, 256, 256, 3};//updated shape
			Ort::MemoryInfo mem_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
			Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
				mem_info, input_tensor_values.data(), input_tensor_values.size(),
				input_shape.data(), input_shape.size()
			);

			// std::cerr << "input_name: " << input_names[0] << "\n";
			// std::cerr << "input_tensor_values size: " << input_tensor_values.size() << "\n";
			// std::cerr << "output_names count: " << output_names.size() << "\n";
			// for (size_t i = 0; i < output_names.size(); ++i){
			// 	std::cerr << "output_name[" << i << "]: " << output_names[i] << "\n";
			// }

			auto output_tensors = session.Run(
				Ort::RunOptions{nullptr},
				input_names.data(), &input_tensor, 1,
				output_names.data(), output_names.size()
			);



			float* output_data = output_tensors[0].GetTensorMutableData<float>();
			std::vector<cv::Point3f> landmarks;
			for (int i = 0; i < 33; ++i) {
				float x = output_data[i * 3 + 0];
				float y = output_data[i * 3 + 1];
				float z = output_data[i * 3 + 2];
				landmarks.emplace_back(x, y, z);
			}
			auto detected_gesture = detect_gestures(landmarks);
			for( auto gesture : detected_gesture) {
				std::cout << gesture << std::endl;
			}
			if (!detected_gesture.empty()) {
				Glib::signal_idle().connect_once([outputBox, detected_gesture]() {
					auto buffer = outputBox->get_buffer();
					for (const auto& gesture : detected_gesture) {
						buffer->insert(buffer->end(), gesture + "\n");
					}
				});
			}

		}

		Glib::signal_idle().connect_once([outputBox] {
			outputBox->get_buffer()->insert(outputBox->get_buffer()->end(), "Test video processing stopped\n");
		});
	}).detach();
}

