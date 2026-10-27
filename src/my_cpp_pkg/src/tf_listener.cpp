#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/point_stamped.hpp"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"
#include "tf2_ros/buffer.h"
#include "tf2_ros/transform_listener.h"

using namespace std::chrono_literals;

class TfListenerNode : public rclcpp::Node
{
public:
    TfListenerNode() : Node("tf_listener")
    {
        buffer_ = std::make_unique<tf2_ros::Buffer>(get_clock());
        listener_ = std::make_shared<tf2_ros::TransformListener>(*buffer_);
        timer_ = create_wall_timer(500ms, std::bind(&TfListenerNode::lookupTransform, this));
        RCLCPP_INFO(get_logger(), "TF listener has been started.");
    }

private:
    void lookupTransform()
    {
        try {
            auto transform = buffer_->lookupTransform("base_frame", "child_frame", tf2::TimePointZero);
            RCLCPP_INFO(get_logger(), "child_frame position: x=%.2f, y=%.2f, z=%.2f",
                        transform.transform.translation.x,
                        transform.transform.translation.y,
                        transform.transform.translation.z);
            transformPoint(transform);
        } catch (const tf2::TransformException &exception) {
            RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 2000, "%s", exception.what());
        }
    }

    void transformPoint(const geometry_msgs::msg::TransformStamped &transform)
    {
        geometry_msgs::msg::PointStamped point;
        point.header.frame_id = "child_frame";
        point.header.stamp = transform.header.stamp;
        point.point.x = 1.0;
        point.point.y = 0.0;
        point.point.z = 0.0;

        geometry_msgs::msg::PointStamped transformed_point;
        tf2::doTransform(point, transformed_point, transform);
        RCLCPP_INFO(get_logger(), "Point in base_frame: x=%.2f, y=%.2f, z=%.2f",
                    transformed_point.point.x,
                    transformed_point.point.y,
                    transformed_point.point.z);
    }

    std::unique_ptr<tf2_ros::Buffer> buffer_;
    std::shared_ptr<tf2_ros::TransformListener> listener_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TfListenerNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}