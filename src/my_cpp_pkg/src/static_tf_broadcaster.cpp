#include "geometry_msgs/msg/transform_stamped.hpp"
#include "rclcpp/rclcpp.hpp"
#include "tf2_ros/static_transform_broadcaster.h"

class StaticTfBroadcasterNode : public rclcpp::Node
{
public:
    StaticTfBroadcasterNode() : Node("static_tf_broadcaster")
    {
        broadcaster_ = std::make_unique<tf2_ros::StaticTransformBroadcaster>(*this);

        geometry_msgs::msg::TransformStamped transform;
        transform.header.stamp = get_clock()->now();
        transform.header.frame_id = "base_frame";
        transform.child_frame_id = "sensor_frame";
        transform.transform.translation.x = 0.5;
        transform.transform.translation.y = 0.0;
        transform.transform.translation.z = 1.0;
        transform.transform.rotation.w = 1.0;
        broadcaster_->sendTransform(transform);

        RCLCPP_INFO(get_logger(), "Static TF broadcaster has been started.");
    }

private:
    std::unique_ptr<tf2_ros::StaticTransformBroadcaster> broadcaster_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<StaticTfBroadcasterNode>();    
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}