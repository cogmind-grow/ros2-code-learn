#include <cmath>

#include "geometry_msgs/msg/transform_stamped.hpp"
#include "rclcpp/rclcpp.hpp"
#include "tf2/LinearMath/Quaternion.h"
#include "tf2_ros/transform_broadcaster.h"

using namespace std::chrono_literals;

class TfBroadcasterNode : public rclcpp::Node
{
public:
    TfBroadcasterNode() : Node("tf_broadcaster"), angle_(0.0)
    {
        broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(*this);
        timer_ = create_wall_timer(100ms, std::bind(&TfBroadcasterNode::broadcastTransform, this));
        RCLCPP_INFO(get_logger(), "TF broadcaster has been started.");
    }

private:
    void broadcastTransform()
    {
        geometry_msgs::msg::TransformStamped transform;
        transform.header.stamp = get_clock()->now();
        transform.header.frame_id = "base_frame";
        transform.child_frame_id = "child_frame";
        transform.transform.translation.x = 2.0;
        transform.transform.translation.y = std::sin(angle_);
        transform.transform.translation.z = 0.0;

        tf2::Quaternion rotation;
        rotation.setRPY(0.0, 0.0, angle_);
        transform.transform.rotation.x = rotation.x();
        transform.transform.rotation.y = rotation.y();
        transform.transform.rotation.z = rotation.z();
        transform.transform.rotation.w = rotation.w();

        broadcaster_->sendTransform(transform);
        angle_ += 0.05;
    }

    std::unique_ptr<tf2_ros::TransformBroadcaster> broadcaster_;
    rclcpp::TimerBase::SharedPtr timer_;
    double angle_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TfBroadcasterNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}