#include "read.h"
int touch_flag;

void * water(void *argv)
{	
	touch_flag=0;
	int x,y;
	touch(&x,&y);
	touch_flag=1;
}

int LED ()
{

	int fd;
	fd = open("/dev/Led",O_RDWR);
	if(fd < 0)
	{
		 printf("open led error!\n");
	}

	int x,y;
	while(1)
	{
		touch(&x ,&y);
		if(460<x && x<555 && 30<y && y<70)//1
		{
			ioctl(fd,LED1,LED_ON);
		}
		if(655<x && x<750 && 30<y && y<70)
		{
			ioctl(fd,LED1,LED_OFF);
		}

		if(460<x && x<555 && 115<y && y<155)//2
		{
			ioctl(fd,LED2,LED_ON);
		}
		if(655<x && x<750 && 110<y && y<155)
		{
			ioctl(fd,LED2,LED_OFF);
		}

		if(460<x && x<555 && 215<y && y<255)//3
		{
			ioctl(fd,LED3,LED_ON);
		}
		if(655<x && x<750 && 215<y && y<255)
		{
			ioctl(fd,LED3,LED_OFF);
		}

		if(460<x && x<555 && 320<y && y<360)//4
		{
			ioctl(fd,LED4,LED_ON);
		}
		if(655<x && x<750 && 320<y && y<360)
		{
			ioctl(fd,LED4,LED_OFF);
		}

		if(555<x && x<750 && 420<y && y<460)
		{
			ioctl(fd,LED1,LED_OFF);
			ioctl(fd,LED2,LED_OFF);
			ioctl(fd,LED3,LED_OFF);
			ioctl(fd,LED4,LED_OFF);

		touch_flag=0;	
		pthread_t thread;
    	pthread_create(&thread , NULL, water, NULL );
			while(1)
			{	
				ioctl(fd,LED4,LED_OFF);
				ioctl(fd,LED1,LED_ON);
				usleep(75000);

				ioctl(fd,LED1,LED_OFF);
				ioctl(fd,LED2,LED_ON);
				usleep(75000);

				ioctl(fd,LED2,LED_OFF);
				ioctl(fd,LED3,LED_ON);
				usleep(75000);

				ioctl(fd,LED3,LED_OFF);
				ioctl(fd,LED4,LED_ON);
				usleep(75000);
				if(touch_flag)
				{	
				ioctl(fd,LED1,LED_OFF);
				ioctl(fd,LED2,LED_OFF);
				ioctl(fd,LED3,LED_OFF);
				ioctl(fd,LED4,LED_OFF);
					break;}
			}
		}


		if(710<x && x<800 && 430<y && y<480)
		{
			break;
		}
	}
	close(fd);	
	return 0;
}

