#include "read.h"
//线程1 语音识别

//线程2 手势识别

int main()
{
    
	clearlcd();
	show_bmp("welcome.bmp"); //欢迎界面
	sleep(1);
	do_beep();
	
    //用户登录

	// pthread_t thread;
    // pthread_create(&thread , NULL, hand, NULL );
	//pthread_t thread2;
	//pthread_create(&thread2 , NULL, voice, NULL );

    show_bmp("jiemian.bmp");
	int x,y;
    while(1)  //临时与普通卡选择
		{		
			touch(&x ,&y); 
			if(30<x && x<280 && 130<y && y<250)
			{
				//开灯
			}
			if(620<x && x<870 && 130<y && y<250)
			{
				//关灯
			}	
            if(620<x && x<870 && 130<y && y<250)
			{
				//音乐播放函数
			}	
            if(620<x && x<870 && 130<y && y<250)
			{
				//视频播放函数
			}	
            if(620<x && x<870 && 130<y && y<250)
			{
				//相册
			}	

		}
    return 0;
}