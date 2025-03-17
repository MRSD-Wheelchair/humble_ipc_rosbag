#include <rclcpp/rclcpp.hpp>
#include <rclcpp/logging.hpp>
#include "rclcpp/serialized_message.hpp"
#include <std_msgs/msg/string.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/camera_info.hpp>
#include <nav_msgs/msg/odometry.hpp>

#include <rosbag2_cpp/writer.hpp>
#include <rosbag2_storage/storage_options.hpp>

#include <rcl_interfaces/msg/parameter_descriptor.hpp>
#include <unordered_map>
#include <chrono>

using namespace std::chrono_literals;
using namespace std::placeholders;

using std::placeholders::_1;

namespace mrsd {
  class IPCBagRecorder : public rclcpp::Node
  {
  public:
    IPCBagRecorder(const rclcpp::NodeOptions & options)
    : Node("ipc_bag_recorder", options)
    {
      // read parameters
      this->declare_parameter<std::string>("bag_file", "ipc_bag");
      this->declare_parameter<std::string>("storage_id", "");
      this->declare_parameter<std::vector<std::string>>("topic_names", std::vector<std::string>());
      this->declare_parameter<std::vector<std::string>>("topic_types", std::vector<std::string>());
      this->declare_parameter<int64_t>("max_bag_size", 0);
      this->declare_parameter<int64_t>("max_bag_duration", 0);

      // load parameters
      this->get_parameter("bag_file", bag_file_);
      this->get_parameter("storage_id", storage_id_);
      this->get_parameter("topic_names", topic_names_);
      this->get_parameter("topic_types", topic_types_);
      this->get_parameter("max_bag_size", max_bag_size_);
      this->get_parameter("max_bag_duration", max_bag_duration_);

      RCLCPP_INFO(this->get_logger(), "bag_file: %s", bag_file_.c_str());
      RCLCPP_INFO(this->get_logger(), "storage_id: %s", storage_id_.c_str());

      for (size_t i = 0; i < topic_names_.size(); i++) {
        RCLCPP_INFO(this->get_logger(), "topic_name: %s, type: %s", topic_names_[i].c_str(), topic_types_[i].c_str());
      }

      RCLCPP_INFO(this->get_logger(), "max_bag_size: %ld", max_bag_size_);
      RCLCPP_INFO(this->get_logger(), "max_bag_duration: %ld", max_bag_duration_);

      writer_ = std::make_unique<rosbag2_cpp::Writer>();
      
      rosbag2_storage::StorageOptions storage_options = {bag_file_, storage_id_, (uint64_t)max_bag_size_, (uint64_t)max_bag_duration_};
      writer_->open(storage_options);

      for (size_t i = 0; i < topic_names_.size(); ++i) {
        std::string topic_name = topic_names_[i];
        std::string topic_type = topic_types_[i];

        if (topic_type == "sensor_msgs/msg/PointCloud2") {
          std::function<void(const sensor_msgs::msg::PointCloud2::SharedPtr msg)> callback =
            std::bind(&IPCBagRecorder::pc_callback, this, _1, topic_name);

          pc_subscriptions_[topic_name] = create_subscription<sensor_msgs::msg::PointCloud2>(
            topic_name, 10, callback);
        } else if (topic_type == "sensor_msgs/msg/Image") {
          std::function<void(const sensor_msgs::msg::Image::SharedPtr msg)> callback =
            std::bind(&IPCBagRecorder::rgb_callback, this, _1, topic_name);

          rgb_subscriptions_[topic_name] = create_subscription<sensor_msgs::msg::Image>(
            topic_name, 10, callback);
        } else if (topic_type == "sensor_msgs/msg/CameraInfo") {
          std::function<void(const sensor_msgs::msg::CameraInfo::SharedPtr msg)> callback =
            std::bind(&IPCBagRecorder::camera_info_callback, this, _1, topic_name);

          camera_info_subscriptions_[topic_name] = create_subscription<sensor_msgs::msg::CameraInfo>(
            topic_name, 10, callback);
        } else if (topic_type == "nav_msgs/msg/Odometry") {
          std::function<void(const nav_msgs::msg::Odometry::SharedPtr msg)> callback =
            std::bind(&IPCBagRecorder::odometry_callback, this, _1, topic_name);

          odometry_subscriptions_[topic_name] = create_subscription<nav_msgs::msg::Odometry>(
            topic_name, 10, callback);
        } else {
          RCLCPP_ERROR(this->get_logger(), "Unsupported topic type: %s", topic_type.c_str());
        }
      };
    }

  private:
    void pc_callback(const sensor_msgs::msg::PointCloud2::SharedPtr msg, const std::string& topic_name) const
    {
      auto serialized_msg = std::make_shared<rclcpp::SerializedMessage>();
      rclcpp::Serialization< nav_msgs::msg::Odometry > serialization;
      serialization.serialize_message(msg.get(), serialized_msg.get());
      rclcpp::Time time_stamp = msg->header.stamp;
      writer_->write(serialized_msg, topic_name, "sensor_msgs/msg/PointCloud2", time_stamp);
    }

    void rgb_callback(const sensor_msgs::msg::Image::SharedPtr msg, const std::string& topic_name) const
    {
      auto serialized_msg = std::make_shared<rclcpp::SerializedMessage>();
      rclcpp::Serialization< nav_msgs::msg::Odometry > serialization;
      serialization.serialize_message(msg.get(), serialized_msg.get());
      rclcpp::Time time_stamp = msg->header.stamp;
      writer_->write(serialized_msg, topic_name, "sensor_msgs/msg/Image", time_stamp);
    }

    void camera_info_callback(const sensor_msgs::msg::CameraInfo::SharedPtr msg, const std::string& topic_name) const
    {
      auto serialized_msg = std::make_shared<rclcpp::SerializedMessage>();
      rclcpp::Serialization< nav_msgs::msg::Odometry > serialization;
      serialization.serialize_message(msg.get(), serialized_msg.get());
      rclcpp::Time time_stamp = msg->header.stamp;
      writer_->write(serialized_msg, topic_name, "sensor_msgs/msg/CameraInfo", time_stamp);
    }

    void odometry_callback(const nav_msgs::msg::Odometry::SharedPtr msg, const std::string& topic_name) const
    {
      auto serialized_msg = std::make_shared<rclcpp::SerializedMessage>();
      rclcpp::Serialization< nav_msgs::msg::Odometry > serialization;
      serialization.serialize_message(msg.get(), serialized_msg.get());
      rclcpp::Time time_stamp = msg->header.stamp;
      writer_->write(serialized_msg, topic_name, "nav_msgs/msg/Odometry", time_stamp);
    }

    std::unordered_map<std::string, rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr> pc_subscriptions_;
    std::unordered_map<std::string, rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr> rgb_subscriptions_;
    std::unordered_map<std::string, rclcpp::Subscription<sensor_msgs::msg::CameraInfo>::SharedPtr> camera_info_subscriptions_;
    std::unordered_map<std::string, rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr> odometry_subscriptions_;
    std::unique_ptr<rosbag2_cpp::Writer> writer_;

    std::string bag_file_;
    std::string storage_id_;
    std::vector<std::string> topic_names_;
    std::vector<std::string> topic_types_;
    int64_t max_bag_size_;
    int64_t max_bag_duration_;
  };
}

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(mrsd::IPCBagRecorder)
