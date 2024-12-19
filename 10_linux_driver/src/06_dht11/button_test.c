
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
 * ./button_test /dev/mydht11
 *
 */
int main(int argc, char **argv)
{
	char buf[2];
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
		if (read(fd, buf, 2) == 2)
			printf("get Humidity: %d, Temperature : %d\n", buf[0], buf[1]);
		else
			printf("get dht11: -1\n");

		sleep(5);
	}

	//sleep(30);

	close(fd);
	
	return 0;
}


