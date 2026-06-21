# ros2_turtlesim_control

ROS 2 node that makes `turtle2` follow `turtle1` in `turtlesim`.
**Port** of the ROS 1 study project from the *Autonomous Systems* module (2020)
([`ros1_turtlesim_control`](https://github.com/ChristianMartin868/ros1_turtlesim_control)).

> Uses the **ROS 2** API (`rclcpp`, `rclcpp::Node`, wall timer,
> `create_subscription`/`create_publisher`). Built with `ament_cmake` / `colcon`.

## How it works

`src/turtle_control_node.cpp`:

- Subscribes to the poses of both turtles: `turtle1/pose`, `turtle2/pose`
  (`turtlesim/msg/Pose`).
- Computes per iteration:
  - **distance** `dabs = sqrt(dx² + dy²)` between the two turtles,
  - **heading difference** `deltaTheta = atan2(dy, dx) − theta2`.
- Publishes `geometry_msgs/msg/Twist` on `turtle2/cmd_vel`:
  - linear velocity `0.1 · dabs`,
  - angular velocity `0.5 · deltaTheta`.
- **Stop threshold:** when `dabs < 0.8` the forward motion is set to 0.
- Runs at 1 Hz (wall timer instead of `ros::Rate(1)`).

## Topics

| Direction | Topic             | Type                      |
|-----------|-------------------|---------------------------|
| sub       | `turtle1/pose`    | `turtlesim/msg/Pose`      |
| sub       | `turtle2/pose`    | `turtlesim/msg/Pose`      |
| pub       | `turtle2/cmd_vel` | `geometry_msgs/msg/Twist` |

## Build & run

In a colcon workspace (e.g. ROS 2 Humble / Jazzy):

```bash
# Clone the repo into <ws>/src/, then from the workspace root:
colcon build --packages-select ros2_turtlesim_control
source install/setup.bash

# start turtlesim and spawn a second turtle:
ros2 run turtlesim turtlesim_node
ros2 service call /spawn turtlesim/srv/Spawn "{x: 2.0, y: 2.0, theta: 0.0, name: 'turtle2'}"

ros2 run ros2_turtlesim_control turtle_control_node
```

Dependencies: ROS 2 with `turtlesim`, `geometry_msgs`, `rclcpp`.

## Differences from the ROS 1 original

- `ros::init` + global `NodeHandle` → `rclcpp::init` + `rclcpp::Node` subclass.
- `n.subscribe` / `n.advertise` → `create_subscription` / `create_publisher`.
- `while(ros::ok()) { … ros::spinOnce(); loop_rate.sleep(); }` →
  `create_wall_timer(1s, …)` + `rclcpp::spin`.
- `turtlesim::Pose` / `geometry_msgs::Twist` → `turtlesim::msg::Pose` /
  `geometry_msgs::msg::Twist` (headers `…/msg/*.hpp`).
- `ROS_INFO` → `RCLCPP_INFO(get_logger(), …)`.
- Global pose variables → members of the node class.
- Build: catkin (`CMakeLists.txt` + `package.xml` format 2) → ament_cmake
  (`package.xml` format 3, `ament_target_dependencies`, `ament_package`).
