#include <rclcpp/rclcpp.hpp>
#include <rclcpp/logging.hpp>
#include <std_msgs/msg/string.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/camera_info.hpp>

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
      this->declare_parameter<int>("max_bag_size", 0);
      this->declare_parameter<int>("max_bag_duration", 0);

      // load parameters
      this->get_parameter("bag_file", bag_file_);
      this->get_parameter("storage_id", storage_id_);
      this->get_parameter("topic_names", topic_names_);
      this->get_parameter("topic_types", topic_types_);
      this->get_parameter("max_bag_size", max_bag_size_);
      this->get_parameter("max_bag_duration", max_bag_duration_);

      RCLCPP_INFO(this->get_logger(), "bag_file: %s", bag_file_.c_str());
      RCLCPP_INFO(this->get_logger(), "storage_id: %s", storage_id_.c_str());

      for (int i = 0; i < topic_names_.size(); i++) {
        RCLCPP_INFO(this->get_logger(), "topic_name: %s, type: %s", topic_names_[i].c_str(), topic_types_[i].c_str());
      }

      RCLCPP_INFO(this->get_logger(), "max_bag_size: %d", max_bag_size_);
      RCLCPP_INFO(this->get_logger(), "max_bag_duration: %d", max_bag_duration_);

      writer_ = std::make_unique<rosbag2_cpp::Writer>();
      
      rosbag2_storage::StorageOptions storage_options = {bag_file_, storage_id_, max_bag_size_, max_bag_duration_};
      writer_->open(storage_options);

      for (int i = 0; i < topic_names_.size(); ++i) {
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
        }
      };
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

    std::unordered_map<std::string, rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr> pc_subscriptions_;
    std::unordered_map<std::string, rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr> rgb_subscriptions_;
    std::unique_ptr<rosbag2_cpp::Writer> writer_;

    std::string bag_file_;
    std::string storage_id_;
    std::vector<std::string> topic_names_;
    std::vector<std::string> topic_types_;
    int max_bag_size_;
    int max_bag_duration_;
  };
}

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(mrsd::IPCBagRecorder)
