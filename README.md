# ROS 2 Course Code Temple

这是学习 Udemy 课程 [ROS 2 For Beginners series](https://www.udemy.com/user/edouard-renard/) 时整理的 ROS 2 工作空间。仓库以代码练习和个人实验为主，覆盖 Python、C++、通信接口、launch、参数、生命周期、TF、Action、插件控制器和硬件接口等主题。

## 环境

- Ubuntu 22.04 + ROS 2 Humble
- Ubuntu 24.04 + ROS 2 Jazzy
- `colcon`、`rosdep`、`turtlesim`

同一个终端只加载一个 ROS 2 发行版。切换发行版后，应在目标发行版环境中重新构建工作空间。

## 快速开始

仓库根目录 `ros2-code-learn` 本身就是工作空间，包位于 `src/` 下。

```bash
cd ros2-code-learn

# 按实际安装的发行版选择一条
source /opt/ros/humble/setup.bash
# source /opt/ros/jazzy/setup.bash

rosdep install --from-paths src --ignore-src -r -y
colcon build --symlink-install
source install/setup.bash
```

只构建某个包，或构建它及其工作空间依赖：

```bash
colcon build --packages-select my_py_pkg
colcon build --packages-up-to my_robot_bringup
```

修改构建配置或切换发行版后，可以清理构建产物再重建：

```bash
rm -rf build install log
colcon build --symlink-install
source install/setup.bash
```

## 包与学习内容

| 包 | 内容 |
| --- | --- |
| `my_py_pkg` | Python 节点示例：Topic、Service、Action、参数、TF、生命周期、执行器和硬件状态 |
| `my_cpp_pkg` | C++ 节点示例，与 Python 包对应，包含 Topic、Service、Action、TF 和自定义接口使用 |
| `my_robot_interfaces` | 自定义接口：`HardwareStatus`、`LedStateArray`、`Turtle` 消息，`SetLed`、`CatchTurtle` 服务，以及 `CountUntil` Action |
| `my_robot_bringup` | launch 文件和 YAML 参数配置，用于组合多个节点，例如 `number_app.launch.py` |
| `turtlesim_catch_them_all` | Python 版 turtlesim 综合示例，包含乌龟生成器和控制器 |
| `turtlesim_project_cpp` | C++ 版 turtlesim 综合示例，包含 `controller` 和 `spawner` |
| `my_robot_controllers` | `controller_interface`、`pluginlib` 和生命周期控制器实验 |
| `my_robot_hardware` | `hardware_interface`、`pluginlib` 和 Dynamixel 移动底盘硬件接口实验 |

## 运行示例

完成构建并 source 工作空间后，可以运行基础 Python 节点：

```bash
ros2 run my_py_pkg py_node
```

运行 launch 示例，它会启动 Python 的 `number_publisher` 和 C++ 的 `number_counter`：

```bash
ros2 launch my_robot_bringup number_app.launch.py
```

也可以传入 launch 参数：

```bash
ros2 launch my_robot_bringup number_app.launch.py number:=10 timer_period:=0.5
```

运行 turtlesim 综合示例时，先启动模拟器：

```bash
ros2 run turtlesim turtlesim_node
```

然后在另一个已加载 ROS 2 环境的终端中运行对应节点，例如：

```bash
ros2 run turtlesim_catch_them_all spawner
ros2 run turtlesim_catch_them_all controller
```

## 常用 ROS 2 CLI

仓库中的 [`命令行整理.md`](命令行整理.md) 记录了 Node、Topic、Service、Action、Interface 和 Param 的操作。最常用的检查命令包括：

```bash
ros2 node list
ros2 topic list -t
ros2 service list
ros2 action list
ros2 interface list
ros2 param list
```

使用 turtlesim 练习时，可另开终端启动键盘控制：

```bash
ros2 run turtlesim turtle_teleop_key
```

## 目录结构

```text
ros2-code-learn/
├── README.md
├── 命令行整理.md
└── src/
    ├── my_cpp_pkg/
    ├── my_py_pkg/
    ├── my_robot_bringup/
    ├── my_robot_controllers/
    ├── my_robot_hardware/
    ├── my_robot_interfaces/
    ├── turtlesim_catch_them_all/
    └── turtlesim_project_cpp/
```

每个包的 `package.xml` 声明依赖，C++ 包使用 `CMakeLists.txt`，Python 包使用 `setup.py` 和 `setup.cfg`。自定义接口位于 `my_robot_interfaces/msg`、`srv` 和 `action` 目录，launch 与参数配置位于 `my_robot_bringup/launch` 和 `config` 目录。
