#include "Follower.h"

int main(int argc, char **argv) {
	ros::init(argc, argv, "follower");
	Follower follower;
	ros::spin();
	return 0;
}


