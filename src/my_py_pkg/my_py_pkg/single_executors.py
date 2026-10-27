import rclpy
import time
from rclpy.executors import SingleThreadedExecutor
from rclpy.node import Node

class Node1(Node):
    def __init__(self):
        super().__init__("single_node1")
        self.get_logger().info("Hello World!")
        self.create_timer(1.0, self.callback1)
        self.create_timer(1.0, self.callback2)
        self.create_timer(1.0, self.callback3)

    def callback1(self):
        time.sleep(3)
        self.get_logger().info("Node1 callback1")

    def callback2(self):
        time.sleep(3)
        self.get_logger().info("Node1 callback2")

    def callback3(self):
        time.sleep(3)
        self.get_logger().info("Node1 callback3")                

class Node2(Node):
    def __init__(self):
        super().__init__("single_node2")
        self.get_logger().info("Hello World!")
        self.create_timer(1.0, self.callback4)
        self.create_timer(1.0, self.callback5)

    def callback4(self):
        time.sleep(3)
        self.get_logger().info("Node2 callback4")

    def callback5(self):
        time.sleep(3)
        self.get_logger().info("Node2 callback5")


def main(args=None):
    rclpy.init(args=args)
    node1 = Node1()
    node2 = Node2()
    executor = SingleThreadedExecutor()
    executor.add_node(node1)
    executor.add_node(node2)
    executor.spin()
    rclpy.shutdown()