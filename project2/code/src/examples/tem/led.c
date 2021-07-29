#include "read.h"
int touch_flag;

void * water(void *argv)
{	
	touch_flag=0;
	int x,y;
	touch(&x,&y);
	touch_flag=1;
}

void * touch_scan1(void *arg)
{
	while(1)
	{	
		printf("Start touch scan\n");
		int x,y;
		touch(&x,&y);
		if(460<x && x<555 && 30<y && y<70)    {led_but1=1;printf("led_but1 = 1\n");}
		if(655<x && x<750 && 30<y && y<70)    {led_but2=1;printf("led_but2 = 1\n");}
		if(460<x && x<555 && 115<y && y<155)  {led_but3=1;printf("led_but3 = 1\n");}
		if(655<x && x<750 && 110<y && y<155)  {led_but4=1;printf("led_but4 = 1\n");}
		if(460<x && x<555 && 215<y && y<255)  {led_but5=1;printf("led_but5 = 1\n");}
		if(655<x && x<750 && 215<y && y<255)  {led_but6=1;printf("led_but6 = 1\n");}
		if(460<x && x<555 && 320<y && y<360)  {led_but7=1;printf("led_but7 = 1\n");}
		if(655<x && x<750 && 320<y && y<360)  {led_but8=1;printf("led_but8 = 1\n");}
		if(555<x && x<750 && 420<y && y<460)  {led_but9=1;printf("led_but9 = 1\n");}
		if(710<x && x<800 && 430<y && y<480)  {led_but10=1;printf("led_but10 = 1\n");}
		touch_int = 1;
	}
}

void * voice_scan1(void *arg)
{
	audio();
}

int LED ()
{

	int fd;
	fd = open("/dev/Led",O_RDWR);
	if(fd < 0)
	{
		 printf("open led error!\n");
	}

	led_but1=0;led_but2=0;led_but3=0;led_but4=0;led_but5=0;
	led_but6=0;led_but7=0;led_but8=0;led_but9=0;led_but10=0;

	
	pthread_t thread;
    pthread_create(&thread , NULL, touch_scan1, NULL );
	pthread_t thread3;
    pthread_create(&thread3 , NULL, voice_scan1, NULL );

	touch_int = 0;
	audio_int = 0;
	int x,y;
	while(1)
	{
		while(!(touch_int || audio_int));
		touch_int = 0;
		audio_int = 0;
		if(led_but1)//1
		{
			ioctl(fd,LED1,LED_ON);
		}
		if(led_but2)
		{
			ioctl(fd,LED1,LED_OFF);
		}

		if(led_but3)//2
		{
			ioctl(fd,LED2,LED_ON);
		}
		if(led_but4)
		{
			ioctl(fd,LED2,LED_OFF);
		}

		if(led_but5)//3
		{
			ioctl(fd,LED3,LED_ON);
		}
		if(led_but6)
		{
			ioctl(fd,LED3,LED_OFF);
		}

		if(led_but7)//4
		{
			ioctl(fd,LED4,LED_ON);
		}
		if(led_but8)
		{
			ioctl(fd,LED4,LED_OFF);
		}

		if(led_but9)
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


		if(led_but10)
		{
			break;
		}

		usleep(1000*10);
		led_but1=0;led_but2=0;led_but3=0;led_but4=0;led_but5=0;
		led_but6=0;led_but7=0;led_but8=0;led_but9=0;led_but10=0;	
	}
	close(fd);	
	return 0;
}

