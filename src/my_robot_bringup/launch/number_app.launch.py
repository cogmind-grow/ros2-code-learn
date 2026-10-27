from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue
from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():
    ld = LaunchDescription()

    # param_config = os.path.join(get_package_share_directory("my_robot_bringup"),
    #                              "config", "number_app.yaml")

    number_publisher = Node(
        package="my_py_pkg",
        executable="number_publisher",
        namespace="/abc",
        name="my_number_publisher",
        remappings=[("number", "my_number")],
        # parameters=[
        #     {"number": 12},
        #     {"timer_period": 1.3}
        # ]
        # parameters=[param_config]
        parameters=[{
            "number": ParameterValue(LaunchConfiguration("number"), value_type=int),
            "timer_period": ParameterValue(LaunchConfiguration("timer_period"), value_type=float),
        }]
    )

    number_counter = Node(
        package="my_cpp_pkg",
        executable="number_counter",
        namespace="/abc",
        name="my_number_counter",
        remappings=[("number", "my_number")]
    )


    ld.add_action(DeclareLaunchArgument("number", default_value="6"))
    ld.add_action(DeclareLaunchArgument("timer_period", default_value="1.5"))

    ld.add_action(number_publisher)
    ld.add_action(number_counter)

    return ld