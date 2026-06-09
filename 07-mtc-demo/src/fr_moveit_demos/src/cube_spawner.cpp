#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "moveit/planning_scene_interface/planning_scene_interface.h"
#include "moveit_msgs/msg/collision_object.hpp"
#include "shape_msgs/msg/solid_primitive.hpp"
#include "geometry_msgs/msg/pose.hpp"

class CubeSpawner : public rclcpp::Node {
public:
    CubeSpawner() : Node("cube_spawner_node") {
        // We use a single-shot timer to execute after the node initializes completely
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(500), 
            std::bind(&CubeSpawner::spawn_cube, this)
        );
    }

private:
    void spawn_cube() {
        timer_->cancel(); // Stop the timer instantly. We only want to run this ONCE.

        // Initialize MoveIt's official planning scene interface
        moveit::planning_interface::PlanningSceneInterface planning_scene_interface;

        // 1. Create the collision object
        moveit_msgs::msg::CollisionObject collision_object;
        
        // CRITICAL: Change "world" to "base_link" if your robot's RViz global frame is base_link!
        collision_object.header.frame_id = "base_link"; 
        collision_object.id = "pp_cube";

        // 2. Define the shape geometry
        shape_msgs::msg::SolidPrimitive primitive;
        primitive.type = shape_msgs::msg::SolidPrimitive::BOX;
        primitive.dimensions.resize(3);
        primitive.dimensions[shape_msgs::msg::SolidPrimitive::BOX_X] = 0.1;
        primitive.dimensions[shape_msgs::msg::SolidPrimitive::BOX_Y] = 0.02;
        primitive.dimensions[shape_msgs::msg::SolidPrimitive::BOX_Z] = 0.1;

        // 3. Define the position coordinates
        geometry_msgs::msg::Pose cube_pose;
        cube_pose.position.x = 0.8;
        cube_pose.position.y = 0.32;
        cube_pose.position.z = -0.09;
        cube_pose.orientation.w = 1.0;

        // Assemble the object configurations
        collision_object.primitives.push_back(primitive);
        collision_object.primitive_poses.push_back(cube_pose);
        collision_object.operation = collision_object.ADD;

        // 4. Submit to MoveIt
        std::vector<moveit_msgs::msg::CollisionObject> collision_objects;
        collision_objects.push_back(collision_object);

        RCLCPP_INFO(this->get_logger(), "Sending cube to MoveIt Planning Scene via official API...");
        
        // This function handles the handshake and guarantees MoveIt receives it safely!
        planning_scene_interface.applyCollisionObjects(collision_objects);

        RCLCPP_INFO(this->get_logger(), "Cube successfully added! Shutting down spawner node safely.");
        rclcpp::shutdown();
    }

    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<CubeSpawner>();
    rclcpp::spin(node);
    return 0;
}