
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <signal.h>
#include <stdlib.h>

static int fd;

/*
 * ./button_test /dev/motor -100  1
 *
 */
int main(int argc, char **argv)
{
	int buf[2];
	int ret;
	
	/* 1. 判断参数 */
	if (argc != 4) 
	{
		printf("Usage: %s <dev> <step_number> <mdelay_number>\n", argv[0]);
		return -1;
	}


	/* 2. 打开文件 */
	fd = open(argv[1], O_RDWR | O_NONBLOCK);
	if (fd == -1)
	{
		printf("can not open file %s\n", argv[1]);
		return -1;
	}

	buf[0] = strtol(argv[2], NULL, 0);
	buf[1] = strtol(argv[3], NULL, 0);

	ret = write(fd, buf, 8);
	close(fd);
	
	return 0;
}


