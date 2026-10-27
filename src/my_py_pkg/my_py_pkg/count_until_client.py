import rclpy
from rclpy.node import Node
import time
from rclpy.action import ActionClient
from rclpy.action.client import ClientGoalHandle
from my_robot_interfaces.action import CountUntil
from action_msgs.msg import GoalStatus

class CountUntilClient(Node):
    def __init__(self):
        super().__init__("count_until_client")
        self.get_logger().info("CountUntilClient node has been started")
        self.count_until_client_ = ActionClient(self, CountUntil, "count_until")

    def send_goal(self, target_count: int, period: float):
        goal = CountUntil.Goal()
        goal.target_number = target_count
        goal.period = period
        self.count_until_client_.wait_for_server()
        self.get_logger().info(f"Sending goal: {goal}")

        goal_handle = self.count_until_client_.send_goal_async(goal, feedback_callback=self.goal_feedback_callback).add_done_callback(self.goal_response_callback)
        self.get_logger().info("Goal handle: goal sent")

        self.timers_ = self.create_timer(2.0, self.cancel_goal)

    def cancel_goal(self):
        self.goal_handle_.cancel_goal_async()
        self.get_logger().info("Goal handle: goal canceled")
        self.timers_.cancel()

    def goal_response_callback(self, future):
        self.goal_handle_: ClientGoalHandle = future.result()
        self.get_logger().info("Goal handle: received goal response")
        if self.goal_handle_.accepted:
            self.get_logger().info("Goal handle: goal accepted")
            self.goal_handle_.get_result_async().add_done_callback(self.goal_result_callback)

    def goal_result_callback(self, future):
        status = future.result().status
        result = future.result().result
        if status == GoalStatus.STATUS_SUCCEEDED:
            self.get_logger().info("Goal handle: goal succeeded")
        elif status == GoalStatus.STATUS_ABORTED:
            self.get_logger().info("Goal handle: goal aborted")
        elif status == GoalStatus.STATUS_CANCELED:
            self.get_logger().info("Goal handle: goal canceled")
        
        self.get_logger().info(f"Result: {result.reached_number}")

    def goal_feedback_callback(self, feedback_msg):
        number = feedback_msg.feedback.current_number
        self.get_logger().info(f"Feedback: {number}")

def main():
    rclpy.init()
    node = CountUntilClient()
    node.send_goal(10, 1.0)
    rclpy.spin(node)
    rclpy.shutdown()