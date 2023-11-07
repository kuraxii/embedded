#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <termios.h>
#include <unistd.h>

#define REQBUFS_COUNT 4

struct cam_buf
{
    void *start;
    int length;
};

struct v4l2_requestbuffers reqbufs;
struct cam_buf bufs[REQBUFS_COUNT];

int camera_init(char *devpath, unsigned int *width, unsigned int *height, unsigned int *size, unsigned int *ismjpeg)
{
    int i;
    int fd = -1;
    ;
    int ret;
    struct v4l2_buffer vbuf;
    struct v4l2_format format;
    struct v4l2_capability capability;
    /*open 打开设备文件*/
    if ((fd = open(devpath, O_RDWR)) == -1)
    {
        perror("open camera:");
        return -1;
    }
    /*ioctl 查看支持的驱动*/
    ret = ioctl(fd, VIDIOC_QUERYCAP, &capability);
    if (ret == -1)
    {
        perror("camera->init");
        return -1;
    }
    /*判断设备是否支持视频采集*/
    if (!(capability.capabilities & V4L2_CAP_VIDEO_CAPTURE))
    {
        fprintf(stderr, "camera->init: device can not support V4L2_CAP_VIDEO_CAPTURE\n");
        close(fd);
        return -1;
    }
    /*判断设备是否支持视频流采集*/
    if (!(capability.capabilities & V4L2_CAP_STREAMING))
    {
        fprintf(stderr, "camera->init: device can not support V4L2_CAP_STREAMING\n");
        close(fd);
        return -1;
    }

    /*设置捕获的视频格式 MJPEG*/
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    format.fmt.pix.width = *width;
    format.fmt.pix.height = *height;
    format.fmt.pix.field = V4L2_FIELD_ANY;
    ret = ioctl(fd, VIDIOC_S_FMT, &format);
    if (ret == -1)
    {
        perror("camera init");
        return -1;
    }
    else
    {
        *ismjpeg = 0;
        fprintf(stdout, "camera->init: picture format is mjpeg\n");
        goto get_fmt;
    }

    /*设置捕获的视频格式 YUYV*/
    memset(&format, 0, sizeof(format));
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;      //永远都是这个类型
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; //设置采集图片的格式
    format.fmt.pix.width = *width;
    format.fmt.pix.height = *height;
    format.fmt.pix.field = V4L2_FIELD_ANY;  //设置图片一行一行的采集
    ret = ioctl(fd, VIDIOC_S_FMT, &format); // ioctl	是设置生效
    if (ret == -1)
        perror("camera init");
    else
    {
        fprintf(stdout, "camera->init: picture format is yuyv\n");
        *ismjpeg = 1;
        goto get_fmt;
    }

get_fmt:
    ret = ioctl(fd, VIDIOC_G_FMT, &format);
    if (ret == -1)
    {
        perror("camera init");
        return -1;
    }
    /*向驱动申请缓存*/
    memset(&reqbufs, 0, sizeof(struct v4l2_requestbuffers));
    reqbufs.count = REQBUFS_COUNT; //缓存区个数
    reqbufs.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbufs.memory = V4L2_MEMORY_MMAP; //设置操作申请缓存的方式:映射 MMAP
    ret = ioctl(fd, VIDIOC_REQBUFS, &reqbufs);
    if (ret == -1)
    {
        perror("camera init");
        close(fd);
        return -1;
    }
    /*循环映射并入队*/
    for (i = 0; i < reqbufs.count; i++)
    {
        /*真正获取缓存的地址大小*/
        memset(&vbuf, 0, sizeof(struct v4l2_buffer));
        vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        vbuf.memory = V4L2_MEMORY_MMAP;
        vbuf.index = i;
        ret = ioctl(fd, VIDIOC_QUERYBUF, &vbuf);
        if (ret == -1)
        {
            perror("camera init");
            close(fd);
            return -1;
        }
        /*映射缓存到用户空间,通过mmap讲内核的缓存地址映射到用户空间,并切和文件描述符fd相关联*/
        bufs[i].length = vbuf.length;
        bufs[i].start = mmap(NULL, vbuf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, vbuf.m.offset);
        if (bufs[i].start == MAP_FAILED)
        {
            perror("camera init");
            close(fd);
            return -1;
        }
        /*每次映射都会入队,放入缓冲队列*/
        vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        vbuf.memory = V4L2_MEMORY_MMAP;
        ret = ioctl(fd, VIDIOC_QBUF, &vbuf);
        if (ret == -1)
        {
            perror("camera init");
            close(fd);
            return -1;
        }
    }
    /*返回真正设置成功的宽.高.大小*/
    *width = format.fmt.pix.width;
    *height = format.fmt.pix.height;
    *size = bufs[0].length;

    return fd;
}

int camera_start(int fd)
{
    int ret;

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    /*ioctl控制摄像头开始采集*/
    ret = ioctl(fd, VIDIOC_STREAMON, &type);
    if (ret == -1)
    {
        perror("camera->start");
        return -1;
    }
    fprintf(stdout, "camera->start: start capture\n");

    return 0;
}

int camera_dqbuf(int fd, void **buf, unsigned int *size, unsigned int *index)
{
    int ret;
    fd_set fds;
    struct timeval timeout;
    struct v4l2_buffer vbuf;

    while (1)
    {
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;
        ret = select(fd + 1, &fds, NULL, NULL,
                     &timeout); //使用select机制,保证fd有图片的时候才能出对
        if (ret == -1)
        {
            perror("camera->dbytesusedqbuf");
            if (errno == EINTR)
                continue;
            else
                return -1;
        }
        else if (ret == 0)
        {
            fprintf(stderr, "camera->dqbuf: dequeue buffer timeout\n");
            return -1;
        }
        else
        {
            vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            vbuf.memory = V4L2_MEMORY_MMAP;
            ret = ioctl(fd, VIDIOC_DQBUF, &vbuf); //出队,也就是从用户空间取出图片
            if (ret == -1)
            {
                perror("camera->dqbuf");
                return -1;
            }

            *buf = bufs[vbuf.index].start;
            *size = vbuf.bytesused;
            *index = vbuf.index;

            return 0;
        }
    }
}

int camera_eqbuf(int fd, unsigned int index)
{
    int ret;
    struct v4l2_buffer vbuf;

    vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    vbuf.memory = V4L2_MEMORY_MMAP;
    vbuf.index = index;
    ret = ioctl(fd, VIDIOC_QBUF, &vbuf); //入队
    if (ret == -1)
    {
        perror("camera->eqbuf");
        return -1;
    }

    return 0;
}

int camera_stop(int fd)
{
    int ret;
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    ret = ioctl(fd, VIDIOC_STREAMOFF, &type);
    if (ret == -1)
    {
        perror("camera->stop");
        return -1;
    }
    fprintf(stdout, "camera->stop: stop capture\n");

    return 0;
}

int camera_exit(int fd)
{
    int i;
    int ret;
    struct v4l2_buffer vbuf;
    vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    vbuf.memory = V4L2_MEMORY_MMAP;
    for (i = 0; i < reqbufs.count; i++)
    {
        ret = ioctl(fd, VIDIOC_DQBUF, &vbuf);
        if (ret == -1)
            break;
    }
    for (i = 0; i < reqbufs.count; i++)
        munmap(bufs[i].start, bufs[i].length);
    fprintf(stdout, "camera->exit: camera exit\n");
    return close(fd);
}