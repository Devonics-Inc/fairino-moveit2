# Fairino MoveIt2 

This repository contains multiple standalone tutorials demonstrating how to integrate grippers with Fairino collaborative robots using MoveIt2.

The project is organized into separate tutorials so each integration workflow can be followed independently depending on your application requirements.


# Tutorials Overview

## 1. moveit/

This directory contains the base MoveIt2 setup and configuration examples for Fairino robots.

### Includes

- Basic MoveIt2 workspace setup
- Motion planning examples
- Visualization using RViz2
- Fundamental robot configuration files
- Initial MoveIt integration workflow

### Recommended For

- Users starting with MoveIt2 for the first time
- Testing robot motion planning before adding peripherals
- Learning the base architecture of MoveIt2 integration

---

## 2. moveit2-gripper-integration/

This tutorial demonstrates how to integrate a gripper into an existing Fairino MoveIt2 environment.

### Includes

- Gripper URDF integration
- Robot description updates
- Joint and controller configuration
- MoveIt2 planning group updates
- Recompilation and launch process
- End-effector visualization and planning

### Recommended For

- Users adding a gripper to an existing MoveIt2 setup
- Applications requiring pick-and-place operations
- End-effector motion planning and simulation

---

## 3. moveit-cobot-integration/

This directory focuses on the full collaborative robot integration workflow.

### Includes

- Cobot-specific MoveIt2 configuration
- Robot communication setup
- Launch configurations
- Full robot planning pipeline
- Integration structure for real robot deployment

### Recommended For

- Full cobot deployments
- Real robot integration workflows
- Advanced motion planning scenarios

---

# Recommended Learning Path

If you are new to the project, it is recommended to follow the tutorials in this order:

1. `moveit/`
2. `moveit-cobot-integration/`
3. `moveit2-gripper-integration/`

This progression helps establish the base MoveIt2 environment before introducing full cobot and gripper integrations.

---

