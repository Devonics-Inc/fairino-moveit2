cmake_minimum_required(VERSION 3.22)
project(fairino_gripper_integrated_desc)

find_package(ament_cmake REQUIRED)

# Safely install the URDF and Mesh assets to the ROS shared install path
install(DIRECTORY 
  urdf
  meshes
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
