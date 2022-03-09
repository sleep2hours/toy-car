#include <ros/ros.h>
#include <std_msgs/Int16.h>
#include <serial/serial.h>
#include <dirent.h>
serial::Serial ser;
void carCtrl(std_msgs::Int16::ConstPtr msg)
{
    uint8_t buffer[1];
    buffer[0]=msg->data;
    int num=ser.write(buffer,1);
    ROS_INFO("Sent comm to serial port. %lu bits.", num);
}
int serialOK(char *output)
{
    std::cout << "Directory dev/serial exists." << std::endl;
    DIR *dir = opendir("/dev/serial/by-id");
    if (!dir)
    { //是否能正常打开目录
        std::cout << "Open directory error." << std::endl;
        return -1;
    }
    struct dirent *files;
    while ((files = readdir(dir)))
    { //查找名称长度大于5的为USB设备
        for (int i = 0; i < 256 && files->d_name[i] > 0; ++i)
        {
            if (i > 5)
            {
                strcat(output, files->d_name); // 云台板断电，重新给予权限
                std::string command = "echo \"bfjg\" | sudo -S chmod 777 " + std::string(output);
                system(command.c_str());
                return 0;
            }
        }
    }
    return -1; //所有设备名称不符合要求
}
int main(int argc, char **argv)
{
    ros::init(argc, argv, "msg_receive");
    ros::NodeHandle nh;
    ros::Subscriber sub = nh.subscribe("/input_value", 1, carCtrl);
    
    try
    {
        char path[128] = "/dev/serial/by-id/";
        if (!serialOK(path) == 0)
        {
            ROS_ERROR_STREAM("Unable to open port. Not exception. Possible null path detected.");
            exit(-1);
        }
        ser.setPort(path);
        std::cout << path << std::endl;
        serial::Timeout to = serial::Timeout::simpleTimeout(10);
        ser.setBaudrate(115200);
        //串口设置timeout
        ser.setTimeout(to);
        ser.open();
        printf("Serial port opened.\n");
    }
    catch (serial::IOException &e)
    {
        ROS_ERROR_STREAM("Unable to open port with Exception.");
        exit(-1);
    }
    if (ser.isOpen())
    {
        ROS_INFO_STREAM("Serial Port initialized.");
    }
    else
    {
        ROS_ERROR_STREAM("Open serial port failed.");
        exit(-1);
    }
    ros::spin();
    return 0;
}