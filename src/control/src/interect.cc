#include <ros/ros.h>
#include <thread>
#include <bitset>
#include "keyCtrl.hpp"
#include <std_msgs/Int16.h>
const std::string dev_name = "/dev/input/by-id/usb-USB_Keyboard_USB_Keyboard_C87ANOLightA-event-kbd";

int main(int argc, char **argv)
{
    ros::init(argc, argv, "keyboard");
    ros::NodeHandle nh;
    ros::Publisher pub = nh.advertise<std_msgs::Int16>("/input_value", 5);
    ros::Rate loop_rate(100);
    int status = 0x00;
    KeyCtrl kc(dev_name, status);
    std::thread th(&KeyCtrl::onKeyThread, &kc);
    th.detach();
    int last_val = status;
    std_msgs::Int16 msg;
    while (ros::ok())
    {
        if (status != 0)
        {
            last_val = status;

            msg.data=status;
            pub.publish(msg);
        }
        // usleep(10000);
        else
        {
            msg.data = 0;
            pub.publish(msg);
        }
        loop_rate.sleep();
    }

    return 0;
}