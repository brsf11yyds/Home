#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
int fi;

void send_cmd(char *cmd)
{
	write(fi,cmd,strlen(cmd));
}
//创建管道文件
void mplayer_init(void)
{
	if(access("/tmp/fifo",F_OK))
	{
		mkfifo("/tmp/fifo",0777);
	}
	
	fi = open("/tmp/fifo",O_RDWR);
}

int main(int argc, char **argv)
{
	mplayer_init();
	
	system("./mplayer -slave -quiet -input file=/tmp/fifo -geometry 0:0 -zoom -x 800 -y 480 Faded3.avi &");
	
	struct input_event buf;
	int x,y;
	int mute=0;
	
	int fd = open("/dev/input/event0",O_RDWR);
	
	while(1)
	{
		read(fd,&buf,sizeof(buf));
		
		if(buf.type == EV_ABS && buf.code == ABS_X)
		{
			x = buf.value;
		}
		
		if(buf.type == EV_KEY && buf.code == BTN_TOUCH && buf.value == 0)
		{
			if(x<100)
			{
				//暂停或播放
				printf("pause or play\n");
				send_cmd("pause\n");
			}
			
			if(x>=100 && x<200)
			{
				//音量调整为10%
				printf("volume 10 1\n");
				send_cmd("volume 10 1\n");
			}				

			if(x>=200 && x<300)
			{
				//音量调整为50%
				printf("volume 50 1\n");
				send_cmd("volume 50 1\n");
			}	

			if(x>=300 && x<400)
			{
				//音量调整为100%
				printf("volume 100 1\n");
				send_cmd("volume 100 1\n");
			}	

			if(x>=400 && x<500)
			{
				mute=!mute;
				
				//静音开关
				if(mute)
				{
					//静音开
					printf("mute\n");
					send_cmd("mute 1\n");					
					
				}
				else
				{
					//静音关
					printf("no mute\n");
					send_cmd("mute 0\n");						
				}
			}	
			if(x>=500 && x<600)
			{

				//后退10秒
				printf("seek -10\n");
				send_cmd("seek -10\n");						

			}			
			if(x>=600 && x<700)
			{

				//前进10秒
				printf("seek 10\n");
				send_cmd("seek 10\n");						

			}				
			if(x > 700)
			{
				//退出
				printf("quit\n");				
				send_cmd("quit\n");
				break;
			}
		}
	}
	
	close(fd);
	
	return 0;
}