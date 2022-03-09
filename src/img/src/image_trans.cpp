#include <ros/ros.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <image_transport/image_transport.h> //接收图像流需要
#include <cv_bridge/cv_bridge.h>             //与图像转换有关的函数和常量
#include <sensor_msgs/image_encodings.h>
#include <sensor_msgs/CompressedImage.h>
#include <cstdio>
#include "aim_cam/HPG.hpp"

int main(int argc, char **argv)
{
    cv::setNumThreads(2);
    hpg::HPGrabbing grab;
    ros::init(argc, argv, "img_puber");
    grab.start();
    grab.setCameraParams(0);
    ros::NodeHandle nh;
    // image_transport::ImageTransport it(nh);
    // image_transport::Publisher pub = it.advertise("/camera/cv", 1);
    ros::Publisher pub = nh.advertise<sensor_msgs::CompressedImage>("/camera/compressed", 1);
    sensor_msgs::CompressedImagePtr msg;
    cv::VideoCapture cap;
    cv_bridge::CvImage img_ptr;
    cv::Mat frame;
    double t_deal = 0.0;
    int cnt = 0;
    do
    {
        cnt++;
        t_deal = std::chrono::system_clock::now().time_since_epoch().count() / 1e9;
        if (grab.getMat(frame))
        {
            continue;
        }
        // cv::imshow("origin", frame);
        // if (cv::waitKey(1) == 'q')
        // {
        //     break;
        // }
        msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame).toCompressedImageMsg(cv_bridge::JPG);
        pub.publish(msg);
        grab.setBufferState(); // unlock HPG
        // double t_end = std::chrono::system_clock::now().time_since_epoch().count() / 1e9;
        // std::cout << (t_end - t_deal) << std::endl;
    } while (ros::ok());
    return 0;
}