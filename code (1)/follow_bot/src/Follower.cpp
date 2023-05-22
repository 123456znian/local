#include "Follower.h"

static const std::string OPENCV_WINDOW = "Image window";

Follower::Follower() : imageTransport(nh)
{
	imageSubscriber = imageTransport.subscribe("/camera/rgb/image_raw", 1,
											   &Follower::imageCallback, this);
	cmdVelPublisher = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 10);

	client = nh.serviceClient<follow_bot::CamToReal>("cam_to_real");

	// 创建显示窗口
	cv::namedWindow(OPENCV_WINDOW);
}

Follower::~Follower()
{
	// 关闭显示窗口
	cv::destroyWindow(OPENCV_WINDOW);
}

void Follower::imageCallback(const sensor_msgs::ImageConstPtr &msg)
{
	// 将 ROS 图像消息转换为 CvImage
	cv_bridge::CvImagePtr cv_ptr;
	try
	{
		cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
	}
	catch (cv_bridge::Exception &e)
	{
		ROS_ERROR("cv_bridge exception: %s", e.what());
		return;
	}

	// 将输入图像转换为 HSV
	cv::Mat image = cv_ptr->image;
	cv::Mat hsvImage;
cv:
	cvtColor(image, hsvImage, CV_BGR2HSV);

	// 对 HSV 图像进行阈值处理，只保留黄色像素
	cv::Scalar lower_yellow(26, 43, 46);
	cv::Scalar upper_yellow(34, 255, 255);

	cv::Mat mask;
	cv::inRange(hsvImage, lower_yellow, upper_yellow, mask);

	int width = mask.cols;
	int height = mask.rows;

	// 使用 OpenCV moments() 函数计算二值图像的 blob 的质心
	cv::Moments M = cv::moments(mask);

	if (M.m00 > 0)
	{
		int cx = int(M.m10 / M.m00);
		int cy = int(M.m01 / M.m00);

		srv.request.pixel_x = cx;
		srv.request.pixel_y = cy;
		client.call(srv);

		distance = srv.response.obj_z;

		cv::circle(image, cv::Point(cx, cy), 2, CV_RGB(255, 0, 0), -1);

		// 根据error通过pid控制器移动机器人
		int err = cx - width / 2;
		cmd.linear.x = (distance - 1.0) / 2;
		cmd.angular.z = -(float)err / 100;
	}
	else
	{
		cmd.linear.x = 0.0;
		cmd.angular.z = 0.0;
	}
	cmdVelPublisher.publish(cmd);

	// 更新 GUI 窗口
	cv::imshow(OPENCV_WINDOW, image);
	cv::waitKey(3);
}
