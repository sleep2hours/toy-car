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
    std::atomic_char status = 0x00;
    KeyCtrl kc(dev_name, status);
    std::thread th(&KeyCtrl::onKeyThread, &kc);
    th.detach();
    char last_val = status;
    std_msgs::Int16 msg;
    while (ros::ok())
    {
        if (status != 0x00)
        {
            last_val = status;
            std::bitset<8> x(last_val);
            for (int i = 0; i < 8; i++)
            {
                if (x.test(i) != 0) //1w2a3s4d5o6p
                {
                    switch (i)
                    {
                    case 0:
                        msg.data = 1;
                        break;
                    case 1:
                        msg.data = 2;
                        break;
                    case 2:
                        msg.data = 3;
                        break;
                    case 3:
                        msg.data = 4;
                        break;
                    case 5:
                        msg.data = 5;
                        break;
                    case 6:
                        msg.data = 6;
                        break;
                    default:
                        msg.data = 0;
                        break;
                    }
                    pub.publish(msg);
                }
            }
            // usleep(10000);
        }
        else
        {
            msg.data = 0;
            pub.publish(msg);
        }
        loop_rate.sleep();
    }
    return 0;
}