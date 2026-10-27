import rclpy
from rclpy.lifecycle import LifecycleNode
from rclpy.lifecycle.node import LifecycleState, TransitionCallbackReturn
from example_interfaces.msg import Int64

class NumberPublisher(LifecycleNode):
    def __init__(self):
        super().__init__("number_publisher")
        self.get_logger().info("In constructor")
        self.number_ = 1
        self.frequency_ = 1.0
        self.number_publisher_ = None
        self.number_timer_ = None        

    def on_configure(self, previous_state: LifecycleState):
        self.get_logger().info("In on_configure")

        self.number_publisher_ = self.create_lifecycle_publisher(Int64, "number", 10)
        self.number_timer_ = self.create_timer(1.0 / self.frequency_, self.timer_callback)
        self.number_timer_.cancel()

        return TransitionCallbackReturn.SUCCESS
    
    def on_activate(self, previous_state: LifecycleState):
        self.get_logger().info("In on_activate")
        self.number_timer_.reset()

        return super().on_activate(previous_state)
    
    def on_deactivate(self, previous_state: LifecycleState):
        self.get_logger().info("In on_deactivate")
        self.number_timer_.cancel()

        return super().on_deactivate(previous_state)
    
    def on_cleanup(self, previous_state: LifecycleState):
        self.get_logger().info("In on_cleanup")
        if self.number_timer_ is not None:
            self.number_timer_.cancel()       

        if self.number_publisher_ is not None:
            self.destroy_publisher(self.number_publisher_)
            self.number_publisher_ = None

        if self.number_timer_ is not None:
            self.destroy_timer(self.number_timer_)
            self.number_timer_ = None
        self.number_ = 1

        return TransitionCallbackReturn.SUCCESS
    
    def on_shutdown(self, previous_state: LifecycleState):
        self.get_logger().info("In on_shutdown")
        if self.number_timer_ is not None:
            self.number_timer_.cancel()      

        if self.number_publisher_ is not None:
            self.destroy_publisher(self.number_publisher_)
            self.number_publisher_ = None

        if self.number_timer_ is not None:
            self.destroy_timer(self.number_timer_)
            self.number_timer_ = None
        self.number_ = 1

        return TransitionCallbackReturn.SUCCESS        

    def on_error(self, previous_state: LifecycleState):
        self.get_logger().info("In on_error")
        if self.number_timer_ is not None:
            self.number_timer_.cancel()      

        if self.number_publisher_ is not None:
            self.destroy_publisher(self.number_publisher_)
            self.number_publisher_ = None

        if self.number_timer_ is not None:
            self.destroy_timer(self.number_timer_)
            self.number_timer_ = None
        self.number_ = 1

        return TransitionCallbackReturn.SUCCESS      

    def timer_callback(self):
        msg = Int64()
        msg.data = self.number_
        self.number_publisher_.publish(msg)
        self.number_ += 1
        self.get_logger().info(f"Published number: {msg.data}")

def main(args=None):
    rclpy.init(args=args)
    node = NumberPublisher()
    rclpy.spin(node)
    rclpy.shutdown()