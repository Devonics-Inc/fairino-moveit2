# Fairino – MoveIt 2 Quick Start Guide

This repository demonstrates how to get started with MoveIt 2 using your Fairino FR collaborative robot, enabling advanced path planning capabilities and visualization of complex robotic scenarios using your real robot code.

The Fairino MoveIt 2 plug-in provides support for motion control, path planning, inverse kinematics solving, and real-time collision detection for Fairino robots. With this integration, users can develop and simulate advanced robotic applications across multiple industries, including industrial automation, welding, manufacturing, automated loading and unloading, palletizing, and medical applications.

---

# Requirements

Before starting, make sure you have the following installed:

1. Ubuntu 22.04.5 LTS  
2. ROS 2 Humble  
3. A Fairino collaborative robot or a Fairino simulation environment (e.g. Fairino SimMachine)
4) Moveit

---

# Installation Steps

## 1. Download the Fairino ROS 2 / MoveIt 2 Plug-in

The MoveIt 2 plug-in contains the required packages for:

- Robot joints defined in URDF
- Hardware communication

You can download the latest release from:

```bash
https://github.com/FAIR-INNOVATION/frcobot_ros2/releases
```

> **Note:**  
> Make sure to install the version compatible with your current WebApp version, whether you are using:
>
> - A physical Fairino cobot
> - The Fairino simulated WebApp environment

---

## 2. Clone the Repository

Clone the repository locally using:

```bash
git clone https://github.com/FAIR-INNOVATION/frcobot_ros2.git
```

---

## 3. Build the Required Packages

Navigate to the ROS workspace:

```bash
cd ~/path/to/plugin/repo/ros_ws
```

Build the `fairino_msgs` package:

```bash
colcon build --packages-select fairino_msgs
```

After the build finishes, source the workspace environment:

```bash
source install/setup.bash
```

This step allows your operating system and ROS environment to access the newly built packages and environment variables.

---

# Next Steps

After successfully building the required packages, you can continue with:

- Building the remaining Fairino ROS 2 packages
- Launching the robot visualization in RViz
- Connecting to your real cobot or simulation environment
- Configuring MoveIt 2 planning pipelines
- Running motion planning examples
