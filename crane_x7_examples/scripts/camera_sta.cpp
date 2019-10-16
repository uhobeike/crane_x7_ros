#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

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
    image_sub_ = it_.subscribe("/camera1/image_raw", 1,
      &ImageConverter::imageCb, this);
    // 処理した画像をパブリッシュ                                                                                          
    image_pub_ = it_.advertise("/image_topic", 1);
 }

  // デストラクタ
  ~ImageConverter()
  {
    cv::destroyWindow(OPENCV_WINDOW);
  }

  // コールバック関数
  void imageCb(const sensor_msgs::ImageConstPtr& msg)
  {
    cv_bridge::CvImagePtr cv_ptr, cv_ptr2, cv_ptr3;
 try
    {
      // ROSからOpenCVの形式にtoCvCopy()で変換。cv_ptr->imageがcv::Matフォーマット。
      cv_ptr    = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
      cv_ptr3   = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::MONO8);
    }
    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
    }

    cv::Mat hsv_image, color_mask, gray_image, cv_image2, cv_image3;
    cv::cvtColor(cv_ptr->image, gray_image, CV_BGR2GRAY);
    cv::threshold(gray_image,bin_img,80,255,CV_THRESH_BINARY);
    cv::imshow("Edge Image", bin_img);
    cv::waitKey(3);
 
    // エッジ画像をパブリッシュ。OpenCVからROS形式にtoImageMsg()で変換。                                                            
    image_pub_.publish(cv_ptr3->toImageMsg());
  }
};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "image_converter");
  ImageConverter ic;
  ros::spin();
  return 0;
}
