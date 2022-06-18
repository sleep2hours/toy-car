#include <ros/ros.h>
#include <image_transport/image_transport.h> //接收图像流需要
#include <cv_bridge/cv_bridge.h>             //与图像转换有关的函数和常量
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sensor_msgs/CompressedImage.h>

void imageCv(sensor_msgs::CompressedImage::ConstPtr msg)
{

    // if (cv_ptr->image.rows > 60 && cv_ptr->image.cols > 60)
    //     cv::circle(cv_ptr->image, cv::Point(50, 50), 5, cv::Scalar(0, 0, 255), -1);
    cv::Mat image = cv::imdecode(cv::Mat(msg->data), 1); //convert compressed image data to cv::Mat
    cv::add(image, image,image);
    cv::imshow("show", image);

    // cv::imshow("show", cv_ptr->image);
    cv::waitKey(1);
}
int main(int argc, char **argv)
{

    ros::init(argc, argv, "image_converter");
    ros::NodeHandle nh;
    // image_transport::ImageTransport it(nh);
    // image_transport::Subscriber img_sub = it.subscribe("/camera/cv", 1, imageCv);
    ros::Subscriber sub = nh.subscribe("/camera/compressed", 1, imageCv);
    ros::spin();
    return 0;
}
