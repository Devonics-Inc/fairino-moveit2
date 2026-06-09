#include <memory>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>
#include <moveit_msgs/msg/planning_scene.hpp>
#include <moveit_msgs/srv/apply_planning_scene.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <ament_index_cpp/get_package_share_directory.hpp>

struct RobotTarget {
    geometry_msgs::msg::Pose pose;
    double gripper_rad;
    std::string collision_object_to_kill; 
};

class FairinoFileSequenceMover : public rclcpp::Node
{
public:
    FairinoFileSequenceMover() : Node("fairino_file_sequence_mover")
    {
        // Fix start state bounds errors directly via the node's parameters
        this->declare_parameter("allow_start_state_max_bounds_error", true);
        this->declare_parameter("start_state_max_bounds_error", 0.1);

        // Setup the synchronous service client
        apply_scene_client_ = this->create_client<moveit_msgs::srv::ApplyPlanningScene>("/apply_planning_scene");

        std::string package_share_dir = ament_index_cpp::get_package_share_directory("fr_moveit_demos");
        std::string full_path = package_share_dir + "/targets.txt";

        move_thread_ = std::thread(&FairinoFileSequenceMover::executeFileSequence, this, full_path);
    }

    ~FairinoFileSequenceMover()
    {
        if (move_thread_.joinable()) {
            move_thread_.join();
        }
    }

private:
    void setCollisionStateWithObject(const std::string& object_name, bool allow_collision)
    {
        if (object_name.empty() || object_name == "none") {
            return; 
        }

        RCLCPP_INFO(this->get_logger(), "%s collision between entire robot and object: '%s'", 
                    allow_collision ? "Disabling (Killing)" : "Re-enabling", object_name.c_str());

        auto request = std::make_shared<moveit_msgs::srv::ApplyPlanningScene::Request>();
        request->scene.is_diff = true;

        std::string robot_target = "all"; 

        request->scene.allowed_collision_matrix.entry_names.push_back(robot_target);
        
        moveit_msgs::msg::AllowedCollisionEntry entry;
        entry.enabled.push_back(allow_collision); 
        
        request->scene.allowed_collision_matrix.entry_values.push_back(entry);
        request->scene.allowed_collision_matrix.default_entry_names.push_back(object_name);
        request->scene.allowed_collision_matrix.default_entry_values.push_back(allow_collision);

        if (!apply_scene_client_->wait_for_service(std::chrono::seconds(5))) {
            RCLCPP_ERROR(this->get_logger(), "Apply Planning Scene service not available!");
            return;
        }

        auto result = apply_scene_client_->async_send_request(request);
        
        if (rclcpp::spin_until_future_complete(this->get_node_base_interface(), result) != rclcpp::FutureReturnCode::SUCCESS) {
            RCLCPP_ERROR(this->get_logger(), "Failed to apply planning scene update safely.");
        }
    }

    std::vector<RobotTarget> parseTargetsFile(const std::string& filename)
    {
        std::vector<RobotTarget> targets;
        std::ifstream file(filename);

        if (!file.is_open()) {
            RCLCPP_ERROR(this->get_logger(), "CRITICAL: Could not open file: %s", filename.c_str());
            return targets;
        }

        std::string line;
        size_t line_number = 0;
        
        while (std::getline(file, line)) {
            line_number++;
            
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }

            if (line.empty() || line[0] == '#') continue;

            std::stringstream ss(line);
            std::string val;
            std::vector<std::string> parsed_tokens;

            while (std::getline(ss, val, ',')) {
                val.erase(0, val.find_first_not_of(" \t"));
                val.erase(val.find_last_not_of(" \t") + 1);
                if (!val.empty()) {
                    parsed_tokens.push_back(val);
                }
            }

            if (parsed_tokens.size() >= 8) {
                try {
                    RobotTarget t;
                    t.pose.position.x = std::stod(parsed_tokens[0]);
                    t.pose.position.y = std::stod(parsed_tokens[1]);
                    t.pose.position.z = std::stod(parsed_tokens[2]);
                    t.pose.orientation.x = std::stod(parsed_tokens[3]);
                    t.pose.orientation.y = std::stod(parsed_tokens[4]);
                    t.pose.orientation.z = std::stod(parsed_tokens[5]);
                    t.pose.orientation.w = std::stod(parsed_tokens[6]);
                    t.gripper_rad = std::stod(parsed_tokens[7]);
                    
                    if (parsed_tokens.size() == 9) {
                        t.collision_object_to_kill = parsed_tokens[8];
                    } else {
                        t.collision_object_to_kill = "none";
                    }

                    targets.push_back(t);
                } catch (...) {
                    RCLCPP_WARN(this->get_logger(), "Line %zu dropped due to parsing error.", line_number);
                    continue;
                }
            }
        }

        file.close();
        return targets;
    }

    void executeFileSequence(const std::string& filepath)
    {
        std::vector<RobotTarget> sequence = parseTargetsFile(filepath);
        if (sequence.empty()) {
            RCLCPP_ERROR(this->get_logger(), "No valid targets found. Aborting runtime.");
            return;
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));

        auto move_group_arm = std::make_shared<moveit::planning_interface::MoveGroupInterface>(shared_from_this(), "arm");
        auto move_group_gripper = std::make_shared<moveit::planning_interface::MoveGroupInterface>(shared_from_this(), "gripper");
        
        move_group_arm->setPoseReferenceFrame("base_link");
        move_group_arm->setEndEffectorLink("wrist3_link"); 

        move_group_arm->setGoalPositionTolerance(0.005);    
        move_group_arm->setGoalOrientationTolerance(0.01);   
        move_group_arm->setPlanningTime(10.0);              
        move_group_arm->setNumPlanningAttempts(5);          

        moveit::planning_interface::MoveGroupInterface::Plan arm_plan;
        moveit::planning_interface::MoveGroupInterface::Plan gripper_plan;

        size_t step_count = 1;
        for (const auto& target : sequence) {
            RCLCPP_INFO(this->get_logger(), "=== Processing Target File Step %zu / %zu ===", step_count, sequence.size());

            if (target.collision_object_to_kill != "none") {
                setCollisionStateWithObject(target.collision_object_to_kill, true); 
            }

            bool arm_success = planAndExecuteArm(move_group_arm, target.pose, arm_plan);
            
            if (target.collision_object_to_kill != "none") {
                setCollisionStateWithObject(target.collision_object_to_kill, false); 
            }

            if (!arm_success) {
                RCLCPP_ERROR(this->get_logger(), "Step %zu Arm planning failed completely. Aborting.", step_count);
                return;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(250));

            bool grip_success = planAndExecuteGripper(move_group_gripper, target.gripper_rad, gripper_plan);
            if (!grip_success) {
                RCLCPP_ERROR(this->get_logger(), "Step %zu Gripper planning failed. Aborting.", step_count);
                return;
            }

            RCLCPP_INFO(this->get_logger(), "Step %zu reached successfully. Pausing for 2 seconds...", step_count);
            std::this_thread::sleep_for(std::chrono::seconds(2));
            step_count++;
        }
        RCLCPP_INFO(this->get_logger(), "File sequence path completed successfully!");
    }

    bool planAndExecuteArm(std::shared_ptr<moveit::planning_interface::MoveGroupInterface>& move_group,
                           const geometry_msgs::msg::Pose& target,
                           moveit::planning_interface::MoveGroupInterface::Plan& plan)
    {
        move_group->setStartStateToCurrentState();
        move_group->setPoseTarget(target);

        if (move_group->plan(plan) == moveit::core::MoveItErrorCode::SUCCESS) {
            auto execution_result = move_group->execute(plan);
            return (execution_result == moveit::core::MoveItErrorCode::SUCCESS);
        }
        return false;
    }

    bool planAndExecuteGripper(std::shared_ptr<moveit::planning_interface::MoveGroupInterface>& move_group,
                               double target_rad,
                               moveit::planning_interface::MoveGroupInterface::Plan& plan)
    {
        move_group->setStartStateToCurrentState();
        
        std::string active_joint = move_group->getActiveJoints().at(0);
        move_group->setJointValueTarget(active_joint, target_rad);
        
        if (move_group->plan(plan) == moveit::core::MoveItErrorCode::SUCCESS) {
            auto execution_result = move_group->execute(plan);
            return (execution_result == moveit::core::MoveItErrorCode::SUCCESS);
        }
        return false;
    }

    std::thread move_thread_;
    rclcpp::Client<moveit_msgs::srv::ApplyPlanningScene>::SharedPtr apply_scene_client_;
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<FairinoFileSequenceMover>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}