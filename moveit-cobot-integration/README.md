# Fairino MoveIt2

This tutorial focuses on the steps required to integrate your Fairino FR collaborative robot with MoveIt2, enabling advanced motion planning, collision avoidance, and safe robot interaction within complex environments.

By following this guide, you will learn how to:

- Configure the Fairino robot for MoveIt2
- Launch and visualize the robot using RViz2
- Perform motion planning and trajectory execution
- Enable obstacle-aware path planning
- Simulate and test robot movements before deployment
- Build a foundation for integrating grippers, sensors, and custom applications

This setup provides a flexible framework for developing safe and intelligent robotic applications using ROS2 and MoveIt2.

# 1. Prerequisites

Before starting this tutorial, make sure you have successfully completed the base MoveIt2 setup tutorial and verified that your Fairino robot environment is functioning correctly.



# 2. Configuration Steps

At this stage, you can either copy the source files into a separate workspace or continue using the existing plugin workspace. If you want to preserve the original tutorial setup, it is recommended to create a backup copy before making any modifications.

Next, navigate to the ROS2 plugin repository corresponding to your Fairino cobot model. Use the path below to access the configuration source files:


```bash
cd ~/path/to/plugin/frcobot_ros2
cd ../
mkdir configured-fr-ws
```
In the same directory, make a new folder, this will be the workspace holding the new configuration to integrate moveit with the actual cobot.

<p align="center">
  <img src="assets/folder.png" width="600"/>
</p>



Now you can move into the configured-fr-ws and make a src subfolder and then copy the configuration file from the original plugin into the src file 

```bash
# Go to the frcobot_ros2 repository
cd ~/path/to/plugin/frcobot_ros2
```

```bash
# Copy the MoveIt config package into your configured workspace src folder
cp -r fairino5_v6_moveit2_config ../configured-fr-ws/src/
cp -r fairino_description ../configured-fr-ws/src/

```

Now we move to the configured-fr-ws and build the configuration files.

```bash
cd ~/path/to/configured-fr-ws

```

```bash
colcon build
source install/setup.bash
```

inside the same directory we can run the moveit configuration tool by running the following command

```bash
ros2 launch moveit_setup_assistant setup_assistant.launch.py
```

