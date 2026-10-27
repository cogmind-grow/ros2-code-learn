#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "action_msgs/msg/goal_status.hpp"
#include "my_robot_interfaces/action/count_until.hpp"

#include <chrono>
#include <functional>
#include <memory>

using CountUntil = my_robot_interfaces::action::CountUntil;
using GoalHandleCountUntil = rclcpp_action::ClientGoalHandle<CountUntil>;
using namespace std::chrono_literals;

class CountUntilClientNode : public rclcpp::Node
{
public:
    CountUntilClientNode() : Node("count_until_client")
    {
        client_ = rclcpp_action::create_client<CountUntil>(this, "count_until");
        RCLCPP_INFO(this->get_logger(), "CountUntil client has been started");
    }

    void sendGoal(int64_t target_count, double period)
    {
        if (!client_->wait_for_action_server(5s)) {
            RCLCPP_ERROR(this->get_logger(), "CountUntil action server not available");
            return;
        }

        CountUntil::Goal goal;
        goal.target_number = target_count;
        goal.period = period;

        rclcpp_action::Client<CountUntil>::SendGoalOptions options;
        options.goal_response_callback =
            std::bind(&CountUntilClientNode::goalResponseCallback, this, std::placeholders::_1);
        options.feedback_callback = std::bind(
            &CountUntilClientNode::feedbackCallback, this, std::placeholders::_1, std::placeholders::_2);
        options.result_callback =
            std::bind(&CountUntilClientNode::resultCallback, this, std::placeholders::_1);

        RCLCPP_INFO(this->get_logger(), "Sending goal: %lld", static_cast<long long>(target_count));
        client_->async_send_goal(goal, options);
        cancel_timer_ = this->create_wall_timer(2s, std::bind(&CountUntilClientNode::cancelGoal, this));
    }

private:
    void goalResponseCallback(const GoalHandleCountUntil::SharedPtr goal_handle)
    {
        if (!goal_handle) {
            RCLCPP_ERROR(this->get_logger(), "Goal was rejected");
            cancel_timer_->cancel();
            return;
        }

        goal_handle_ = goal_handle;
        RCLCPP_INFO(this->get_logger(), "Goal was accepted");
    }

    void cancelGoal()
    {
        cancel_timer_->cancel();
        if (goal_handle_) {
            RCLCPP_INFO(this->get_logger(), "Canceling goal");
            client_->async_cancel_goal(goal_handle_);
        }
    }

    void feedbackCallback(
        GoalHandleCountUntil::SharedPtr,
        const std::shared_ptr<const CountUntil::Feedback> feedback)
    {
        RCLCPP_INFO(this->get_logger(), "Feedback: %lld", static_cast<long long>(feedback->current_number));
    }

    void resultCallback(const GoalHandleCountUntil::WrappedResult & wrapped_result)
    {
        switch (wrapped_result.code) {
            case rclcpp_action::ResultCode::SUCCEEDED:
                RCLCPP_INFO(this->get_logger(), "Goal succeeded");
                break;
            case rclcpp_action::ResultCode::ABORTED:
                RCLCPP_INFO(this->get_logger(), "Goal aborted");
                break;
            case rclcpp_action::ResultCode::CANCELED:
                RCLCPP_INFO(this->get_logger(), "Goal canceled");
                break;
            default:
                RCLCPP_ERROR(this->get_logger(), "Unknown result code");
                break;
        }
        RCLCPP_INFO(this->get_logger(), "Result: %lld",
                    static_cast<long long>(wrapped_result.result->reached_number));
    }

    rclcpp_action::Client<CountUntil>::SharedPtr client_;
    GoalHandleCountUntil::SharedPtr goal_handle_;
    rclcpp::TimerBase::SharedPtr cancel_timer_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<CountUntilClientNode>();
    node->sendGoal(10, 1.0);
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}