#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "my_robot_interfaces/action/count_until.hpp"

#include <chrono>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

using CountUntil = my_robot_interfaces::action::CountUntil;
using GoalHandleCountUntil = rclcpp_action::ServerGoalHandle<CountUntil>;
using namespace std::chrono_literals;

class CountUntilServerNode : public rclcpp::Node
{
public:
    CountUntilServerNode() : Node("count_until_server")
    {
        server_ = rclcpp_action::create_server<CountUntil>(
            this, "count_until",
            std::bind(&CountUntilServerNode::goalCallback, this, std::placeholders::_1, std::placeholders::_2),
            std::bind(&CountUntilServerNode::cancelCallback, this, std::placeholders::_1),
            std::bind(&CountUntilServerNode::handleAcceptedCallback, this, std::placeholders::_1));
        RCLCPP_INFO(this->get_logger(), "CountUntil server has been started");
    }

private:
    rclcpp_action::GoalResponse goalCallback(
        const rclcpp_action::GoalUUID &, std::shared_ptr<const CountUntil::Goal> goal)
    {
        RCLCPP_INFO(this->get_logger(), "Received goal: %lld", static_cast<long long>(goal->target_number));
        if (goal->target_number <= 0) {
            return rclcpp_action::GoalResponse::REJECT;
        }
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }

    rclcpp_action::CancelResponse cancelCallback(const std::shared_ptr<GoalHandleCountUntil>)
    {
        RCLCPP_INFO(this->get_logger(), "Received cancellation request");
        return rclcpp_action::CancelResponse::ACCEPT;
    }

    void handleAcceptedCallback(const std::shared_ptr<GoalHandleCountUntil> goal_handle)
    {
        std::lock_guard<std::mutex> lock(goal_mutex_);
        if (active_goal_) {
            RCLCPP_INFO(this->get_logger(), "Another goal is active; queuing the new goal");
            queued_goals_.push_back(goal_handle);
            return;
        }

        active_goal_ = goal_handle;
        std::thread{std::bind(&CountUntilServerNode::execute, this, std::placeholders::_1), goal_handle}.detach();
    }

    void execute(const std::shared_ptr<GoalHandleCountUntil> goal_handle)
    {
        const auto goal = goal_handle->get_goal();
        auto feedback = std::make_shared<CountUntil::Feedback>();
        auto result = std::make_shared<CountUntil::Result>();
        int64_t count = 0;

        for (int64_t index = 0; index < goal->target_number; ++index) {
            if (goal_handle->is_canceling()) {
                result->reached_number = count;
                goal_handle->canceled(result);
                startNextGoal();
                return;
            }

            count = index + 1;
            feedback->current_number = count;
            goal_handle->publish_feedback(feedback);
            RCLCPP_INFO(this->get_logger(), "Count: %lld", static_cast<long long>(count));
            std::this_thread::sleep_for(std::chrono::duration<double>(goal->period));
        }

        result->reached_number = count;
        goal_handle->succeed(result);
        startNextGoal();
    }

    void startNextGoal()
    {
        std::shared_ptr<GoalHandleCountUntil> next_goal;
        {
            std::lock_guard<std::mutex> lock(goal_mutex_);
            if (queued_goals_.empty()) {
                active_goal_.reset();
                return;
            }
            next_goal = queued_goals_.front();
            queued_goals_.pop_front();
            active_goal_ = next_goal;
        }
        std::thread{std::bind(&CountUntilServerNode::execute, this, std::placeholders::_1), next_goal}.detach();
    }

    rclcpp_action::Server<CountUntil>::SharedPtr server_;
    std::mutex goal_mutex_;
    std::shared_ptr<GoalHandleCountUntil> active_goal_;
    std::deque<std::shared_ptr<GoalHandleCountUntil>> queued_goals_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<CountUntilServerNode>();
    rclcpp::executors::MultiThreadedExecutor executor;
    executor.add_node(node);
    executor.spin();
    rclcpp::shutdown();
    return 0;
}