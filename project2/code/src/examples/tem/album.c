#include "read.h"


BITMAPFILEHEADER1 fileHeader;
BITMAPINFOHEADER1 infoHeader;
int touch_flag;

void * pic(void *argv)
{	
	touch_flag=0;
	int x,y;
    while(1)
    {
    touch(&x,&y);
    if(x<400)
        {touch_flag=1;break;}   
    }
}


int show_bmp1(char *path,int xx ,int yy)
{

    FILE* fp;    
    fp = fopen(path, "rb");//读取同目录下的image.bmp文件。
    if(fp == NULL)
    {
        printf("打开'image.bmp'失败！\n");
        return -1;
    }
    //如果不先读取bifType，根据C语言结构体Sizeof运算规则——整体大于部分之和，从而导致读文件错位
    unsigned short  fileType;   
    fread(&fileType,1,sizeof (unsigned short), fp);  
    if (fileType = 0x4d42)   
    {   
        //printf("文件类型标识正确!" );  
        //printf("\n文件标识符：%d\n", fileType); 
        fread(&fileHeader, 1, sizeof(BITMAPFILEHEADER1), fp);
        showBmpHead(fileHeader);
        fread(&infoHeader, 1, sizeof(BITMAPINFOHEADER1), fp);
        showBmpInfoHead(infoHeader);
        fclose(fp);        
    }

    int bmp_fd = open(path, O_RDWR);
	if(bmp_fd == -1)
	{
		printf("3.failed\n");
		return -1;
	}

    int lcd_fd = open("/dev/fb0", O_RDWR);
	if(lcd_fd == -1)
	{
		printf("lcd.failed\n");
		return -1;
	}

    //read data of bmp
	lseek(bmp_fd ,54 ,SEEK_SET);
    lseek(lcd_fd , 0 ,SEEK_SET);

    char bmp_buf[infoHeader.biWidth*infoHeader.biHeight*3];
    bzero(bmp_buf , sizeof(bmp_buf));
    read(bmp_fd ,bmp_buf ,sizeof(bmp_buf));

    int *mmap_fd =(int *)mmap(NULL,800*480*4 ,PROT_READ | PROT_WRITE,MAP_SHARED,lcd_fd,0);

    int x,y;
    for(x=0;x<infoHeader.biWidth;x++)
    {
        usleep(300);
        for(y=0;y<infoHeader.biHeight;y++)
        {
            *(mmap_fd+x+xx+(infoHeader.biHeight-1-y+yy)*800) =bmp_buf[x*3+y*infoHeader.biWidth*3] | bmp_buf[x*3+1+y*infoHeader.biWidth*3] << 8 | bmp_buf[x*3+2+y*infoHeader.biWidth*3] << 16;
        }
    }

    munmap(mmap_fd , 800*480*4);

    close(bmp_fd);
    close(lcd_fd);    
}

int show_bmp2(char *path,int xx ,int yy)
{

    FILE* fp;    
    fp = fopen(path, "rb");//读取同目录下的image.bmp文件。
    if(fp == NULL)
    {
        printf("打开'image.bmp'失败！\n");
        return -1;
    }
    //如果不先读取bifType，根据C语言结构体Sizeof运算规则——整体大于部分之和，从而导致读文件错位
    unsigned short  fileType;   
    fread(&fileType,1,sizeof (unsigned short), fp);  
    if (fileType = 0x4d42)   
    {   
        //printf("文件类型标识正确!" );  
        //printf("\n文件标识符：%d\n", fileType); 
        fread(&fileHeader, 1, sizeof(BITMAPFILEHEADER1), fp);
        showBmpHead(fileHeader);
        fread(&infoHeader, 1, sizeof(BITMAPINFOHEADER1), fp);
        showBmpInfoHead(infoHeader);
        fclose(fp);        
    }

    int bmp_fd = open(path, O_RDWR);
	if(bmp_fd == -1)
	{
		printf("3.failed\n");
		return -1;
	}

    int lcd_fd = open("/dev/fb0", O_RDWR);
	if(lcd_fd == -1)
	{
		printf("lcd.failed\n");
		return -1;
	}

    //read data of bmp
	lseek(bmp_fd ,54 ,SEEK_SET);
    lseek(lcd_fd , 0 ,SEEK_SET);

    char bmp_buf[infoHeader.biWidth*infoHeader.biHeight*3];
    bzero(bmp_buf , sizeof(bmp_buf));
    read(bmp_fd ,bmp_buf ,sizeof(bmp_buf));

    int *mmap_fd =(int *)mmap(NULL,800*480*4 ,PROT_READ | PROT_WRITE,MAP_SHARED,lcd_fd,0);

    int x,y;
    for(x=infoHeader.biWidth-1;x>-1;x--)
    {
        usleep(300);
        for(y=0;y<infoHeader.biHeight;y++)
        {
            *(mmap_fd+x+xx+(infoHeader.biHeight-1-y+yy)*800) =bmp_buf[x*3+y*infoHeader.biWidth*3] | bmp_buf[x*3+1+y*infoHeader.biWidth*3] << 8 | bmp_buf[x*3+2+y*infoHeader.biWidth*3] << 16;
        }
    }

    munmap(mmap_fd , 800*480*4);

    close(bmp_fd);
    close(lcd_fd);    
}

int show_bmp3(char *path,int xx ,int yy)
{
    FILE* fp;    
    fp = fopen(path, "rb");//读取同目录下的image.bmp文件。
    if(fp == NULL)
    {
        printf("打开'image.bmp'失败！\n");
        return -1;
    }
    //如果不先读取bifType，根据C语言结构体Sizeof运算规则——整体大于部分之和，从而导致读文件错位
    unsigned short  fileType;   
    fread(&fileType,1,sizeof (unsigned short), fp);  
    if (fileType = 0x4d42)   
    {   
        //printf("文件类型标识正确!" );  
        //printf("\n文件标识符：%d\n", fileType); 
        fread(&fileHeader, 1, sizeof(BITMAPFILEHEADER1), fp);
        showBmpHead(fileHeader);
        fread(&infoHeader, 1, sizeof(BITMAPINFOHEADER1), fp);
        showBmpInfoHead(infoHeader);
        fclose(fp);        
    }

    int bmp_fd = open(path, O_RDWR);
	if(bmp_fd == -1)
	{
		printf("3.failed\n");
		return -1;
	}

    int lcd_fd = open("/dev/fb0", O_RDWR);
	if(lcd_fd == -1)
	{
		printf("lcd.failed\n");
		return -1;
	}

    //read data of bmp
	lseek(bmp_fd ,54 ,SEEK_SET);
    lseek(lcd_fd , 0 ,SEEK_SET);

    char bmp_buf[infoHeader.biWidth*infoHeader.biHeight*3];
    bzero(bmp_buf , sizeof(bmp_buf));
    read(bmp_fd ,bmp_buf ,sizeof(bmp_buf));

    int *mmap_fd =(int *)mmap(NULL,800*480*4 ,PROT_READ | PROT_WRITE,MAP_SHARED,lcd_fd,0);

    int x,y;
    //for(x=0;x<infoHeader.biWidth;x++)
    //{
    //    usleep(300);
    //    for(y=0;y<infoHeader.biHeight;y++)
    //    {
    //        *(mmap_fd+x+xx+(infoHeader.biHeight-1-y+yy)*800) =bmp_buf[x*3+y*infoHeader.biWidth*3] | bmp_buf[x*3+1+y*infoHeader.biWidth*3] << 8 | bmp_buf[x*3+2+y*infoHeader.biWidth*3] << 16;
    //    }
    //}
    int row,col;
    int m = 299;
    int n = 499;
    row = 0;
    col = 0;
    while( row <= n || col <= m )
    {
        usleep(5000);
        for( int x = row, y = col; y <= m && row <= n; y++ )
            *(mmap_fd+x+xx+(infoHeader.biHeight-1-y+yy)*800) =bmp_buf[x*3+y*infoHeader.biWidth*3] | bmp_buf[x*3+1+y*infoHeader.biWidth*3] << 8 | bmp_buf[x*3+2+y*infoHeader.biWidth*3] << 16;
        row++;
        for( int x = row, y = m; x <= n && col <= m; x++ )
            *(mmap_fd+x+xx+(infoHeader.biHeight-1-y+yy)*800) =bmp_buf[x*3+y*infoHeader.biWidth*3] | bmp_buf[x*3+1+y*infoHeader.biWidth*3] << 8 | bmp_buf[x*3+2+y*infoHeader.biWidth*3] << 16;
        m--;
        for( int x = n, y = m; y >= col && row <= n; y-- )
            *(mmap_fd+x+xx+(infoHeader.biHeight-1-y+yy)*800) =bmp_buf[x*3+y*infoHeader.biWidth*3] | bmp_buf[x*3+1+y*infoHeader.biWidth*3] << 8 | bmp_buf[x*3+2+y*infoHeader.biWidth*3] << 16;
        n--;
        for( int x = n, y = col; x >= row && col <= m; x-- )
            *(mmap_fd+x+xx+(infoHeader.biHeight-1-y+yy)*800) =bmp_buf[x*3+y*infoHeader.biWidth*3] | bmp_buf[x*3+1+y*infoHeader.biWidth*3] << 8 | bmp_buf[x*3+2+y*infoHeader.biWidth*3] << 16;
        col++;
    }



    munmap(mmap_fd , 800*480*4);

    close(bmp_fd);
    close(lcd_fd);    
}

int touch_y(int* y)
{
    int touch_fd = open("/dev/input/event0",O_RDWR);
	if(touch_fd == -1)
	{
		printf("open touch error!\n");
		return -1;
	}
	int x;
	//2、读取触摸屏的返回数据
	//先声明系统触摸屏结构体
	struct input_event ts;
	
	while(1)
	{
		read(touch_fd ,&ts , sizeof(struct input_event));
		
		if(ts.type == EV_ABS)//先判断它是声明事件
		{
			if(ts.code == ABS_X)
			{
				
				x = ts.value;
			}
			if(ts.code == ABS_Y)
			{
				
				*y = ts.value;
			}
				
		}
		
		if(ts.type == EV_KEY)//判断压力值
		{
			if(ts.code == BTN_TOUCH)
			{
				//  按压  1   松开  0
				printf("F = %d\n",ts.value);
				if(ts.value == 0)
				{
					printf("x2 = %d\n",x);
					printf("y2 = %d\n",*y);
					printf("----------------\n");
                    return 0;
				}
                else
                {
					printf("x1 = %d\n",x);
					printf("y1 = %d\n",*y);
					printf("----------------\n");                    
                    return 1;
                }
			}			
		}
	}	
	//进程	
	//3、关闭触摸屏
	close(touch_fd);
}

int getVec()
{
    int x1,x2;
    while(!touch_y(&x1));
    while(touch_y(&x2));
    if(x1-x2>200){return 1;}
    else if(x1-x2<-200){return 0;}
    else {return 2;}
}


int bmp_small(char *path,int xx ,int yy)
{
    FILE* fp;    
    fp = fopen(path, "rb");//读取同目录下的image.bmp文件。
    if(fp == NULL)
    {
        printf("打开'image.bmp'失败！\n");
        return -1;
    }
    //如果不先读取bifType，根据C语言结构体Sizeof运算规则——整体大于部分之和，从而导致读文件错位
    unsigned short  fileType;   
    fread(&fileType,1,sizeof (unsigned short), fp);  
    if (fileType = 0x4d42)   
    {   
        //printf("文件类型标识正确!" );  
        //printf("\n文件标识符：%d\n", fileType); 
        fread(&fileHeader, 1, sizeof(BITMAPFILEHEADER1), fp);
        showBmpHead(fileHeader);
        fread(&infoHeader, 1, sizeof(BITMAPINFOHEADER1), fp);
        showBmpInfoHead(infoHeader);
        fclose(fp);        
    }

    int bmp_fd = open(path, O_RDWR);
	if(bmp_fd == -1)
	{
		printf("3.failed\n");
		return -1;
	}

    int lcd_fd = open("/dev/fb0", O_RDWR);
	if(lcd_fd == -1)
	{
		printf("lcd.failed\n");
		return -1;
	}

    //read data of bmp
	lseek(bmp_fd ,54 ,SEEK_SET);
    lseek(lcd_fd , 0 ,SEEK_SET);

    char bmp_buf[infoHeader.biWidth*infoHeader.biHeight*3];
    bzero(bmp_buf , sizeof(bmp_buf));
    read(bmp_fd ,bmp_buf ,sizeof(bmp_buf));

    int *mmap_fd =(int *)mmap(NULL,800*480*4 ,PROT_READ | PROT_WRITE,MAP_SHARED,lcd_fd,0);

    int x,y;
    for(x=0;x<500;x+=2)
    {
        for(y=0;y<300;y+=2)
        {
            *(mmap_fd+(x/2)+xx+(150-1-(y/2)+yy)*800) =(bmp_buf[x*3+y*infoHeader.biWidth*3]+bmp_buf[(x+1)*3+y*infoHeader.biWidth*3]+bmp_buf[x*3+(y+1)*infoHeader.biWidth*3]+bmp_buf[(x+1)*3+(y+1)*infoHeader.biWidth*3])/4 
                                                            | (bmp_buf[x*3+1+y*infoHeader.biWidth*3]+bmp_buf[(x+1)*3+1+y*infoHeader.biWidth*3]+bmp_buf[x*3+1+(y+1)*infoHeader.biWidth*3]+bmp_buf[(x+1)*3+1+(y+1)*infoHeader.biWidth*3])/4 << 8 
                                                            | (bmp_buf[x*3+2+y*infoHeader.biWidth*3]+bmp_buf[(x+1)*3+2+y*infoHeader.biWidth*3]+bmp_buf[x*3+2+(y+1)*infoHeader.biWidth*3]+bmp_buf[(x+1)*3+2+(y+1)*infoHeader.biWidth*3])/4 << 16;
        }
    }

    munmap(mmap_fd , 800*480*4);

    close(bmp_fd);
    close(lcd_fd);    
}

int album()
{   
    int vector;
    int slide=1;
    touch_flag=0;	
	pthread_t thread;
    pthread_create(&thread , NULL, pic, NULL );
	while(1)
	{   
        vector = getVec();
        if(vector==1)
            {slide=slide + 1;if(slide==4){slide=1;}
            printf("vector=1\n");
		    switch(slide)
		        {
			    case 1:clearlcd(); bmp_small("3.bmp",550,0);bmp_small("1.bmp",550,160);bmp_small("2.bmp",550,320);show_bmp1("1.bmp",25,90);  break;
			    case 2:clearlcd(); bmp_small("1.bmp",550,0);bmp_small("2.bmp",550,160);bmp_small("3.bmp",550,320);show_bmp2("2.bmp",25,90);  break;
			    case 3:clearlcd(); bmp_small("2.bmp",550,0);bmp_small("3.bmp",550,160);bmp_small("1.bmp",550,320);show_bmp3("3.bmp",25,90);  break;
		        }   
                if(touch_flag){break;}   
            }

        else if (vector==0)
            {slide=slide - 1;if(slide==0){slide=3;}
            printf("vector=0\n");                
		    switch(slide)
		        {
			    case 1:clearlcd(); bmp_small("3.bmp",550,0);bmp_small("1.bmp",550,160);bmp_small("2.bmp",550,320);show_bmp3("1.bmp",25,90); break;
			    case 2:clearlcd(); bmp_small("1.bmp",550,0);bmp_small("2.bmp",550,160);bmp_small("3.bmp",550,320);show_bmp2("2.bmp",25,90); break;
			    case 3:clearlcd(); bmp_small("2.bmp",550,0);bmp_small("3.bmp",550,160);bmp_small("1.bmp",550,320);show_bmp1("3.bmp",25,90); break;
		        }
                if(touch_flag){break;}           
            }
	}
	return 0;
}