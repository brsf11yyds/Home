#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

/* 
接线说明： 手势识别模块<--连接---> GEC6818
			       VCC			       PIN1
				   GND			       PIN19/20
				   SCL			       PIN15
				   SDA			       PIN17
 */



int main(void)
{

	int fd;
	char tmp,i=100;
	fd = open("/dev/IIC_drv", O_RDWR);
	if(fd < 0)
	{
		perror("open");
		return -1;	
	}
	
	while(1)
	{
		read(fd,&tmp,1);
		
		if(tmp>=1 && tmp<=9)
		{
			printf("%x\n",tmp);
			
			switch(tmp)
			{	
				case 1:printf("向上-Up\n");break;//向上
				case 2:printf("向下-Down\n");break;//向下
				case 3:printf("向左-Left\n");break;//向左
				case 4:printf("向右-Right\n");break;//向右
				case 5:printf("前-Forward\n");break;//前
				case 6:printf("后-Backward\n");break;//后
				case 7:printf("顺时针-Clockwise\n");break;//顺时针
				case 8:printf("/逆时针-AntiClockwise\n");break;//逆时针
				case 9:printf("挥动-Wave\n");break;//挥动
				default:break;
			}
			
		}
				
		

	}

	
	close(fd);
	return 0;
}