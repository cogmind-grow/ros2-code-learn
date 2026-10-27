#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import TransformStamped
from tf2_ros.static_transform_broadcaster import StaticTransformBroadcaster

class StaticTfBroadcasterNode(Node):
    def __init__(self):
        super().__init__("static_tf_broadcaster")
        self.broadcaster_ = StaticTransformBroadcaster(self)

        transform = TransformStamped()
        transform.header.stamp = self.get_clock().now().to_msg()
        transform.header.frame_id = "base_frame"
        transform.child_frame_id = "sensor_frame"
        transform.transform.translation.x = 0.5
        transform.transform.translation.y = 0.0        
        transform.transform.translation.z = 1.0
        transform.transform.rotation.w = 1.0
        self.broadcaster_.sendTransform(transform)
        self.get_logger().info("Static TF broadcaster has been started.")

def main(args=None):
    rclpy.init(args=args)
    node = StaticTfBroadcasterNode()
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ == "__main__":
    main()