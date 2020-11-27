#ifndef CAMERADEVICE_HPP
#define CAMERADEVICE_HPP

#include <functional>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/v4l2-common.h>
#include <linux/v4l2-controls.h>
#include <linux/videodev2.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>
#include <fstream>
#include <string>

typedef std::function<void(char* buffer, size_t size)> NewFrameEvent;

enum AirCamError
{
    AIRCAM_SUCCESS = 0,
    AIRCAM_FAILED_TO_OPEN = 1,
    AIRCAM_DEVICE_CAPABILITIES = 2,
    AIRCAM_IMAGE_FORMAT = 3,
    AIRCAM_REQUEST_BUFFER = 4,
    AIRCAM_BUFFER_INFORMATION = 5,
    AIRCAM_STREAMING = 6,
    AIRCAM_QUEUE_BUFFER = 7,
    AIRCAM_DEQUEUE_BUFFER = 8,
    AIRCAM_END_STREAMING = 9    
};

namespace AirCam
{
    class CameraDevice
    {
    public:
        NewFrameEvent onNewFrame;
        CameraDevice();
        CameraDevice(unsigned int width, unsigned int height);
        ~CameraDevice();
        int Start();
        int Stop();
        int Update();
    private:
        int fd;
        v4l2_buffer bufferinfo;
        int type;
        char* buffer;
        unsigned int width;
        unsigned int height;
    };
}
#endif
