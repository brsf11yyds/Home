#include "read.h"
int fi;


void * touch_scan(void *arg)
{
	while(1)
	{	
		printf("Start touch scan\n");
		int x,y;
		touch(&x,&y);
		if(x>625 && x<780 && 25<y && y<60)  {but1=1;printf("But1 = 1\n");}
		if(x>625 && x<780 && 90<y && y<125) {but2=1;printf("But2 = 1\n");}
		if(x>625 && x<780 && 150<y && y<185)  {but3=1;printf("But3 = 1\n");}
		if(x>625 && x<780 && 220<y && y<255)  {but4=1;printf("But4 = 1\n");}
		if(x>625 && x<780 && 290<y && y<325)  {but5=1;printf("But5 = 1\n");}
		if(x>625 && x<780 && 360<y && y<395)  {but6=1;printf("But6 = 1\n");}
		touch_int = 1;
	}
}

void * hand_scan(void *arg)
{
	int fd;
	char tmp,i=100;
	fd = open("/dev/IIC_drv", O_RDWR);
	if(fd < 0)
	{
		perror("open");	
	}
	
	while(1)
	{
		read(fd,&tmp,1);		
		if(tmp>=1 && tmp<=9)
		{
			printf("tmp is %x\n",tmp);			
			switch(tmp)
			{	
				case 1:printf("向上-Up 音量+\n");but4=1;hand_int=1;break;//音量+
				case 2:printf("向下-Down 音量-\n");but5=1;hand_int=1;break;//音量-
				case 3:printf("向左-Left 倒退\n");but3=1;hand_int=1;break;//倒退
				case 4:printf("向右-Right 快进\n");but2=1;hand_int=1;break;//快进
				case 5:printf("前-Forward 播放\n");but1=1;hand_int=1;break;//播放
				//case 6:printf("后-Backward 播放\n");but1=1;hand_int=1;break;//暂停
				case 7:printf("顺时针-Clockwise 退出\n");but6=1;hand_int=1;break;//退出
				case 8:printf("/逆时针-AntiClockwise 退出\n");but6=1;hand_int=1;break;//退出
				//case 9:printf("挥动-Wave\n");break;//挥动
				default:break;
			}
			if(hand_int){sleep(2);}			
		}					
	}	
	close(fd);
}

void * voice_scan(void *arg)
{
	audio();
}

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

int mplay(char *path)
{
	but1=0;but2=0;but3=0;but4=0;but5=0;but6=0;
	mplayer_init();
	char cmd[200]="mplayer -slave -quiet -input file=/tmp/fifo -geometry 0:0  ";	
	strcat(cmd,path);
	strcat(cmd," &");
	system(cmd);
	
	
    Clean_Area(0,0,600,50,0x00ffffff);
	char tem[50]="Now it is plyaing ";
	strcat(tem,path);
	Display_characterX(0,0,tem,0x00ff0000,2);

	pthread_t thread;
    pthread_create(&thread , NULL, touch_scan, NULL );
	pthread_t thread2;
    pthread_create(&thread2 , NULL, hand_scan, NULL );
	pthread_t thread3;
    pthread_create(&thread3 , NULL, voice_scan, NULL );
	int x,y;	
	int playing=1;
	int vol=50;
	char buff[100];

	touch_int = 0;
	hand_int = 0;
	audio_int = 0;
	while(1)
	{
		while(!(touch_int || hand_int || audio_int));
		touch_int = 0;
		hand_int = 0;
		audio_int = 0;
		if(but1)
			{
				//暂停或播放
				printf("pause or play\n");
				send_cmd("pause\n");
			}
			
		if(but4)
			{
				//音量+5				
				vol=((vol+5)>100)?100:(vol+5);
				printf("volume +5 1\n");
				sprintf(buff,"volume %d 1\n",vol);
				send_cmd(buff);
			}				

		if(but5)
			{
				//音量-5				
				vol=((vol-5)<0)?0:(vol-5);
				printf("volume -5 1\n");
				sprintf(buff,"volume %d 1\n",vol);
				send_cmd(buff);
			}	

		if(but2)
			{
				//前进10秒
				printf("seek 10\n");
				send_cmd("seek 10\n");
			}	

		if(but3)
			{
				//后退10秒
				printf("seek -10\n");
				send_cmd("seek -10\n");	
			}

		if(but6)
			{
				//退出
				printf("quit\n");				
				send_cmd("quit\n");
				break;						
			}
		usleep(1000*10);
		but1=0;but2=0;but3=0;but4=0;but5=0;but6=0;					
	}
		
	return 0;
}