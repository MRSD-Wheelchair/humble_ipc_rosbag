import os

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument

from launch.substitutions import LaunchConfiguration

from launch_ros.actions import LoadComposableNodes
from launch_ros.descriptions import ComposableNode

from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    # get the path to the default parameters file
    default_params_file = os.path.join(
        get_package_share_directory("bag_recorder_nodes"), "config", "params.yaml"
    )

    declare_params_file_arg = DeclareLaunchArgument(
        "params_file", default_value=default_params_file
    )
    declare_container_name_arg = DeclareLaunchArgument(
        "container_name", default_value="/zedx/zed_container"
    )

    params_file = LaunchConfiguration("params_file")
    container_name = LaunchConfiguration("container_name")

    bag_recorder = ComposableNode(
        package="bag_recorder_nodes",
        name="bag_recorder",
        plugin="mrsd::IPCBagRecorder",
        parameters=[params_file],
        namespace="mrsd",
        extra_arguments=[{"use_intra_process_comms": True}],
    )

    load_recorder_node = LoadComposableNodes(
        composable_node_descriptions=[bag_recorder],
        target_container=container_name,
    )

    return LaunchDescription(
        [
            declare_params_file_arg,
            declare_container_name_arg,
            load_recorder_node,
        ]
    )
