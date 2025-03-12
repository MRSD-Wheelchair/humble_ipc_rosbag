#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/camera_info.hpp>

#include <rosbag2_cpp/writer.hpp>

#include <rcl_interfaces/msg/parameter_descriptor.hpp>
#include <unordered_map>
#include <chrono>

using namespace std::chrono_literals;
using namespace std::placeholders;

using std::placeholders::_1;

class SimpleBagRecorder : public rclcpp::Node
{
public:
  SimpleBagRecorder(const rclcpp::NodeOptions & options)
  : Node("simple_bag_recorder", options)
  {
    writer_ = std::make_unique<rosbag2_cpp::Writer>();

    writer_->open("my_bag");

    std::string topic_name = "/zed_multi/zed_node/point_cloud/cloud_registered";

    std::function<void(const sensor_msgs::msg::PointCloud2::SharedPtr msg)> callback =
      std::bind(&SimpleBagRecorder::pc_callback, this, _1, topic_name);

    subscription_ = create_subscription<sensor_msgs::msg::PointCloud2>(
      topic_name, 10, callback);

    std::string left_rgb_topic_name = "/zed_multi/zed_node/left/image_rect_color";
    std::function<void(const sensor_msgs::msg::Image::SharedPtr msg)> rgb_callback =
      std::bind(&SimpleBagRecorder::rgb_callback, this, _1, left_rgb_topic_name);

    rgb_subscription_ = create_subscription<sensor_msgs::msg::Image>(
      left_rgb_topic_name, 10, rgb_callback);
  }

private:
  void pc_callback(const sensor_msgs::msg::PointCloud2::SharedPtr msg, const std::string& topic_name) const
  {
    rclcpp::SerializedMessage serialized_msg;
    rclcpp::Serialization<sensor_msgs::msg::PointCloud2> serializer;
  
    serializer.serialize_message(msg.get(), &serialized_msg);
  
    rclcpp::Time time_stamp = msg->header.stamp;
    writer_->write(serialized_msg, topic_name, "sensor_msgs/msg/PointCloud2", time_stamp);
  }

  void rgb_callback(const sensor_msgs::msg::Image::SharedPtr msg, const std::string& topic_name) const
  {
    rclcpp::SerializedMessage serialized_msg;
    rclcpp::Serialization<sensor_msgs::msg::Image> serializer;
  
    serializer.serialize_message(msg.get(), &serialized_msg);
  
    rclcpp::Time time_stamp = msg->header.stamp;
    writer_->write(serialized_msg, topic_name, "sensor_msgs/msg/Image", time_stamp);
  }

  rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr subscription_;
  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr rgb_subscription_;
  std::unique_ptr<rosbag2_cpp::Writer> writer_;
};

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(SimpleBagRecorder)
