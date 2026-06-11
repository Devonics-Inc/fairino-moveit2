#include <moveit/task_constructor/task.h>
#include <moveit/task_constructor/stages/current_state.h>
#include <rclcpp.hpp>

int main(int argc, char** argv)
{

  rclcpp::init(argc,argv);
  auto node_options = rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true);
  auto node = std::make_shared<rclcpp::Node>("mtc_node",node_options);
  RCLCPP_INFO(node->get_logger(), "Initializing ...");
  
  return 0;

}