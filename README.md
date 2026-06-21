# ros2_turtlesim_control

ROS-2-Node, mit der `turtle2` der `turtle1` in `turtlesim` folgt.
**Portierung** des ROS-1-Studienprojekts aus dem Modul *Autonome Systeme* (2020)
([`ros1_turtlesim_control`](https://github.com/ChristianMartin868/ros1_turtlesim_control)).

> Verwendet die **ROS 2**-API (`rclcpp`, `rclcpp::Node`, Wall-Timer,
> `create_subscription`/`create_publisher`). Gebaut mit `ament_cmake` / `colcon`.

## Funktionsweise

`src/turtle_control_node.cpp`:

- Abonniert die Posen beider Schildkröten: `turtle1/pose`, `turtle2/pose`
  (`turtlesim/msg/Pose`).
- Berechnet pro Iteration:
  - **Abstand** `dabs = sqrt(dx² + dy²)` zwischen beiden Schildkröten,
  - **Winkeldifferenz** `deltaTheta = atan2(dy, dx) − theta2`.
- Veröffentlicht `geometry_msgs/msg/Twist` auf `turtle2/cmd_vel`:
  - Lineargeschwindigkeit `0.1 · dabs`,
  - Winkelgeschwindigkeit `0.5 · deltaTheta`.
- **Stoppschwelle:** bei `dabs < 0.8` wird die Vorwärtsbewegung auf 0 gesetzt.
- Läuft mit 1 Hz (Wall-Timer statt `ros::Rate(1)`).

## Topics

| Richtung | Topic             | Typ                       |
|----------|-------------------|---------------------------|
| sub      | `turtle1/pose`    | `turtlesim/msg/Pose`      |
| sub      | `turtle2/pose`    | `turtlesim/msg/Pose`      |
| pub      | `turtle2/cmd_vel` | `geometry_msgs/msg/Twist` |

## Bauen & Ausführen

In einem colcon-Workspace (z. B. ROS 2 Humble / Jazzy):

```bash
# Repo nach <ws>/src/ klonen, dann im Workspace-Root:
colcon build --packages-select ros2_turtlesim_control
source install/setup.bash

# turtlesim starten und zweite Schildkröte spawnen:
ros2 run turtlesim turtlesim_node
ros2 service call /spawn turtlesim/srv/Spawn "{x: 2.0, y: 2.0, theta: 0.0, name: 'turtle2'}"

ros2 run ros2_turtlesim_control turtle_control_node
```

Abhängigkeiten: ROS 2 mit `turtlesim`, `geometry_msgs`, `rclcpp`.

## Unterschiede zum ROS-1-Original

- `ros::init` + globale `NodeHandle` → `rclcpp::init` + `rclcpp::Node`-Subklasse.
- `n.subscribe` / `n.advertise` → `create_subscription` / `create_publisher`.
- `while(ros::ok()) { … ros::spinOnce(); loop_rate.sleep(); }` →
  `create_wall_timer(1s, …)` + `rclcpp::spin`.
- `turtlesim::Pose` / `geometry_msgs::Twist` → `turtlesim::msg::Pose` /
  `geometry_msgs::msg::Twist` (Header `…/msg/*.hpp`).
- `ROS_INFO` → `RCLCPP_INFO(get_logger(), …)`.
- Globale Pose-Variablen → Member der Node-Klasse.
- Build: catkin (`CMakeLists.txt` + `package.xml` format 2) → ament_cmake
  (`package.xml` format 3, `ament_target_dependencies`, `ament_package`).
