cmake_minimum_required(VERSION 3.10)
# Detect Apple Silicon
if(APPLE)
    execute_process(COMMAND uname -m OUTPUT_VARIABLE ARCHITECTURE OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(ARCHITECTURE STREQUAL "arm64")
        message(STATUS "Building for Apple Silicon (ARM64)")
        set(CMAKE_OSX_ARCHITECTURES arm64)
    else()
        message(STATUS "Building for Intel (x86_64)")
        set(CMAKE_OSX_ARCHITECTURES x86_64)
    endif()
endif()

project(body_language_c LANGUAGES CXX)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")

# set(ONNXRUNTIME_DIR "/Users/ch862076/brew/Cellar/onnxruntime/1.22.1")

set(ORT_SOURCE_DIR "/Users/ch862076/Desktop/body-language-detector/extern/extern/onnxruntime")
set(ORT_BUILD_DIR "${ORT_SOURCE_DIR}/build/MacOS/RelWithDebInfo")
set(ORT_INCLUDE_DIR "${ORT_SOURCE_DIR}/include")

include_directories(${ORT_BUILD_DIR}/include)
link_directories(${ORT_BUILD_DIR})


# include_directories(${ONNXRUNTIME_DIR}/include)
# link_directories(${ONNXRUNTIME_DIR}/lib)
set(OpenCV_DIR "${CMAKE_SOURCE_DIR}/extern/extern/opencv/build")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(PkgConfig REQUIRED)
find_package(OpenCV REQUIRED core imgproc highgui videoio dnn)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)
# after
link_directories(${GTKMM_LIBRARY_DIRS})

file(GLOB SOURCES "src/*.cpp")

add_executable(body_language_c ${SOURCES})

target_include_directories(body_language_c PRIVATE
    ${GTKMM_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
	${ORT_SOURCE_DIR}/include

	# ${ONNXRUNTIME_DIR}/include/onnxruntime
)
target_link_libraries(body_language_c PRIVATE ${GTKMM_LIBRARIES} ${OpenCV_LIBS} onnxruntime)
target_compile_options(body_language_c PRIVATE ${GTKMM_CFLAGS_OTHER})

