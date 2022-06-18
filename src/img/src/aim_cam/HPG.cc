#include "../../include/aim_cam/HPG.hpp"

const int max_size = 12;

namespace hpg
{

    HPGrabbing::HPGrabbing()
    {
        for (int i = 0; i < 2; ++i)
        {
            buf[i].r_busy = true; //初始时默认不可读取，需等待第一次写入成功后方可读入
            buf[i].w_busy = false;
            buf[i].frame.pBufAddr = NULL;
        }
        bool init_color_blue = true;
        int mode = 0;
        grab_mode = mode;
        balance_r = 3600;
        balance_b = 1997;
        exp_short = 4000;
        exp_long = 4000;
        next_pos = 1;
        last_pos = NULL_POS;
        grab = true;
    }

    HPGrabbing::~HPGrabbing() { ; }

    void HPGrabbing::start()
    {
        ctl.startGrabbing();
        std::thread job(&HPGrabbing::zoomGrabbing, this);
        job.detach();
    }

    int HPGrabbing::getMat(cv::Mat &src)
    {
        mtx.lock();
        int res = GRAB_OK;
        if (img_buff.size() > 0)
        {
            img_buff.back().copyTo(src);
            img_buff.pop_front();
        }
        else
            res = GRAB_FAILED;
        mtx.unlock();
        return res;
    }

    void HPGrabbing::setCameraParams(int mode)
    {
        ctl.setExposureTime(8000);
        ctl.setWhiteBalance(4000, 1024, 1500);
    }


    void *HPGrabbing::writeWorkThread(void *pUser)
    {
        int next = 0, pos = 0, error = MV_OK;
        while (1)
        {
            pos = next;                      //保留本次写入位置
            next = writeBuffer(next, error); //每次写入之前会free上一次写入的buffer
            if (buf[pos].frame.pBufAddr != NULL)
            { //先释放存在已经写入数据的位置
                error = MV_CC_FreeImageBuffer(pUser, &buf[pos].frame);
#ifdef HPG_DEBUG
                if (error != MV_OK)
                {
                    hpg_debug("Freeing previous frame failed[0x%x].\n", error);
                }
                hpg_debug("Freed one spot.\n");
#endif
            }
            if (!grab)
                break;
        }
        hpg_debug("Exiting thread: writing...\n");
        return 0;
    }

    void HPGrabbing::zoomGrabbing()
    {
        while (1)
        {
            cv::Mat frame = ctl.getOpencvMat();
            if (frame.empty())
            {
                continue;
            }
            mtx.lock();
            img_buff.emplace_back(frame);
            if (img_buff.size() > max_size)
            {
                img_buff.pop_front();
            }
            mtx.unlock();
        }
    }

    int HPGrabbing::writeBuffer(int pos, int &e)
    {
        if (isWritable(pos))
        {
            buf[pos].w_busy = true;
            buf[pos].r_busy = true;
            e = MV_CC_GetImageBuffer(ctl.handle, &buf[pos].frame, 1000);
            if (e != MV_OK)
            {
                hpg_debug("Failed to get image buffer.[0x%x]\n", e);
                return NULL_POS;
            }
        }
        buf[pos].w_busy = true;
        buf[pos].r_busy = false; //对应位置写入完毕，不可写入，可以读取
        hpg_debug("Writing succeeded. [%d].r_busy, w_busy=(%d, %d)\n",
                  pos, buf[pos].r_busy, buf[pos].w_busy);
        next_pos = pos; //可以read的位置在pos处（本次读取位置）
        return 1 - pos; //返回下一个写入位置
    }

    void HPGrabbing::readBuffer(const int pos, cv::Mat &src)
    {
        buf[pos].r_busy = true;
        buf[pos].w_busy = true;
        //================解码==================//
        cv::Mat yuyv_img(1080, 1440, CV_8UC2, buf[pos].frame.pBufAddr);
        cv::cvtColor(yuyv_img, src, cv::COLOR_YUV2BGR_YUYV);
        hpg_debug("Decoding is a success.\n");
    }

    bool HPGrabbing::isWritable(const int pos)
    {
        hpg_debug("Writing to [%d], isWritable:%d, isReadable:%d.\n",
                  pos, buf[pos].w_busy, buf[pos].r_busy);
        while (buf[pos].w_busy == true)
        { //自旋锁,无超时保护
            std::this_thread::yield();
        }
        return true;
    }
} //namespace hpg