#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <image_transport/image_transport.h>

static const std::string OPENCV_WINDOW = "Image window";

cv::Mat bin_img;

class ImageConverter
{
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_;
  image_transport::Subscriber image_sub_;
  image_transport::Publisher image_pub_;

public:
  // コンストラクタ
    ImageConverter()
    : it_(nh_)
  {
    // カラー画像をサブスクライブ                                                                
    image_sub_ = it_.subscribe("/camera1/depth/image_raw", 1,&ImageConverter::imageCb, this);

 }

  // デストラクタ
  ~ImageConverter()
  {
    cv::destroyWindow(OPENCV_WINDOW);
  }

  // コールバック関数
  void imageCb(const sensor_msgs::ImageConstPtr& msg)
  {
    cv_bridge::CvImagePtr cv_ptr;
    try
    {
      // ROSからOpenCVの形式にtoCvCopy()で変換。cv_ptr->imageがcv::Matフォーマット。
      cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::TYPE_32FC1);
   
    }
    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
    }
  
    cv::imshow("Edge Image", cv_ptr->image);
    cv::waitKey(3);
  }
};
int main(int argc, char **argv){
    ros::init(argc, argv, "depth_estimater");
 
    ImageConverter ic;
 
    ros::spin();
    return 0;
}
