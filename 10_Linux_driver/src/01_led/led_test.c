
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <signal.h>

static int fd;


//int led_on(int which);
//int led_off(int which);
//int led_status(int which);

/*
 * ./led_test <0|1|2|..>  on 
 * ./led_test <0|1|2|..>  off
 * ./led_test <0|1|2|..>
 */
int main(int argc, char **argv)
{
	int ret;
	char buf[2];

	int i;
	
	/* 1. 判断参数 */
	if (argc < 2) 
	{
		printf("Usage: %s <0|1|2|...> [on | off]\n", argv[0]);
		return -1;
	}


	/* 2. 打开文件 */
	fd = open("/dev/100ask_led", O_RDWR);
	if (fd == -1)
	{
		printf("can not open file /dev/100ask_led\n");
		return -1;
	}

	if (argc == 3)
	{
		/* write */
		buf[0] = strtol(argv[1], NULL, 0);

		if (strcmp(argv[2], "on") == 0)
			buf[1] = 0;
		else
			buf[1] = 1;
		
		ret = write(fd, buf, 2);
	}
	else
	{
		buf[0] = strtol(argv[1], NULL, 0);
		ret = read(fd, buf, 2);
		if (ret == 2)
		{
			printf("led %d status is %s\n", buf[0], buf[1] == 0 ? "on" : "off");
		}
	}
	
	close(fd);
	
	return 0;
}


