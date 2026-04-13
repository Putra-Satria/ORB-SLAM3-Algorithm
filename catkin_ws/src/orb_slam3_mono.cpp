#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include "System.h"

class ORBSLAM3Node {
private:
    ORB_SLAM3::System* SLAM;
    image_transport::Subscriber image_sub;
    ros::Publisher pose_pub;
    
public:
    ORBSLAM3Node(ros::NodeHandle& nh) {
        // Get parameters
        std::string vocab_path, settings_path;
        nh.param<std::string>("vocab_path", vocab_path, "Vocabulary/ORBvoc.txt");
        nh.param<std::string>("settings_path", settings_path, "settings.yaml");
        
        // Initialize ORB-SLAM3
        SLAM = new ORB_SLAM3::System(vocab_path, settings_path, ORB_SLAM3::System::MONOCULAR, true);
        
        // Setup subscribers and publishers
        image_transport::ImageTransport it(nh);
        image_sub = it.subscribe("/camera/image_raw", 1, &ORBSLAM3Node::imageCallback, this);
        pose_pub = nh.advertise<geometry_msgs::PoseStamped>("/slam/pose", 10);
    }
    
    void imageCallback(const sensor_msgs::ImageConstPtr& msg) {
        try {
            cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
            
            // Process frame with ORB-SLAM3
            Sophus::SE3f pose = SLAM->TrackMonocular(cv_ptr->image, msg->header.stamp.toSec());
            
            if (!pose.matrix().isZero()) {
                // Publish pose for navigation
                geometry_msgs::PoseStamped pose_msg;
                pose_msg.header = msg->header;
                // Convert Sophus::SE3f to geometry_msgs::Pose
                // ... conversion code here
                pose_pub.publish(pose_msg);
            }
        }
        catch (cv_bridge::Exception& e) {
            ROS_ERROR("CV bridge error: %s", e.what());
        }
    }
};

int main(int argc, char** argv) {
    ros::init(argc, argv, "orb_slam3_node");
    ros::NodeHandle nh("~");
    
    ORBSLAM3Node node(nh);
    ros::spin();
    
    return 0;
}