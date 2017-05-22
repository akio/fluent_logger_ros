#include <chrono>
#include <ros/ros.h>
#include <rosgraph_msgs/Log.h>
#include <fluent.hpp>


class FluentLogger
{
public:
    FluentLogger() : logger_(new fluent::Logger)
    {
        ros::NodeHandle private_nh("~");

        int queue_size;
        private_nh.param("queue_size", queue_size, 100);

        std::string host;
        private_nh.param("host", host, std::string("localhost"));

        int port;
        private_nh.param("port", port, 24224);

        std::string rosout_topic;
        private_nh.param("rosout_topic", rosout_topic,
                         std::string("/rosout_agg"));

        private_nh.param("log_name", log_name_, std::string("ros.rosout"));

        logger_->new_forward(host, port);

        ros::NodeHandle nh;
        rosout_sub_ = nh.subscribe<rosgraph_msgs::Log>(rosout_topic,
                                                       queue_size,
                                                       &FluentLogger::callback,
                                                       this);
    }

    FluentLogger(const FluentLogger&) = delete;
    void operator=(const FluentLogger&) = delete;

    void callback(const rosgraph_msgs::Log::ConstPtr& msg)
    {
        fluent::Message* log = logger_->retain_message(log_name_);

        std::chrono::nanoseconds nsec(msg->header.stamp.toNSec());
        std::chrono::time_point<std::chrono::system_clock> timestamp(nsec);

        log->set_ts(std::chrono::system_clock::to_time_t(timestamp));

        switch (msg->level)
        {
        case rosgraph_msgs::Log::DEBUG:
            log->set("level", "DEBUG");
            break;
        case rosgraph_msgs::Log::INFO:
            log->set("level", "INFO");
            break;
        case rosgraph_msgs::Log::WARN:
            log->set("level", "WARN");
            break;
        case rosgraph_msgs::Log::ERROR:
            log->set("level", "ERROR");
            break;
        case rosgraph_msgs::Log::FATAL:
            log->set("level", "FATAL");
            break;
        default:
            log->set("level", "UNKNOWN");
            break;
        }

        log->set("name", msg->name);
        log->set("msg", msg->msg);
        log->set("file", msg->file);
        log->set("function", msg->function);
        log->set("line", msg->line);

        fluent::Message::Array* array = log->retain_array("topics");
        for (auto topic : msg->topics)
        {
            array->push(topic);
        }

        logger_->emit(log);
    }

private:
    std::unique_ptr<fluent::Logger> logger_;
    ros::Subscriber rosout_sub_;
    std::string log_name_;
};


int main(int argc, char** argv)
{
    ros::init(argc, argv, "fluent_logger", ros::init_options::NoRosout);

    FluentLogger logger;
    ros::spin();
    return 0;
}
