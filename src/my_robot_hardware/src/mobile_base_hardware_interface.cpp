#include "my_robot_hardware/mobile_base_hardware_interface.hpp"

namespace mobile_base_hardware {

hardware_interface::CallbackReturn MobileBaseHardwareInterface::on_init
    (const hardware_interface::HardwareInfo & info)
{
    if (hardware_interface::SystemInterface::on_init(info) !=
        hardware_interface::CallbackReturn::SUCCESS)
    {
        return hardware_interface::CallbackReturn::ERROR;
    }

    info_ = info;

    left_motor_id_ = 10;
    right_motor_id_ = 20;
    port_ = "/dev/ttyACM0";

    driver_ = std::make_shared<XL330Driver>(port_);

    left_wheel_position_ = 0.0;
    left_wheel_velocity_ = 0.0;
    right_wheel_position_ = 0.0;
    right_wheel_velocity_ = 0.0;
    left_wheel_command_ = 0.0;
    right_wheel_command_ = 0.0;

    return hardware_interface::CallbackReturn::SUCCESS;
}

std::vector<hardware_interface::StateInterface> MobileBaseHardwareInterface::export_state_interfaces()
{
    std::vector<hardware_interface::StateInterface> state_interfaces;

    state_interfaces.emplace_back(hardware_interface::StateInterface(
        "base_left_wheel_joint",
        hardware_interface::HW_IF_POSITION,
        &left_wheel_position_));
    state_interfaces.emplace_back(hardware_interface::StateInterface(
        "base_left_wheel_joint",
        hardware_interface::HW_IF_VELOCITY,
        &left_wheel_velocity_));
    state_interfaces.emplace_back(hardware_interface::StateInterface(
        "base_right_wheel_joint",
        hardware_interface::HW_IF_POSITION,
        &right_wheel_position_));
    state_interfaces.emplace_back(hardware_interface::StateInterface(
        "base_right_wheel_joint",
        hardware_interface::HW_IF_VELOCITY,
        &right_wheel_velocity_));

    return state_interfaces;
}

std::vector<hardware_interface::CommandInterface> MobileBaseHardwareInterface::export_command_interfaces()
{
    std::vector<hardware_interface::CommandInterface> command_interfaces;

    command_interfaces.emplace_back(hardware_interface::CommandInterface(
        "base_left_wheel_joint",
        hardware_interface::HW_IF_VELOCITY,
        &left_wheel_command_));
    command_interfaces.emplace_back(hardware_interface::CommandInterface(
        "base_right_wheel_joint",
        hardware_interface::HW_IF_VELOCITY,
        &right_wheel_command_));

    return command_interfaces;
}

hardware_interface::CallbackReturn MobileBaseHardwareInterface::on_configure
    (const rclcpp_lifecycle::State & previous_state)
{
    (void)previous_state;
    if (driver_->init() != 0) {
        return hardware_interface::CallbackReturn::ERROR;
    }
    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn MobileBaseHardwareInterface::on_activate
    (const rclcpp_lifecycle::State & previous_state)
{
    (void)previous_state;
    set_state("base_left_wheel_joint/velocity", 0.0);
    set_state("base_right_wheel_joint/velocity", 0.0);
    set_state("base_left_wheel_joint/position", 0.0);
    set_state("base_right_wheel_joint/position", 0.0);    
    driver_->activateWithVelocityMode(left_motor_id_);
    driver_->activateWithVelocityMode(right_motor_id_);
    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn MobileBaseHardwareInterface::on_deactivate
    (const rclcpp_lifecycle::State & previous_state)
{
    (void)previous_state;
    driver_->deactivate(left_motor_id_);
    driver_->deactivate(right_motor_id_);
    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::return_type MobileBaseHardwareInterface::read
    (const rclcpp::Time & time, const rclcpp::Duration & period)
{
    (void)time;
    double left_vel = driver_->getVelocityRadianPerSec(left_motor_id_);
    double right_vel = driver_->getVelocityRadianPerSec(right_motor_id_);
    set_state("base_left_wheel_joint/velocity", left_vel);
    set_state("base_right_wheel_joint/velocity", right_vel);
    set_state("base_left_wheel_joint/position", get_state("base_left_wheel_joint/position") + left_vel * period.seconds()); // 
    set_state("base_right_wheel_joint/position", get_state("base_right_wheel_joint/position") + right_vel * period.seconds()); 
    return hardware_interface::return_type::OK;
}

hardware_interface::return_type MobileBaseHardwareInterface::write
    (const rclcpp::Time & time, const rclcpp::Duration & period)
{
    (void)time;
    (void)period;
    driver_->setTargetVelocityRadianPerSec(left_motor_id_, get_command("base_left_wheel_joint/velocity"));
    driver_->setTargetVelocityRadianPerSec(right_motor_id_, get_command("base_right_wheel_joint/velocity"));
    return hardware_interface::return_type::OK;
}

void MobileBaseHardwareInterface::set_state(const std::string & name, double value)
{
    if (name == "base_left_wheel_joint/position") {
        left_wheel_position_ = value;
    } else if (name == "base_left_wheel_joint/velocity") {
        left_wheel_velocity_ = value;
    } else if (name == "base_right_wheel_joint/position") {
        right_wheel_position_ = value;
    } else if (name == "base_right_wheel_joint/velocity") {
        right_wheel_velocity_ = value;
    }
}

double MobileBaseHardwareInterface::get_state(const std::string & name) const
{
    if (name == "base_left_wheel_joint/position") {
        return left_wheel_position_;
    } else if (name == "base_left_wheel_joint/velocity") {
        return left_wheel_velocity_;
    } else if (name == "base_right_wheel_joint/position") {
        return right_wheel_position_;
    } else if (name == "base_right_wheel_joint/velocity") {
        return right_wheel_velocity_;
    }
    return 0.0;
}

double MobileBaseHardwareInterface::get_command(const std::string & name) const
{
    if (name == "base_left_wheel_joint/velocity") {
        return left_wheel_command_;
    } else if (name == "base_right_wheel_joint/velocity") {
        return right_wheel_command_;
    }
    return 0.0;
}

} // namespace mobile_base_hardware

#include "pluginlib/class_list_macros.hpp"

PLUGINLIB_EXPORT_CLASS(mobile_base_hardware::MobileBaseHardwareInterface, hardware_interface::SystemInterface)