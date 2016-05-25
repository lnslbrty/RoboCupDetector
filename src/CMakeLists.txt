#####################################
cmake_minimum_required (VERSION 2.8) 
project (raspicam_test)
set(TEST false CACHE BOOL "test_opt")
set(raspicam_DIR /usr/lib/cmake)
find_package(raspicam REQUIRED)
find_package(OpenCV)

IF (TEST)
	add_definitions(-DTEST)
ENDIF()
add_executable (raspicam_still raspicam_still.cpp)
IF (OpenCV_FOUND AND raspicam_CV_FOUND)
add_executable (robocup RC_Main.cpp RC_CircularBuffer.cpp RC_Camera.cpp)
add_executable (raspicam_test raspicam_test.cpp)
target_link_libraries (robocup ${raspicam_CV_LIBS})
target_link_libraries (raspicam_test ${raspicam_CV_LIBS})
MESSAGE(STATUS "COMPILING OPENCV TESTS")
ELSE()
MESSAGE(FATAL_ERROR "OPENCV NOT FOUND IN YOUR SYSTEM")
ENDIF()

target_link_libraries (raspicam_still ${raspicam_LIBS})
#####################################
