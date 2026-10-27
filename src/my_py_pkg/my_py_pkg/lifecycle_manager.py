import rclpy
from rclpy.node import Node
import time
from lifecycle_msgs.srv import ChangeState
from lifecycle_msgs.msg import Transition

class LifecycleManager(Node):
    def __init__(self):
        super().__init__("lifecycle_manager")
        self.get_logger().info("LifecycleManager node started")
        self.declare_parameter("manager_node_name", rclpy.Parameter.Type.STRING)
        node_name = self.get_parameter("manager_node_name").value
        state_name = "/" + node_name + "/change_state"
        self.client = self.create_client(ChangeState, state_name)

    def change_state(self, transition: Transition):
        self.client.wait_for_service()
        self.get_logger().info(f"Requesting state change to {transition.label}")
        request = ChangeState.Request()
        request.transition = transition
        future = self.client.call_async(request)
        rclpy.spin_until_future_complete(self, future)

    def initialize(self):
        self.get_logger().info("LifecycleManager node initialized")
        transition = Transition()
        transition.id= Transition.TRANSITION_CONFIGURE
        transition.label = "configure"
        self.change_state(transition)

        time.sleep(3)

        transition = Transition()
        transition.id= Transition.TRANSITION_ACTIVATE
        transition.label = "activate"
        self.change_state(transition)        

def main(args=None):
    rclpy.init(args=args)
    node = LifecycleManager()
    node.initialize()
    rclpy.shutdown()