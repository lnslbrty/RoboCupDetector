#####################################
cmake_minimum_required (VERSION 2.8) 
project (raspicam_test)

set(USE_XWINDOW false CACHE BOOL "use X11 for realtime analysis")
set(CMAKE_CXX_FLAGS "-Wall -g -O2 -std=c++11")
set(raspicam_DIR /usr/lib/cmake)

find_package(raspicam REQUIRED)
find_package(OpenCV)

IF (OpenCV_FOUND AND raspicam_CV_FOUND)
IF (USE_XWINDOW)
	add_definitions(-DUSE_XWINDOW=1)
	set(XWINDOW_SOURCES RC_Window.cpp)
ENDIF()

add_executable (robocup RC_Main.cpp RC_CircularBuffer.cpp RC_Camera.cpp RC_BlobObject.cpp RC_BlobDetector.cpp RC_BlobDetectorFactory.cpp ${XWINDOW_SOURCES})
target_link_libraries (robocup ${raspicam_CV_LIBS} pthread)

MESSAGE(STATUS "USE_XWINDOW=${USE_XWINDOW}")
MESSAGE(STATUS "OpenCV found")
ELSE()
MESSAGE(FATAL_ERROR "OpenCV NOT FOUND")
ENDIF()
MESSAGE(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
#####################################
