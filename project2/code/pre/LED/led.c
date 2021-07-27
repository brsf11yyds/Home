#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>

#define LED1 _IO(TEST_MAGIC, 0)
#define LED2 _IO(TEST_MAGIC, 1)
#define LED3 _IO(TEST_MAGIC, 2)
#define LED4 _IO(TEST_MAGIC, 3) 
#define TEST_MAGIC 'x'

#define LED_ON  0	
#define LED_OFF	1   

int do_led()
{
	//1. open
	int fd;
	fd = open("/dev/Led",O_RDWR);
	if(fd < 0)
	{
		 printf("open led error!\n");
	}
	ioctl(fd,LED2,LED_OFF);
	ioctl(fd,LED3,LED_OFF);
	ioctl(fd,LED4,LED_OFF);
	//2. ioctl
	int i;
	for(i=0;i<10;i++)
	{
		ioctl(fd,LED1,LED_ON);
		sleep(1);
		ioctl(fd,LED1,LED_OFF);
		sleep(1);
	}
	
	//3. close
	close(fd);
	
	return 0;
}