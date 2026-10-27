#!/usr/bin/env python3
import math
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import TransformStamped
from tf2_ros import TransformBroadcaster

class TfBroadcasterNode(Node):
    def __init__(self):
        super().__init__("tf_broadcaster")
        self.broadcaster_ = TransformBroadcaster(self)
        self.angle_ = 0.0
        self.timer_ = self.create_timer(0.1, self.broadcast_transform)
        self.get_logger().info("TF broadcaster has been started.")

    def broadcast_transform(self):
        transform = TransformStamped()
        transform.header.stamp = self.get_clock().now().to_msg()
        transform.header.frame_id = "base_frame"
        transform.child_frame_id = "child_frame"
        transform.transform.translation.x = 2.0
        transform.transform.translation.y = math.sin(self.angle_)
        transform.transform.translation.z = 0.0
        transform.transform.rotation.z = math.sin(self.angle_ / 2.0)
        transform.transform.rotation.w = math.cos(self.angle_ / 2.0)
        self.broadcaster_.sendTransform(transform)
        self.angle_ += 0.05

def main(args=None):
    rclpy.init(args=args)
    node = TfBroadcasterNode()
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ == "__main__":
    main()