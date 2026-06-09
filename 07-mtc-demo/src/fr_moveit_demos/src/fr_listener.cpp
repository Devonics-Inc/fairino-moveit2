#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <tf2_ros/transform_listener.h>
#include <tf2_ros/buffer.h>

#include <chrono>
#include <memory>
#include <string>
#include <algorithm>

using namespace std::chrono_literals;

class FRRobotListener : public rclcpp::Node
{
public:
    FRRobotListener(const rclcpp::NodeOptions & options)
    : Node("fr_listener", options)
    {
        // 1. Initialize TF2 Buffer and Listener to capture TCP coordinates
        tf_buffer_ = std::make_unique<tf2_ros::Buffer>(this->get_clock());
        tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

        // 2. Subscribe to the live Joint States topic for the gripper values
        joint_state_sub_ = this->create_subscription<sensor_msgs::msg::JointState>(
            "/joint_states", 10,
            std::bind(&FRRobotListener::jointStateCallback, this, std::placeholders::_1)
        );

        // 3. Create a periodic timer to print the TCP pose at 2Hz (every 500ms)
        timer_ = this->create_wall_timer(
            2000ms, std::bind(&FRRobotListener::timerCallback, this)
        );

        RCLCPP_INFO(this->get_logger(), "FR Listener Node has been initialized.");
    }

private:
    // Callback to monitor the gripper's active joint value
    void jointStateCallback(const sensor_msgs::msg::JointState::SharedPtr msg)
    {
        // Find the specific active driver joint of your AG95 gripper
        auto it = std::find(msg->name.begin(), msg->name.end(), "gripper_left_outer_knuckle_joint");
        
        if (it != msg->name.end()) {
            size_t index = std::distance(msg->name.begin(), it);
            double joint_position = msg->position[index];
            
            // Optional: Convert raw joint angle to approximate millimeters if desired
            RCLCPP_INFO_THROTTLE(this->get_logger(), *this->get_clock(), 1000,
                "Gripper Active Joint State: %.4f rad", joint_position);
        }
    }

    // Timer loop to safely look up and calculate the TCP frame transformation
    void timerCallback()
    {
        geometry_msgs::msg::TransformStamped transformStamped;

        // FIX: The base frame is our static global frame, the tool is our tracking frame
        std::string global_frame = "base_link"; 
        
        // TIP: Change "wrist3_link" to "tool0" or "flange" if MoveIt expects the tool endpoint!
        std::string tool_frame = "wrist3_link"; 

        try {
            // FIX: Pass the global reference frame FIRST, and the moving tool frame SECOND
            transformStamped = tf_buffer_->lookupTransform(
                global_frame, tool_frame, tf2::TimePointZero);
            
            // Extract coordinates
            double x = transformStamped.transform.translation.x;
            double y = transformStamped.transform.translation.y;
            double z = transformStamped.transform.translation.z;

            // Extract orientation quaternions
            double qx = transformStamped.transform.rotation.x;
            double qy = transformStamped.transform.rotation.y;
            double qz = transformStamped.transform.rotation.z;
            double qw = transformStamped.transform.rotation.w;

            RCLCPP_INFO(this->get_logger(), 
                "%.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f", 
                x, y, z, qx, qy, qz, qw);

        } catch (const tf2::TransformException & ex) {
            RCLCPP_WARN(this->get_logger(), "Could not transform %s to %s: %s", 
                global_frame.c_str(), tool_frame.c_str(), ex.what());
        }
    }
    // Class members
    std::unique_ptr<tf2_ros::Buffer> tf_buffer_;
    std::shared_ptr<tf2_ros::TransformListener> tf_listener_;
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr joint_state_sub_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);
    
    rclcpp::NodeOptions options;
    options.automatically_declare_parameters_from_overrides(true);
    
    // Spin the node natively using the cleaner object-oriented architecture
    rclcpp::spin(std::make_shared<FRRobotListener>(options));
    
    rclcpp::shutdown();
    return 0;
}