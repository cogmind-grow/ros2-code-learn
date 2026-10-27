cmake_minimum_required(VERSION 3.8)
project(my_robot_controllers)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclpy REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)
find_package(controller_interface REQUIRED)
find_package(pluginlib REQUIRED)
find_package(example_interfaces REQUIRED)

add_library(
  ${PROJECT_NAME}
  SHARED
  src/my_controller.cpp
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

ament_target_dependencies(${PROJECT_NAME}
  rclcpp
  rclcpp_lifecycle
  controller_interface
  pluginlib
  example_interfaces
)

pluginlib_export_plugin_description_file(controller_interface my_robot_plugin.xml)

install(
  TARGETS ${PROJECT_NAME}
  DESTINATION lib/${PROJECT_NAME}
)

install(
  DIRECTORY include
  DESTINATION include/${PROJECT_NAME}
)

install(
  FILES my_robot_plugin.xml
  DESTINATION share/${PROJECT_NAME}
)

ament_export_libraries(${PROJECT_NAME})

ament_export_include_directories(
  include
)

ament_export_dependencies(
  rclcpp
  rclcpp_lifecycle
  controller_interface
  pluginlib
  example_interfaces
)

ament_package()