cmake_minimum_required(VERSION 3.10)
project(body_language_c LANGUAGES CXX)

find_package(PkgConfig REQUIRED)
find_package(OpenCV REQUIRED core imgproc highgui videoio)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

file(GLOB SOURCES "src/*.cpp")

add_executable(body_language_c ${SOURCES})

target_include_directories(body_language_c PRIVATE
    ${GTKMM_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(body_language_c PRIVATE ${GTKMM_LIBRARIES} ${OpenCV_LIBS})
target_compile_options(body_language_c PRIVATE ${GTKMM_CFLAGS_OTHER})

