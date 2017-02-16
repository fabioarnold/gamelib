cmake_minimum_required(VERSION 2.8)
project(gamelib)

set(SOURCE_FILES
	src/system/files.cpp
	src/system/log.cpp

	src/video/camera.cpp
	src/video/image.cpp
)

add_library(gamelib ${SOURCE_FILES})

target_include_directories(gamelib PUBLIC include)
