#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
 
using namespace::cv;
 
class depth_estimater{
public:
    depth_estimater();
    ~depth_estimater();
    void rgbImageCallback(const sensor_msgs::ImageConstPtr& msg);
    void depthImageCallback(const sensor_msgs::ImageConstPtr& msg);
 
private:
    ros::NodeHandle nh;
    ros::Subscriber sub_rgb, sub_depth;
};
cv::Mat img_1;
depth_estimater::depth_estimater(){
    sub_rgb = nh.subscribe<sensor_msgs::Image>("/camera/color/image_raw", 1, &depth_estimater::rgbImageCallback, this);
}
 
depth_estimater::~depth_estimater(){
}
 
void depth_estimater::rgbImageCallback(const sensor_msgs::ImageConstPtr& msg){
 
    cv_bridge::CvImagePtr cv_ptr;
 
    try{
        cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    }catch (cv_bridge::Exception& ex){
        ROS_ERROR("error");
        exit(-1);
    }    
    cv::Mat hsv_img;
    cvtColor( cv_ptr->image,hsv_img,CV_BGR2HSV,3);
    Scalar lower = cv::Scalar(160,50,50);
    Scalar upper = cv::Scalar(180,255,255);

	// BGRからHSVへ変換
	Mat mask_image, output_image;
	// inRangeを用いてフィルタリング
	inRange(hsv_img, lower, upper, mask_image);

	// マスクを基に入力画像をフィルタリング
	cv_ptr->image.copyTo(output_image, mask_image);
	
    cv::imshow("RGB image", output_image);
    cv::waitKey(10);
}
 
 
int main(int argc, char **argv){
    ros::init(argc, argv, "depth_estimater");
 
    depth_estimater depth_estimater;
 
    ros::spin();
    return 0;
}
