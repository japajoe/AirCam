#include "CameraDevice.h"

AirCam::CameraDevice::CameraDevice()
{
    onNewFrame = nullptr;
    width = 640;
    height = 480;
}

AirCam::CameraDevice::~CameraDevice()
{

}

AirCam::CameraDevice::CameraDevice(unsigned int width, unsigned int height)
{
    this->width = width;
    this->height = height;
}

int AirCam::CameraDevice::Start()
{
    v4l2_capability capability;
    v4l2_format imageFormat;
    v4l2_requestbuffers requestBuffer;
    v4l2_buffer queryBuffer;

    // 1.  Open the device
    fd = open("/dev/video0",O_RDWR);

    if(fd < 0)
        return AIRCAM_FAILED_TO_OPEN;

    // 2. Ask the device if it can capture frames
    if(ioctl(fd, VIDIOC_QUERYCAP, &capability) < 0)
        return AIRCAM_DEVICE_CAPABILITIES;

    // 3. Set Image format
    imageFormat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    imageFormat.fmt.pix.width = width;
    imageFormat.fmt.pix.height = height;
    imageFormat.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    imageFormat.fmt.pix.field = V4L2_FIELD_NONE;
    
    // tell the device you are using this format
    if(ioctl(fd, VIDIOC_S_FMT, &imageFormat) < 0)
        return AIRCAM_IMAGE_FORMAT;

    // 4. Request Buffers from the device
    requestBuffer.count = 1; // one request buffer
    requestBuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; // request a buffer wich we an use for capturing frames
    requestBuffer.memory = V4L2_MEMORY_MMAP;

    if(ioctl(fd, VIDIOC_REQBUFS, &requestBuffer) < 0)
        return AIRCAM_REQUEST_BUFFER;

    // 5. Query the buffer to get raw data ie. ask for the requested buffer
    queryBuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    queryBuffer.memory = V4L2_MEMORY_MMAP;
    queryBuffer.index = 0;

    if(ioctl(fd, VIDIOC_QUERYBUF, &queryBuffer) < 0)
        return AIRCAM_BUFFER_INFORMATION;

    // use a pointer to point to the newly created buffer
    // mmap() will map the memory address of the device to an address in memory
    buffer = (char*)mmap(NULL, queryBuffer.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, queryBuffer.m.offset);
    memset(buffer, 0, queryBuffer.length);

    // 6. Get a frame
    // Create a new buffer type so the device knows which buffer we are talking about
    memset(&bufferinfo, 0, sizeof(bufferinfo));
    bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufferinfo.memory = V4L2_MEMORY_MMAP;
    bufferinfo.index = 0;

    //buffer = (char*)mmap(NULL, bufferinfo.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, bufferinfo.m.offset);
    //memset(buffer, 0, bufferinfo.length);

    // Activate streaming
    type = bufferinfo.type;

    if(ioctl(fd, VIDIOC_STREAMON, &type) < 0)
        return AIRCAM_STREAMING;

    return AIRCAM_SUCCESS;
}

int AirCam::CameraDevice::Stop()
{
    if(ioctl(fd, VIDIOC_STREAMOFF, &type) < 0)
        return AIRCAM_END_STREAMING;

    close(fd);

    return AIRCAM_SUCCESS;
}

int AirCam::CameraDevice::Update()
{
    // Queue the buffer
    if(ioctl(fd, VIDIOC_QBUF, &bufferinfo) < 0)
        return AIRCAM_QUEUE_BUFFER;

    // Dequeue the buffer
    if(ioctl(fd, VIDIOC_DQBUF, &bufferinfo) < 0)
        return AIRCAM_DEQUEUE_BUFFER;

    // Frames get written after dequeuing the buffer

    //std::cout << "Buffer has: " << (double)bufferinfo.bytesused / 1024  << " KBytes of data\n";

    if(onNewFrame != nullptr)
        onNewFrame(buffer, bufferinfo.length);

    return AIRCAM_SUCCESS;
}
