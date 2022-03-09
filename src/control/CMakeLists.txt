cmake_minimum_required(VERSION 3.0.2)
project(control)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
find_package(OpenCV REQUIRED)
if(NOT OpenCV_FOUND)
   message(FATAL_ERROR "OpenCV > 3 not found.")
endif()

find_package(
  catkin REQUIRED COMPONENTS
  cv_bridge
  image_transport
  roscpp
  std_msgs
  serial
)
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES image_tr
)
link_directories(
  /opt/MVS/lib/64
  /opt/MVS/lib/32
  /usr/local/lib
#   ${TORCH_LIBRARY_DIRS}
)
include_directories(
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  /opt/MVS/include
  ./include/
  include/serial_com
)

add_executable(keyin 
src/interect.cc
src/keyCtrl.cc)

add_executable(chip 
src/receive.cc)

target_link_libraries(keyin
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
  MvCameraControl
  libMvCameraControl.so
  ${CERES_LIBRARIES}
  pthread
)
target_link_libraries(chip
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
)