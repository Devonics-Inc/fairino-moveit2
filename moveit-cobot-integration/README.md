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




