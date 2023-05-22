#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <ros/ros.h>
#include <cv_bridge/cv_bridge.h>
#include <geometry_msgs/PointStamped.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/CameraInfo.h>
#include <sensor_msgs/Image.h>
#include "follow_bot/CamToReal.h"
#include <algorithm>
#include <stdlib.h>

using namespace cv;
using namespace std;

class ImageConverter
{
private:
	ros::NodeHandle nh_;
	image_transport::ImageTransport it_;
	image_transport::Subscriber image_sub_depth; //接收深度图像
	image_transport::Subscriber image_sub_color; //接收彩色图像

	ros::Subscriber camera_info_sub_; //接收深度图像对应的相机参数话题

	ros::ServiceServer CamtoReal;
	float obj_x, obj_y;
	sensor_msgs::CameraInfo camera_info;

	/* Mat depthImage,colorImage; */
	Mat colorImage;
	Mat depthImage = Mat::zeros(480, 640, CV_32FC1); // camera_info

public:
	ImageConverter() : it_(nh_)
	{
		image_sub_depth = it_.subscribe( "/camera/depth/image_raw",
						 1, &ImageConverter::imageDepthCb, this );
		image_sub_color = it_.subscribe( "/camera/rgb/image_raw", 1,
						 &ImageConverter::imageColorCb, this );
		camera_info_sub_ =
			nh_.subscribe( "/camera/depth/camera_info", 1,
				       &ImageConverter::cameraInfoCb, this );
		CamtoReal = nh_.advertiseService("/cam_to_real", &ImageConverter::CamtoRealCallback, this);
	}

	~ImageConverter()
	{
	}

	void cameraInfoCb(const sensor_msgs::CameraInfo &msg)
	{
		camera_info = msg;
	}

	void imageDepthCb(const sensor_msgs::ImageConstPtr &msg)
	{
		cv_bridge::CvImagePtr cv_ptr;

		try
		{
			cv_ptr =
				cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::TYPE_32FC1);
			depthImage = cv_ptr->image;
		}
		catch (cv_bridge::Exception &e)
		{
			ROS_ERROR("cv_bridge exception: %s", e.what());
			return;
		}
	}

	void imageColorCb(const sensor_msgs::ImageConstPtr &msg)
	{
		cv_bridge::CvImagePtr cv_ptr;
		try
		{
			cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
			colorImage = cv_ptr->image;
		}
		catch (cv_bridge::Exception &e)
		{
			ROS_ERROR("cv_bridge exception: %s", e.what());
			return;
		}

	}

	bool CamtoRealCallback(follow_bot::CamToReal::Request &req,
						   follow_bot::CamToReal::Response &res)
	{
		obj_x = req.pixel_x;
		obj_y = req.pixel_y;
		float real_z = depthImage.at<float>(obj_y, obj_x);
		float	real_x	=
			(obj_x - camera_info.K.at( 2 ) ) / camera_info.K.at( 0 ) * real_z;
		float real_y =
			(obj_y - camera_info.K.at( 5 ) ) / camera_info.K.at( 4 ) * real_z;
		if (real_z != 0)
		{
			res.obj_x = real_x;
			res.obj_y = real_y;
			res.obj_z = real_z;
			res.result = true;
			return true;
		}
		else
		{
			res.obj_x = 0;
			res.obj_y = 0;
			res.obj_z = 0;
			res.result = false;
			return false;
		}
	}
};

int main(int argc, char **argv)
{
	ros::init(argc, argv, "detect_obj");
	ImageConverter imageconverter;
	ros::AsyncSpinner spinner(1);
	spinner.start();
	ros::waitForShutdown();
}
