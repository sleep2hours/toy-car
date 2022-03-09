cmake_minimum_required(VERSION 3.0.2)
project(img)
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
)

aux_source_directory(src/aim_cam CAM_SRC)
add_executable(imgtr src/image_trans.cpp
${CAM_SRC}
)
add_executable(imgcv src/image_convert.cpp)
target_link_libraries(imgtr
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
  MvCameraControl
  libMvCameraControl.so
  ${CERES_LIBRARIES}
  pthread
)
target_link_libraries(imgcv
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
)