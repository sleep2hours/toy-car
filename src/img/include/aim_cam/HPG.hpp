#ifndef _HP_GRABBING_HPP
#define _HP_GRABBING_HPP

#include <mutex>
#include <queue>
#include <atomic>
#include <thread>
#include <iostream>
#include <opencv2/core.hpp>
#include "CameraCtl.hpp"



// #define HPG_DEBUG
#ifndef HPG_DEBUG
#define hpg_debug(...)
#else
#define hpg_debug printf
#endif

namespace hpg
{
    const int NULL_POS = -1;
    const int CYCLE_TIMES = 3; //高性能取流明暗曝光交替周期(帧数)
    const int GRAB_OK = 0, GRAB_FAILED = 1;

    enum MODE
    {
        FROM_CAM = 0,             // 高性能交替曝光
        FROM_VIDEO = 1,           // 从视频取流
        FROM_CAM_LOW_PERFORM = 2, // 低性能
        FROM_NEW_CAM = 4,         // 新海康摄像头
    };

    struct ImageBuffer
    {
        volatile std::atomic<bool> r_busy;
        volatile std::atomic<bool> w_busy;
        MV_FRAME_OUT frame; //帧数据
    };

    class HPGrabbing
    {
    public:
        HPGrabbing();
        ~HPGrabbing();

        int getMat(cv::Mat &src); //src入参/输出, high_exp:是否是高曝光

        void start(); //外部开启并detach进程

        /// 对应了Frame中的ROBOT_STATUS这个enum中的值
        /// 设置相机参数 (以及敌方颜色信息(颜色不同参数不同))
        void setCameraParams(int mode);

        inline void setBufferState()
        {
            if (grab_mode != 0)
                return;
            buf[last_pos].r_busy = true; //一个位置只能解码一次
            buf[last_pos].w_busy = false;
            hpg_debug("Reading succeeded. [%d].r_busy, w_busy=(%d, %d)\n",
                      last_pos, buf[last_pos].r_busy, buf[last_pos].w_busy);
        }

    public:
        bool grab;            //是否进行grab的标签
        cv::VideoCapture cap; //对视频播放进行集成
    private:
        void zoomGrabbing(); // Hikvision SDK 2.0 + 新镜头 的166fps取流
        /** 
     * @brief 写入操作线程主循环，
     * @param pUser CameraCtl中的相机操作句柄（handle）
    */
        void *writeWorkThread(void *pUser);
        bool isWritable(const int pos);               //bool:pos是否可写
        void readBuffer(const int pos, cv::Mat &src); //对pos进行写入操作，包括解码和图像处理

        /**
     * @brief 对缓冲区的pos位置进行写入操作
     * @param pUser 相机控制模块中的handle
     * @param pos 缓冲区下标，取值为0或1（双缓冲）
     * @param e 入参/输出-SDK错误代码
     * @return 下一个写入的缓冲区下标
    */
        int writeBuffer(int pos, int &e);

    private:
        int next_pos;  //下一次读取的位置
        int last_pos;  //上一次读取的位置
        int grab_mode; //取流方式from_cam还是from_video
        int exp_short;
        int exp_long;
        int balance_r;
        int balance_b;
        cm::CameraCtl ctl;
        ImageBuffer buf[2]; //乒乓缓冲池

        std::mutex mtx;
        std::deque<cv::Mat> img_buff;
    };
} //namespace hpg
#endif //_HP_GRABBING_HPP