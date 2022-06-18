#include <unordered_map>
#include "keyCtrl.hpp"
#include <iostream>
const std::unordered_map<int, int> mapping = {
    {458778, 1}, {458756, 2}, {458774, 3}, {458759, 4}, {458770, 5}, {458771, 6}, {458763, 8}, {458762, 7}, {458761, 9}, {458775, 10}}; // w a s d r o p F H G T

KeyCtrl::KeyCtrl(std::string dev_name, int &stat) : status(stat)
{
    constexpr char name[13] = "K_MEDIUMRAW";
    const char *device = NULL;
    int a = KEY_A;
    if ((getuid()) != 0)
    { // admin clearance
        std::string command = "echo \"123\" | sudo -S chmod 777 " + dev_name;
        system(command.c_str());
    }
    device = dev_name.c_str();

    if ((fd = open(device, O_RDONLY)) == -1)
        printf("%s is not a vaild device.\n", device);

    ioctl(fd, EVIOCGNAME(sizeof(name)), name);
    printf("Reading From : %s (%s)\n", device, name);
    struct input_event s;
}

KeyCtrl::~KeyCtrl()
{
    close(fd);
}

void KeyCtrl::onKeyThread()
{
    struct input_event ev[64];
    int rd = 0, value = 0, size = sizeof(input_event);
    while (true)
    {
        if ((rd = read(fd, ev, size * 64)) < size)
            perror_exit("read()");
        value = ev[0].value;
        if (value != ' ' && ev[1].value == 1)
        {
            // key down
            std::unordered_map<int, int>::const_iterator cit = mapping.find(value);
            if (cit == mapping.cend())
                continue;
            int id = cit->second;
            char digit = (0x001 << id);
    
            status =id;
        }
        else
        {
            std::unordered_map<int, int>::const_iterator cit = mapping.find(value);
            if (cit == mapping.cend())
                continue;
            int id = cit->second;
       
            char digit = ((0xfe) >> (8 - id) | (0xfe << id));
            status =0;

        }
    }
}