#include <cstdio>
#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>
#include <geometry_msgs/msg/pose_stamped.hpp>

// this code is hello world example for fairino and ros integrated with the gripper.

// This tutorial focuses on establishing a plan that covers both the gripper and the arm 
// executes this basic plan ! 
int main(int argc, char ** argv)
{
  // initalize ROS2 client, makes global context, that can be used later 
  // to access ROS2 infrastructure, such as topics, services, parameters, etc.
  rclcpp::init(argc,argv);

  printf("hello world hello_fr package\n");


  // create a node options for to be created node.
  rclcpp::NodeOptions options;
  options.automatically_declare_parameters_from_overrides(true);
  // create node using options object
  auto node = std::make_shared<rclcpp::Node>("hello_fr", options);


  // run the node in a seperate thread using executor object.
  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_node(node);
  std::thread([&executor]() { executor.spin(); }).detach();



  // create  a move group interface object.
  moveit::planning_interface::MoveGroupInterface move_group(node, "arm");
  // create a move group interface object for the gripper.
  moveit::planning_interface::MoveGroupInterface gripper_group(node,"gripper");

  geometry_msgs::msg::Pose target_pose;
  // Define orientation using a basic Quaternion (no rotation)
  target_pose.orientation.w = 1.0;
  target_pose.orientation.x = 0.0;
  target_pose.orientation.y = 0.0;
  target_pose.orientation.z = 0.0;

  // Define position target relative to the robot base frame (in meters)
  target_pose.position.x = 0.25;
  target_pose.position.y = 0.1;
  target_pose.position.z = 0.4;

  move_group.setPoseTarget(target_pose);

  moveit::planning_interface::MoveGroupInterface::Plan my_plan;
  auto const logger = rclcpp::get_logger("hello_moveit");
  bool success = (move_group.plan(my_plan) == moveit::core::MoveItErrorCode::SUCCESS);
  if (success) {
    RCLCPP_INFO(logger, "Planning successful, executing the plan...");
    move_group.execute(my_plan);
    RCLCPP_INFO(logger, "Plan executed successfully");
  } else {
    RCLCPP_ERROR(logger, "Planning failed");
  } 

  std::vector<std::string> gripper_joint_names = gripper_group.getJointNames();
  std::vector<double> current_gripper_values = gripper_group.getCurrentJointValues();

  RCLCPP_INFO(logger, "=== Gripper Joints Initial Status ===");
  if (current_gripper_values.empty()) {
    RCLCPP_WARN(logger, "Gripper joint values are empty! Ensure /joint_states is actively publishing.");
  } else {
    for (size_t i = 0; i < gripper_joint_names.size(); ++i) {
      RCLCPP_INFO(logger, "Joint [%zu]: Name = %s | Current Position = %f", 
                  i, gripper_joint_names[i].c_str(), current_gripper_values[i]);
    }
  }
  RCLCPP_INFO(logger, "=====================================");


  std::vector<double> gripper_joints = gripper_group.getCurrentJointValues();
  gripper_joints[0] =  0.4;
  gripper_group.setJointValueTarget(gripper_joints);
  moveit::planning_interface::MoveGroupInterface::Plan gripper_plan;
  bool gripper_success = (gripper_group.plan(gripper_plan) == moveit::core::MoveItErrorCode::SUCCESS);  
  if (gripper_success) {
    RCLCPP_INFO(logger, "Gripper planning successful, executing the plan...");
    gripper_group.execute(gripper_plan);
    RCLCPP_INFO(logger, "Gripper plan executed successfully");
  } else {
    RCLCPP_ERROR(logger, "Gripper planning failed");
  }



  rclcpp::shutdown();
  return 0;
}
