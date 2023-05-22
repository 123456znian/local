#ifndef FOLLOW_BOT_SRC_FOLLOWER_H_
#define FOLLOW_BOT_SRC_FOLLOWER_H_

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <follow_bot/CamToReal.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <geometry_msgs/Twist.h>


class Follower {
private:
	ros::NodeHandle nh;
	image_transport::ImageTransport imageTransport;
	image_transport::Subscriber imageSubscriber;
	ros::Publisher cmdVelPublisher;
	ros::ServiceClient client;
	follow_bot::CamToReal srv;
	double distance;
	geometry_msgs::Twist cmd;

	void imageCallback(const sensor_msgs::ImageConstPtr& msg);

public:
	Follower();
	virtual ~Follower();
};

#endif /* FOLLOW_BOT_SRC_FOLLOWER_H_ */
