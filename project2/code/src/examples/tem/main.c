#include "read.h"
//线程1 语音识别

//线程2 手势识别


int main()
{
    
	clearlcd();
	show_bmp("welcome.bmp"); //欢迎界面
	sleep(1);
	do_beep();

	Init_Font();
    //用户登录
	data();
	do_beep();

    show_bmp("main.bmp");
	int x,y;
    while(1)  
		{		
			touch(&x ,&y); 
			if(500<x && x<745 && 30<y && y<90)
			{
				//开灯
				while(1)
				{
				clearlcd();	
				show_bmp("light.bmp");
				LED ();	
				show_bmp("main.bmp");
				break;
				}				
			}
            else if(500<x && x<745 && 140<y && y<200)
			{
				clearlcd();	
				album();//相册
				show_bmp("main.bmp");
			}	
            else if(500<x && x<745 && 240<y && y<300)
			{
				
				clearlcd();	
				show_bmp("play.bmp");
				mplay("1.avi");
				show_bmp("main.bmp");//视频播放函数
			}	
            else if(500<x && x<745 && 330<y && y<390)
			{
				clearlcd();	
				show_bmp("play.bmp");
				mplay("Apologize.mp3");
				show_bmp("main.bmp");//音乐
			}	

		}
	UnInit_Font();
    return 0;
}