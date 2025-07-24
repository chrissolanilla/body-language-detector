#include <gtkmm.h>
#include "app.h"
#include "pose_runner.h"
#include "detectors.h"
#include "gdkmm/monitor.h"
#include "glibmm/main.h"

#include <filesystem>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <gtkmm/textview.h>
#include <gtkmm/textbuffer.h>
#include <opencv2/dnn.hpp>
#include <onnxruntime/core/session/onnxruntime_cxx_api.h>



int main(int argc, char* argv[]) {
	//test before
	std::cout << "doing simple test" << std::endl;
	Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test_env");
	Ort::SessionOptions opts;
	opts.SetIntraOpNumThreads(1);
	Ort::Session session(env, "../models/pose_landmark_full.onnx", opts);
	std:: cout << "test passed" << std::endl;

    auto app = Gtk::Application::create(argc, argv, "com.example.bodylanguagec");

    Gtk::Window window;
    window.set_title("Body Language C");
    window.set_default_size(400, 300);

    auto content = build_ui();
    window.add(*content);

    window.show_all();

    return app->run(window);
}

