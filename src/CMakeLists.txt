#####################################
cmake_minimum_required (VERSION 2.8) 
project (raspicam_test)

set(USE_XWINDOW false CACHE BOOL "use X11 for realtime analysis")
set(CFLAGS "-Wall -g -O2" CACHE STRING "cflags")
set(raspicam_DIR /usr/lib/cmake)

find_package(raspicam REQUIRED)
find_package(OpenCV)

IF (OpenCV_FOUND AND raspicam_CV_FOUND)
IF (USE_XWINDOW)
	add_definitions(-DUSE_XWINDOW=1)
	set(XWINDOW_SOURCES RC_Window.cpp)
ENDIF()

add_executable (robocup RC_Main.cpp RC_CircularBuffer.cpp RC_Camera.cpp RC_BlobObject.cpp RC_BlobDetector.cpp ${XWINDOW_SOURCES})
add_executable (raspicam_test raspicam_test.cpp)
add_executable (raspicam_still raspicam_still.cpp)

target_link_libraries (robocup ${raspicam_CV_LIBS})
target_link_libraries (raspicam_test ${raspicam_CV_LIBS})
target_link_libraries (raspicam_still ${raspicam_LIBS})

MESSAGE(STATUS "OpenCV found")
ELSE()
MESSAGE(FATAL_ERROR "OpenCV NOT FOUND")
ENDIF()
MESSAGE(STATUS "CFLAGS: ${CFLAGS}")
#####################################
