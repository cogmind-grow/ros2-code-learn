#!/usr/bin/env python3
import rclpy
from geometry_msgs.msg import PointStamped
from rclpy.node import Node
from tf2_geometry_msgs import do_transform_point
from tf2_ros import Buffer, TransformException, TransformListener

class TfListenerNode(Node):
    def __init__(self):
        super().__init__("tf_listener")
        self.buffer_ = Buffer()
        self.listener_ = TransformListener(self.buffer_, self)
        self.timer_ = self.create_timer(0.5, self.lookup_transform)
        self.get_logger().info("TF listener has been started.")

    def lookup_transform(self):
        try:
            transform = self.buffer_.lookup_transform(
                "base_frame", "child_frame", rclpy.time.Time()) #获取两个坐标系之间的变换关系
            translation = transform.transform.translation
            self.get_logger().info(
                f"child_frame position: x={translation.x:.2f}, "
                f"y={translation.y:.2f}, z={translation.z:.2f}")
            self.transform_point(transform)
        except TransformException as exception:
            self.get_logger().warning(f"{exception}")

    def transform_point(self, transform):
        point = PointStamped()
        point.header.frame_id = "child_frame"
        point.header.stamp = transform.header.stamp
        point.point.x = 1.0
        point.point.y = 0.0
        point.point.z = 0.0

        transformed_point = do_transform_point(point, transform) #利用变换关系，转换一个具体的坐标点
        self.get_logger().info(
            f"Point in base_frame: x={transformed_point.point.x:.2f}, "
            f"y={transformed_point.point.y:.2f}, "
            f"z={transformed_point.point.z:.2f}")

def main(args=None):
    rclpy.init(args=args)
    node = TfListenerNode()
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ == "__main__":
    main()