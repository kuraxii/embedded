
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <signal.h>

static int fd;

/*
 * ./button_test /dev/myds18b20
 *
 */
int main(int argc, char **argv)
{
	int buf[2];
	int ret;

	int i;
	
	/* 1. 判断参数 */
	if (argc != 2) 
	{
		printf("Usage: %s <dev>\n", argv[0]);
		return -1;
	}


	/* 2. 打开文件 */
	fd = open(argv[1], O_RDWR | O_NONBLOCK);
	if (fd == -1)
	{
		printf("can not open file %s\n", argv[1]);
		return -1;
	}

	while (1) 
	{
		if (read(fd, buf, 8) == 8)
			printf("get ds18b20: %d.%d\n", buf[0], buf[1]);
		else
			printf("get ds18b20: -1\n");
		
		sleep(5);
	}
	
	close(fd);
	
	return 0;
}


