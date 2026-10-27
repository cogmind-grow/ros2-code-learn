#ifndef MOBILE_BASE_HARDWARE_INTERFACE_HPP
#define MOBILE_BASE_HARDWARE_INTERFACE_HPP

#include <memory>
#include <string>

#include "hardware_interface/system_interface.hpp"
#include "hardware_interface/types/hardware_interface_type_values.hpp"
#include "my_robot_hardware/xl330_driver.hpp"

namespace mobile_base_hardware {

class MobileBaseHardwareInterface : public hardware_interface::SystemInterface {
public:
    // Lifecycle node override
    hardware_interface::CallbackReturn
    on_configure(const rclcpp_lifecycle::State & previous_state) override;
    hardware_interface::CallbackReturn
    on_activate(const rclcpp_lifecycle::State & previous_state) override;
    hardware_interface::CallbackReturn
    on_deactivate(const rclcpp_lifecycle::State & previous_state) override;

    // SystemInterface override
    hardware_interface::CallbackReturn
    on_init(const hardware_interface::HardwareInfo & info) override;
    std::vector<hardware_interface::StateInterface> export_state_interfaces() override;
    std::vector<hardware_interface::CommandInterface> export_command_interfaces() override;
    hardware_interface::return_type
    read(const rclcpp::Time & time, const rclcpp::Duration & period) override;
    hardware_interface::return_type
    write(const rclcpp::Time & time, const rclcpp::Duration & period) override;

private:
    std::shared_ptr<XL330Driver> driver_;
    int left_motor_id_;
    int right_motor_id_;
    std::string port_;
    
    double left_wheel_position_;
    double left_wheel_velocity_;
    double right_wheel_position_;
    double right_wheel_velocity_;
    double left_wheel_command_;
    double right_wheel_command_;
    
    void set_state(const std::string & name, double value);
    double get_state(const std::string & name) const;
    double get_command(const std::string & name) const;
}; // class MobileBaseHardwareInterface

} // namespace mobile_base_hardware

#endif