// ROS 2 node: turtle2 follows turtle1 in turtlesim.
// Ported from the ROS 1 node (Aut4 study project, 2020).
#include <chrono>
#include <cmath>
#include <functional>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "turtlesim/msg/pose.hpp"
#include "geometry_msgs/msg/twist.hpp"

using namespace std::chrono_literals;

class TurtleControlNode : public rclcpp::Node
{
public:
  TurtleControlNode()
  : Node("turtle_control_node")
  {
    sub1_ = create_subscription<turtlesim::msg::Pose>(
      "turtle1/pose", 1,
      [this](const turtlesim::msg::Pose::SharedPtr msg) { pose1_ = *msg; });

    sub2_ = create_subscription<turtlesim::msg::Pose>(
      "turtle2/pose", 1,
      [this](const turtlesim::msg::Pose::SharedPtr msg) { pose2_ = *msg; });

    pub_ = create_publisher<geometry_msgs::msg::Twist>("turtle2/cmd_vel", 1);

    // Replaces the ROS 1 ros::Rate(1) + manual while(ros::ok()) loop: a 1 Hz timer.
    timer_ = create_wall_timer(1s, std::bind(&TurtleControlNode::onTimer, this));
  }

private:
  void onTimer()
  {
    const double dx = pose1_.x - pose2_.x;
    const double dy = pose1_.y - pose2_.y;
    const double dabs = std::sqrt(dx * dx + dy * dy);
    RCLCPP_INFO(get_logger(), "deltaAbs: %lf", dabs);

    const double theta = std::atan2(dy, dx);
    const double deltaTheta = theta - pose2_.theta;
    RCLCPP_INFO(get_logger(), "deltaTheta: %lf", deltaTheta);

    geometry_msgs::msg::Twist msg;
    msg.linear.x = 0.1 * dabs;
    msg.linear.y = 0.0;
    msg.linear.z = 0.0;
    msg.angular.x = 0.0;
    msg.angular.y = 0.0;
    msg.angular.z = 0.5 * deltaTheta;

    // Stop threshold: do not drive into turtle1.
    if (dabs < 0.8) {
      msg.linear.x = 0.0;
    }

    pub_->publish(msg);
  }

  turtlesim::msg::Pose pose1_;
  turtlesim::msg::Pose pose2_;
  rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub1_;
  rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub2_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<TurtleControlNode>());
  rclcpp::shutdown();
  return 0;
}
