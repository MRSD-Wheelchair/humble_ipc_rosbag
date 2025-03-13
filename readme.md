# humble_ipc_rosbag

The `humble_ipc_rosbag` package provides tools for recording and managing ROS2 bag files with intra-process communication (IPC) support. This package is designed to work with ROS2 Humble and leverages the `rosbag2` framework for efficient data recording and playback.

## Features

- Record ROS2 topics with IPC support to minimize latency and overhead.
- Configurable parameters for bag file storage, topic selection, and recording options.
- Launch files for easy setup and execution of recording nodes.
- Currently support `Image` and `PointCloud2` messages, will add more in the future.

## Requirements

- ROS2 Humble
- `rosbag2` package
- `rclcpp` and `rclcpp_components` packages

## Installation

1. Clone the repository into your ROS2 workspace:
    ```bash
    mkdir -p ~/ros2_ws/src
    cd ~/ros2_ws/src
    git clone https://github.com/your-repo/humble_ipc_rosbag.git
    ```

2. Install dependencies:
    ```bash
    cd ~/ros2_ws
    rosdep install --from-paths src --ignore-src -r -y
    ```

3. Build the package:
    ```bash
    colcon build --symlink-install
    ```

4. Source the workspace:
    ```bash
    source ~/ros2_ws/install/setup.bash
    ```

## Usage

### Launching the Recorder Node

Thre are two launch files, `simple.launch.py` and `load.launch.py`.

`simple.launch.py` will launch multiple zed camera and the Recorder node simultaneously. To use it, you have to first take a look at the information of your zed cameras by using `ZED_Explerer --all`. Then you can use it by
```bash
ros2 launch bag_recorder_nodes simple.launch.py cam_names:=[<the name you want to give the camera>] cam_models:=[<your camera model>] cam_serials:=[<your camera serial>]

# for example
ros2 launch bag_recorder_nodes simple.launch.py cam_names:=[zed_front] cam_models:=[zedx] cam_serials:=[35199186]
```

`load.launch.py` is a more recommended launch file to use. You can first run up your zed camera container, and launch the recorder whenever you want.

```bash
# first bringup your zed container, open a new terminal, and source ros environment
ros2 component list # to see the container name
ros2 launch bag_recorder_nodes load.launch.py container_name:=<zed_contianer_name>

# when you want to terminate your recording
ros2 component list # to see the id of the composable node
ros2 component unload <zed_container_name> <compoable_node_id>
```

### Configuring Parameters

You can customize the recording parameters by editing the params.yaml file located in the `config` directory. The following parameters can be configured:

- `bag_file`: Name of the bag file to be created.
- `storage_id`: Storage format for the bag file (e.g., `sqlite3`, `mcap`).
- `topic_names`: List of topics to be recorded.
- `topic_types`: Corresponding types of the topics to be recorded.
- `max_bag_size`: Maximum size of the bag file in bytes.
- `max_bag_duration`: Maximum duration of the bag file in seconds.

### Example Configuration

Here is an example configuration in params.yaml:

```yaml
/**:
  ros__parameters:
    bag_file: "test_bag"
    storage_id: "mcap"
    topic_names:
      - "/zed/zed_node/left/image_rect_color"
      - "/zed/zed_node/rgb/image_rect_color"
      - "/zed/zed_node/point_cloud/cloud_registered"
    topic_types:
      - "Image"
      - "Image"
      - "PointCloud2"
    max_bag_size: 1000000000
    max_bag_duration: 0
```

### Launching with Custom Parameters

To launch the recorder node with custom parameters, use the load.launch.py file and specify the path to your custom params.yaml file:

```bash
ros2 launch bag_recorder_nodes load.launch.py params_file:=/path/to/your/params.yaml
```

## License

This project is licensed under the Apache License, Version 2.0. See the LICENSE file for details.

## Contributing

Contributions are welcome! Please open an issue or submit a pull request on GitHub.

## Maintainers

- [Your Name](mailto:your.email@example.com)
```

This README provides an overview of the `humble_ipc_rosbag` package, including installation instructions, usage examples, and configuration options. Adjust the repository URL, maintainer information, and other details as needed.
This README provides an overview of the `humble_ipc_rosbag` package, including installation instructions, usage examples, and configuration options. Adjust the repository URL, maintainer information, and other details as needed.