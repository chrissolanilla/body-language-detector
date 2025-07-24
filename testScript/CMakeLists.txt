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

project(test_onnx LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")

# set(ORT_BUILD_DIR "/Users/ch862076/Desktop/body-language-detector/extern/extern/onnxruntime/include")
set(ORT_SOURCE_DIR "/Users/ch862076/Desktop/body-language-detector/extern/extern/onnxruntime")
set(ORT_BUILD_DIR "${ORT_SOURCE_DIR}/build/MacOS/RelWithDebInfo")
set(ORT_INCLUDE_DIR "${ORT_SOURCE_DIR}/include")
# set(ORT_INCLUDE_DIR "${ORT_SOURCE_DIR}/include/onnxruntime/core/session")



include_directories(${ORT_BUILD_DIR}/include)
link_directories(${ORT_BUILD_DIR})
# link_directories(${ORT_BUILD_DIR}/lib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(PkgConfig REQUIRED)
# after
link_directories(${GTKMM_LIBRARY_DIRS})

file(GLOB SOURCES "minimal.cpp")

add_executable(test_onnx ${SOURCES})

target_include_directories(test_onnx PRIVATE
    ${CMAKE_SOURCE_DIR}/include
	${ORT_SOURCE_DIR}/include
)
target_link_libraries(test_onnx PRIVATE   onnxruntime)
# target_compile_options(test_onnx PRIVATE ${GTKMM_CFLAGS_OTHER})


