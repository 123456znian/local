cmake_minimum_required(VERSION 2.8.3)
project(follow_bot)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  cv_bridge
  gazebo_ros
  image_transport
  roscpp
  sensor_msgs
  std_msgs
)
find_package(OpenCV)

add_service_files(
  FILES
  CamToReal.srv
)

generate_messages(
  DEPENDENCIES
  std_msgs
)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES follow_bot
 CATKIN_DEPENDS cv_bridge gazebo_ros image_transport roscpp sensor_msgs std_msgs
#  DEPENDS system_lib
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)
add_executable(follow_bot src/Follower.cpp src/main.cpp)

target_link_libraries(follow_bot
   ${catkin_LIBRARIES}
   ${OpenCV_LIBS}
)

add_executable(cam_to_real src/cam_to_real.cpp)
target_link_libraries(cam_to_real ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
add_dependencies(cam_to_real ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})