import rclpy
from rclpy.node import Node
import time
import threading
from rclpy.action import ActionServer, GoalResponse, CancelResponse
from rclpy.action.server import ServerGoalHandle
from my_robot_interfaces.action import CountUntil
from rclpy.executors import MultiThreadedExecutor
from rclpy.callback_groups import ReentrantCallbackGroup

class CountUntilServer(Node):
    def __init__(self):
        super().__init__("count_until_server")
        self.get_logger().info("CountUntilServer node has been started")
        self.goal_handle_ : ServerGoalHandle = None
        self.goal_lock_ = threading.Lock()
        self.goal_queue_ = []
        self.count_until_server_ = ActionServer(self, CountUntil, "count_until", 
                                                goal_callback=self.goal_callback, 
                                                handle_accepted_callback=self.handle_accepted_callback,
                                                cancel_callback=self.cancel_callback,
                                                execute_callback=self.execute_callback,
                                                callback_group=ReentrantCallbackGroup())
    def goal_callback(self, goal_request: CountUntil.Goal):
        self.get_logger().info(f"Received goal: {goal_request.target_number}")

        # with self.goal_lock_:
        #     if self.goal_handle_ is not None and self.goal_handle_.is_active:
        #         self.get_logger().info("Another goal is active")
        #         return GoalResponse.REJECT
            
        if goal_request.target_number <= 0:
            return GoalResponse.REJECT
        
        # with self.goal_lock_:
        #     if self.goal_handle_ is not None and self.goal_handle_.is_active:
        #         self.get_logger().info("Abort previous goal and accept new goal")
        #         self.goal_handle_.abort()
        #         return GoalResponse.ACCEPT

        self.get_logger().info(f"Goal: {goal_request.target_number}")
        return GoalResponse.ACCEPT
    

    def handle_accepted_callback(self, goal_handle: ServerGoalHandle):
        self.get_logger().info(f"Goal handle accepted: {goal_handle}")
        
        with self.goal_lock_:
            if self.goal_handle_ is not None:
                self.get_logger().info("Another goal is active, queue the goal")
                self.goal_queue_.append(goal_handle)
            else:
                self.get_logger().info("No goal is active, the goal is active")
                goal_handle.execute()
    
    def cancel_callback(self, goal_handle: ServerGoalHandle):
        self.get_logger().info("Goal handle: goal canceled")
        return CancelResponse.ACCEPT

    def execute_callback(self, goal_handle: ServerGoalHandle):
        with self.goal_lock_:
            self.goal_handle_ = goal_handle

        goal = goal_handle.request
        target_count = goal.target_number
        period = goal.period
        self.get_logger().info(f"Received goal: {goal}")

        feed_back_msg = CountUntil.Feedback()

        result = CountUntil.Result()

        count = 0
        for i in range(target_count):
            if not goal_handle.is_active:
                result.reached_number = count
                self.process_next_goal_in_queue()
                return result                
            
            if goal_handle.is_cancel_requested:
                goal_handle.canceled()
                result.reached_number = count
                self.process_next_goal_in_queue()
                return result
            count = i + 1
            self.get_logger().info(f"Count: {count}")
            feed_back_msg.current_number = count
            goal_handle.publish_feedback(feed_back_msg)
            time.sleep(period)
        
        goal_handle.succeed()

        result.reached_number = count

        self.process_next_goal_in_queue()

        return result
    
    def process_next_goal_in_queue(self):
        with self.goal_lock_:
            if len(self.goal_queue_) > 0:
                goal_handle = self.goal_queue_.pop(0)
                goal_handle.execute()
            else:
                self.goal_handle_ = None

def main():
    rclpy.init()
    node = CountUntilServer()
    executor = MultiThreadedExecutor()
    executor.add_node(node)
    executor.spin()
    rclpy.shutdown()