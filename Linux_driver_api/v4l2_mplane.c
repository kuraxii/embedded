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

struct plane_start
{
    void *start;
};

struct buffer
{
    struct plane_start *plane_start;
    struct v4l2_plane *planes_buffer;
};


#define REQBUFS_COUNT 4

struct v4l2_requestbuffers reqbufs;
struct buffer bufs[REQBUFS_COUNT];
static int num_plane;
static struct v4l2_plane *planes_buffer;
static struct plane_start *plane_start;
static struct v4l2_plane *tmp_plane;
int camera_init(char *devpath, unsigned int *width, unsigned int *height, unsigned int *nmplane)
{
    int i, j;
    int fd = -1;
    ;
    int ret;

    struct v4l2_buffer vbuf;
    struct v4l2_format format;
    struct v4l2_capability capability;

    /*open 打开设备文件*/
    if ((fd = open(devpath, O_RDWR)) == -1)
    {
        perror("open:");
        return -1;
    }
    /*ioctl 查看支持的驱动*/
    ret = ioctl(fd, VIDIOC_QUERYCAP, &capability);
    if (ret == -1)
    {
        perror("camera->init");
        return -1;
    }
    /*判断设备是否支持视频流采集*/
    if (!(capability.capabilities & V4L2_CAP_STREAMING))
    {
        fprintf(stderr, "camera->init: device can not support V4L2_CAP_STREAMING\n");
        close(fd);
        return -1;
    }

    /*判断设备是否支持视频采集*/
    if (!(capability.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE))
    {
        fprintf(stderr, "camera->init: device can not support V4L2_CAP_VIDEO_CAPTURE_MPLANE\n");
        close(fd);
        return -1;
    }

    printf("Support capture!\n");

    /*设置捕获的视频格式 */
    memset(&format, 0, sizeof(struct v4l2_format));
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    format.fmt.pix_mp.width = *width;
    format.fmt.pix_mp.height = *height;
    format.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_GREY;
    format.fmt.pix_mp.field = V4L2_FIELD_ANY;
    ret = ioctl(fd, VIDIOC_S_FMT, &format);
    if (ret == -1)
    {
        perror("camera init, set format fail");
        return -1;
    }

    fprintf(stdout, "camera->init: picture format set  success\n");
    ret = ioctl(fd, VIDIOC_G_FMT, &format);
    if (ret == -1)
    {
        perror("camera init, get format fail");
        return -1;
    }
    num_plane = format.fmt.pix_mp.num_planes;

    printf("width = %d\n", format.fmt.pix_mp.width);
    printf("height = %d\n", format.fmt.pix_mp.height);
    printf("nmplane = %d\n", format.fmt.pix_mp.num_planes);

    /*向驱动申请缓存*/
    memset(&reqbufs, 0, sizeof(struct v4l2_requestbuffers));
    reqbufs.count = REQBUFS_COUNT; //缓存区个数
    reqbufs.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    reqbufs.memory = V4L2_MEMORY_MMAP; //设置操作申请缓存的方式:映射 MMAP
    ret = ioctl(fd, VIDIOC_REQBUFS, &reqbufs);
    if (ret == -1)
    {
        perror("camera init, reqbufs fail");
        close(fd);
        return -1;
    }

    /*循环映射并入队*/
    for (i = 0; i < reqbufs.count; i++)
    {
        /*真正获取缓存的地址大小*/
        memset(&vbuf, 0, sizeof(struct v4l2_buffer));
        planes_buffer = calloc(num_plane, sizeof(struct v4l2_plane));
        plane_start = calloc(num_plane, sizeof(struct plane_start));

        vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        vbuf.memory = V4L2_MEMORY_MMAP;
        vbuf.index = i;
        vbuf.m.planes = planes_buffer;
        vbuf.length = num_plane;

        ret = ioctl(fd, VIDIOC_QUERYBUF, &vbuf);
        if (ret == -1)
        {
            perror("camera init, querybuf fail");
            close(fd);
            return -1;
        }

        (bufs + i)->planes_buffer = planes_buffer;
        (bufs + i)->plane_start = plane_start;

        /*映射每个平面缓存到用户空间,通过mmap讲内核的缓存地址映射到用户空间*/
        for (j = 0; j < num_plane; j++)
        {
            (plane_start + j)->start = mmap(NULL, (planes_buffer + j)->length, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                                            (planes_buffer + j)->m.mem_offset);
            printf("lib: index: {%d}, mplane: {%p}\n", i, (plane_start + j)->start);
            if (MAP_FAILED == (plane_start + j)->start)
            {
                perror("mmap");
                close(fd);
                return -1;
            }
        }

        /*每次映射都会入队,放入缓冲队列*/
        vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        vbuf.memory = V4L2_MEMORY_MMAP;
        ret = ioctl(fd, VIDIOC_QBUF, &vbuf);
        if (ret == -1)
        {
            perror("camera init, qbuf fail");
            close(fd);
            return -1;
        }
    }
    tmp_plane = calloc(num_plane, sizeof(struct v4l2_plane));
    /*返回真正设置成功的宽.高, 平面数*/
    *width = format.fmt.pix_mp.width;
    *height = format.fmt.pix_mp.height;
    *nmplane = num_plane;
    return fd;
}

int camera_start(int fd)
{
    int ret;

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
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

int camera_dqbuf(int fd, struct plane_start **buf, struct v4l2_plane **plane, unsigned int *index)
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
        ret = select(fd + 1, &fds, NULL, NULL, &timeout); //使用select机制,保证fd有图片的时候才能出对
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
            vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
            vbuf.memory = V4L2_MEMORY_MMAP;
            vbuf.m.planes = tmp_plane;
            vbuf.length = num_plane;
            ret = ioctl(fd, VIDIOC_DQBUF, &vbuf); //出队,也就是从用户空间取出图片
            if (ret == -1)
            {
                perror("camera->dqbuf");
                return -1;
            }
            *buf = bufs[vbuf.index].plane_start;
            *plane = tmp_plane;
            *index = vbuf.index;

            return 0;
        }
    }
}

int camera_eqbuf(int fd, unsigned int index)
{
    int ret;
    struct v4l2_buffer vbuf;

    vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    vbuf.memory = V4L2_MEMORY_MMAP;
    vbuf.index = index;
    vbuf.m.planes = planes_buffer;
    vbuf.length = num_plane;
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
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;

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
    int i, j;
    int ret;
    struct v4l2_buffer vbuf;
    vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    vbuf.memory = V4L2_MEMORY_MMAP;
    for (i = 0; i < reqbufs.count; i++)
    {
        ret = ioctl(fd, VIDIOC_DQBUF, &vbuf);
        if (ret == -1)
            break;
    }
    for (i = 0; i < reqbufs.count; i++)
    {
        free((bufs + i)->plane_start);
        free((bufs + i)->planes_buffer);
        for (j = 0; j < num_plane; j++)
            munmap(((bufs + i)->plane_start + j)->start, ((bufs + i)->planes_buffer + j)->length);

    }

    free(planes_buffer);
    free(plane_start);
    free(tmp_plane);
    fprintf(stdout, "camera->exit: camera exit\n");
    
    return close(fd);
}
