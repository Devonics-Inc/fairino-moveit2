cmake_minimum_required(VERSION 3.8)
project(fr_moveit_demos)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(moveit_ros_planning_interface REQUIRED)

add_executable(hello_fr src/hello_fr.cpp)
target_include_directories(hello_fr PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(hello_fr PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17
ament_target_dependencies(
  hello_fr
  "rclcpp"
  "moveit_ros_planning_interface"
)


add_executable(fr_listener src/fr_listener.cpp)
target_include_directories(fr_listener PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(fr_listener PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17
ament_target_dependencies(
  fr_listener
  "rclcpp"
  "moveit_ros_planning_interface"
)


add_executable(cube_spawner src/cube_spawner.cpp)
target_include_directories(cube_spawner PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(cube_spawner PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17
ament_target_dependencies(
  cube_spawner
  "moveit_ros_planning_interface"
  "rclcpp"
  "geometry_msgs"
  "shape_msgs"
  "moveit_msgs"
)

add_executable(fr_sequencer src/fr_sequencer.cpp)
target_include_directories(fr_sequencer PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(fr_sequencer PUBLIC c_std_99 cxx_std_17)
# Added "ament_index_cpp" to this list
ament_target_dependencies(
  fr_sequencer
  "moveit_ros_planning_interface"
  "rclcpp"
  "geometry_msgs"
  "shape_msgs"
  "moveit_msgs"
  "ament_index_cpp"
)

# Explicitly link the C++ execution threads
target_link_libraries(fr_sequencer Threads::Threads)

install(TARGETS hello_fr fr_listener cube_spawner fr_sequencer
  DESTINATION lib/${PROJECT_NAME})

install(FILES
  targets.txt
  DESTINATION share/${PROJECT_NAME}
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
