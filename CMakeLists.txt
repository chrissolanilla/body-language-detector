cmake_minimum_required(VERSION 3.10)
project(body_language_c LANGUAGES CXX)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)

file(GLOB SOURCES "src/*.cpp")

add_executable(body_language_c ${SOURCES})

target_include_directories(body_language_c PRIVATE
	${GTK_INCLUDE_DIRS}
	${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(body_language_c PRIVATE ${GTK_LIBRARIES})
target_compile_options(body_language_c PRIVATE ${GTK_CFLAGS_OTHER})
