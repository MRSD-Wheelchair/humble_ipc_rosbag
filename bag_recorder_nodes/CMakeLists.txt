cmake_minimum_required(VERSION 3.8)
project(bag_recorder_nodes)

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(DEPENDENCIES_COMP
    rclcpp
    rclcpp_components
    builtin_interfaces
    sensor_msgs
)

# find dependencies
find_package(ament_cmake REQUIRED)
# ament_auto_find_build_dependencies()

find_package(example_interfaces REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(rosbag2_cpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(sensor_msgs REQUIRED)



# add_executable(simple_bag_recorder src/simple_bag_recorder.cpp)
add_library(simple_bag_recorder_component SHARED src/simple_bag_recorder.cpp)
ament_target_dependencies(simple_bag_recorder_component rclcpp rclcpp_components rosbag2_cpp std_msgs sensor_msgs)

rclcpp_components_register_node(
    simple_bag_recorder_component
    PLUGIN "SimpleBagRecorder"
    EXECUTABLE SimpleBagRecorder
)

ament_export_targets(export_simple_bag_recorder_component)

install(TARGETS
  simple_bag_recorder_component
  EXPORT export_simple_bag_recorder_component
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
