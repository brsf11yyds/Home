#include <stdio.h>
#include <stdio.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include "read.h"
#include <linux/input.h>

BITMAPFILEHEADER fileHeader;
BITMAPINFOHEADER infoHeader;

void showBmpHead(BITMAPFILEHEADER pBmpHead) 
{  //定义显示信息的函数，传入文件头结构体
    //printf("BMP文件大小：%dkb\n", fileHeader.bfSize/1024);
    //printf("保留字必须为0：%d\n",  fileHeader.bfReserved1);
    //printf("保留字必须为0：%d\n",  fileHeader.bfReserved2);
    //printf("实际位图数据的偏移字节数: %d\n",  fileHeader.bfOffBits);
}

void showBmpInfoHead(BITMAPINFOHEADER pBmpinfoHead) 
{//定义显示信息的函数，传入的是信息头结构体
   //printf("位图信息头:\n" );   
   //printf("信息头的大小:%d\n" ,infoHeader.biSize);   
   //printf("位图宽度:%d\n" ,infoHeader.biWidth);   
   //printf("位图高度:%d\n" ,infoHeader.biHeight);   
   //printf("图像的位面数(位面数是调色板的数量,默认为1个调色板):%d\n" ,infoHeader.biPlanes);   
   //printf("每个像素的位数:%d\n" ,infoHeader.biBitCount);   
   //printf("压缩方式:%d\n" ,infoHeader.biCompression);   
   //printf("图像的大小:%d\n" ,infoHeader.biSizeImage);   
   //printf("水平方向分辨率:%d\n" ,infoHeader.biXPelsPerMeter);   
   //printf("垂直方向分辨率:%d\n" ,infoHeader.biYPelsPerMeter);   
   //printf("使用的颜色数:%d\n" ,infoHeader.biClrUsed);   
   //printf("重要颜色数:%d\n" ,infoHeader.biClrImportant);

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
        fread(&fileHeader, 1, sizeof(BITMAPFILEHEADER), fp);
        showBmpHead(fileHeader);
        fread(&infoHeader, 1, sizeof(BITMAPINFOHEADER), fp);
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
        fread(&fileHeader, 1, sizeof(BITMAPFILEHEADER), fp);
        showBmpHead(fileHeader);
        fread(&infoHeader, 1, sizeof(BITMAPINFOHEADER), fp);
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
        fread(&fileHeader, 1, sizeof(BITMAPFILEHEADER), fp);
        showBmpHead(fileHeader);
        fread(&infoHeader, 1, sizeof(BITMAPINFOHEADER), fp);
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

int touch(int* y)
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
    while(!touch(&x1));
    while(touch(&x2));
    if(x1-x2>200){return 1;}
    else if(x1-x2<-200){return 0;}
    else {return 2;}
}

int clearlcd()
{
    int lcd_fd = open("/dev/fb0", O_RDWR);
	if(lcd_fd == -1)
	{
		printf("lcd.failed\n");
		return -1;
	}
    //read data of bmp
    lseek(lcd_fd , 0 ,SEEK_SET);
    int *mmap_fd =(int *)mmap(NULL,800*480*4 ,PROT_READ | PROT_WRITE,MAP_SHARED,lcd_fd,0);
    int x;
    for(x=0;x<800*480;x++)
    {
        *(mmap_fd+x) =0x00ffffff; 
    }
    munmap(mmap_fd , 800*480*4);
    close(lcd_fd);    
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
        fread(&fileHeader, 1, sizeof(BITMAPFILEHEADER), fp);
        showBmpHead(fileHeader);
        fread(&infoHeader, 1, sizeof(BITMAPINFOHEADER), fp);
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

int main(int argc ,char * argv[])
{   
    int vector;
    int slide=1;
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
            }
	}
	return 0;
}