// #include <onnxruntime_cxx_api.h>
// #include <onnxruntime/core/session/onnxruntime_cxx_api.h>
#include <onnxruntime/core/session/onnxruntime_cxx_api.h>

// #include </Users/ch862076/Desktop/body-language-detector/extern/extern/onnxruntime/include/onnxruntime/core/session/onnxruntime_cxx_api.h>
#include <iostream>
#include <filesystem>

int main() {
	std::cout << "Starting minimal ONNX test...\n";

	Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "minimal_test");
	Ort::SessionOptions session_options;
	session_options.SetIntraOpNumThreads(1);

	const char* model_path = "../../models/pose_landmark_full.onnx";
	// const char* model_path = "../adv_inception_v3_Opset16.onnx";
	std::cout << "Checking if model exists: " << model_path << "\n";

	if (!std::filesystem::exists(model_path)) {
		std::cerr << "Model file not found!\n";
		return 1;
	}

	std::cout << "it exists, creating session\n" << std::endl;

	try {
		Ort::Session session(env, model_path, session_options);
		std::cout << "Model loaded successfully!" << std::endl;
		std::cout << "SUCCESS?!!!!" << std::endl;
	} catch (const Ort::Exception& e) {
		std::cerr << "ONNX Runtime error: " << e.what() << std::endl;
	}

	return 0;
}

