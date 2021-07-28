#include "read.h"
#include "libxml/xmlmemory.h"
#include "libxml/parser.h"

xmlChar *__get_cmd_id(xmlDocPtr doc, xmlNodePtr cur)
{
	xmlChar *key, *id;
	cur = cur->xmlChildrenNode;
	while (cur != NULL)
	{
	    if ((!xmlStrcmp(cur->name, (const xmlChar *)"cmd")))
	    {
			//获取字符串
		    key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
		    printf("cmd: %s\n", key);
		    xmlFree(key);

			//读取节点属性
		    id = xmlGetProp(cur, (const xmlChar *)"id");
		    printf("id: %s\n", id);

		    xmlFree(doc);
		    return id;
 	    }
		cur = cur->next;
	}
	//释放文档指针
	xmlFree(doc);
    return NULL;
}

xmlChar *parse_xml(char *xmlfile)
{
	xmlDocPtr doc;
	xmlNodePtr cur1, cur2;

	//分析一个xml文件，并返回一个xml文档的对象指针
	doc = xmlParseFile(xmlfile);
	if (doc == NULL)
	{
		fprintf(stderr,"Document not parsed successfully. \n");
		return NULL;
	}
	
	//获得文档的根节点
	cur1 = xmlDocGetRootElement(doc);
	if(cur1 == NULL)
	{
		fprintf(stderr,"empty document\n");
		xmlFreeDoc(doc);
		return NULL;
	}

	if(xmlStrcmp(cur1->name, (const xmlChar *)"nlp"))
	{
		fprintf(stderr,"document of the wrong type, root node != nlp");
		xmlFreeDoc(doc);
		return NULL;
	}
	
	cur1 = cur1->xmlChildrenNode;

	while (cur1 != NULL)
	{
		if ((!xmlStrcmp(cur1->name, (const xmlChar *)"result")))
		{
			cur2 = cur1->xmlChildrenNode;
			while(cur2 != NULL)
			{
				if((!xmlStrcmp(cur2->name, (const xmlChar *)"confidence")))
				{
					xmlChar *key = xmlNodeListGetString(doc, cur2->xmlChildrenNode, 1);
					if(atoi((char *)key) < 30)
					{
						xmlFree(doc);
						fprintf(stderr, "sorry, I'm NOT sure what you say.\n");
						return NULL;
					}
				}
				
				if((!xmlStrcmp(cur2->name, (const xmlChar *)"object")))
				{
					return __get_cmd_id(doc, cur2);
				}
				cur2 = cur2->next;
			}
		}
		cur1 = cur1->next;
	}

	//释放文档指针
	xmlFreeDoc(doc);
	return NULL;
}

int  tcp_send_pcm(int socket_fd,const char *pcm_file)
{
	int fd;
	
	//以只读方式打开pcm文件
	fd = open(pcm_file,O_RDONLY);
	
	if(fd < 0)
	{
		printf("open %s\n",pcm_file);
		
		return -1;
		
	}
	
	// 取得PCM数据的大小
	off_t pcm_size = lseek(fd, 0, SEEK_END);
	
	// 从新定位到文件的头部
	lseek(fd, 0, SEEK_SET);
	
	// 分配1个pcm_size字节大小的缓冲区
	char *pcm_buf = calloc(1, pcm_size);
	
	// 读取PCM文件数据
	read(fd, pcm_buf, pcm_size);

	// 将PCM文件发送给语音识别引擎系统
	int m = send(socket_fd, pcm_buf, pcm_size,0);
	
	printf("%d bytes has been sent\n", m);

	free(pcm_buf);	
	
	return 0;
}


int tcp_recv_xml(int socket_fd)
{
	
	//calloc动态分配完内存后，自动初始化该内存空间为零
	//分配1个1024字节大小的缓冲区
	char *xml_buf = calloc(1, 1024);

	// 从ubuntu接收XML结果
	int n = recv(socket_fd, xml_buf, 1024,0);

	if(n <= 0)
	{
		perror("tcp recv:");
		
		return -1;
	}
	
	printf("%d bytes has been recv\n", n);
	
	//创建result.xml文件并清空
	int fd = open("result.xml",O_CREAT|O_RDWR|O_TRUNC);
	
	if(fd < 0)
	{
		printf("create result.xml fail\n");
		
		return -1;
		
	}
	
	//将接收到XML写入到result.xml文件
	n = write(fd,xml_buf,n);
	
	printf("%d bytes has been write to result.xml\n", n);

	//关闭result.xml文件
	close(fd);
	
	return 0;
}

//屏幕分辨率为800x480，像素点的总数=800x480
//每个32位的像素点占用内存空间4个字节
char color_buf[800*480*4]={0};
char bmp_buf[800*480*3]={0};


int lcd_draw_bmp_file(const char *pbmp_path)
{
	int lcd_fd;
	int bmp_fd;
	int i=0,j=0;

	//打开LCD设备，并以可读可写方式访问
	lcd_fd = open("/dev/fb0",O_RDWR);
	
	if(lcd_fd < 0)
	{
		printf("open lcd fail\n");
		return -1;		
		
	}
	
	//打开bmp图片，并只读方式访问
	bmp_fd = open(pbmp_path,O_RDONLY);
	
	if(bmp_fd < 0)
	{
		printf("open %s fail\n",pbmp_path);
		return -1;		
		
	}	
	
	//bmp文件定位到rgb颜色区域的起始位置
	lseek(bmp_fd,54,SEEK_SET);
	
	//将bmp所有的rgb数据全部读取
	read(bmp_fd,bmp_buf,800*480*3);
	
	//由于不再需要操作1.bmp
	close(bmp_fd);
	
	
	
	//将显示位置定位到坐标原点
	lseek(lcd_fd,0,SEEK_SET);
	
	
	//将bmp的rgb数据给我color_buf
	for(i=0,j=0; i<800*480*4; i+=4,j+=3)
	{
		color_buf[i]  =bmp_buf[j];		//b
		color_buf[i+1]=bmp_buf[j+1];	//g		
		color_buf[i+2]=bmp_buf[j+2];	//r		
		color_buf[i+3]=0;				//a			
	}
	
	//向LCD设备写入数据
	for(i=479; i>=0; i--)
		write(lcd_fd,&color_buf[800*i*4],800*4);		

	for(i=479; i>=0; i--)
		write(lcd_fd,&color_buf[800*i*4],800*4);		
	//关闭LCD设备
	close(lcd_fd);	
}


int audio ()
{
	int socket_fd;
	
	int rt;
	int len;
/*
	struct timeval {
		time_t tv_sec; 	// 秒 
		long tv_usec; 	// 微秒
	};
*/
	//3秒超时
	struct timeval timeout={3,0};
	
	//创建套接字，协议为IPv4，类型为TCP
	socket_fd = socket(AF_INET,SOCK_STREAM,0);
	
	if(socket_fd<0)
	{
		perror("create socket fail:");
		
		return -1;
	}
	
	struct sockaddr_in	 dest_addr;
	
	dest_addr.sin_family 		= AF_INET;					//IPv4
	dest_addr.sin_port   		= htons(54321);				//目的端口为54321
	dest_addr.sin_addr.s_addr	= inet_addr("192.168.1.222");	//目的IP地址填写

	//设置发送超时
	rt = setsockopt(socket_fd,SOL_SOCKET,SO_SNDTIMEO,(const char *)&timeout,sizeof timeout);
	
	if(rt < 0)
	{
		perror("setsockopt SO_RCVTIMEO:");
		return -1;
	}
	
	//设置接收超时
	rt = setsockopt(socket_fd,SOL_SOCKET,SO_RCVTIMEO,(const char *)&timeout,sizeof timeout);
	
	if(rt < 0)
	{
		perror("setsockopt SO_RCVTIMEO:");
		return -1;
	}
	
	//要跟服务器建立连接（等同于登陆游戏）
	rt=connect(socket_fd,(struct sockaddr *)&dest_addr,sizeof dest_addr);
	
	if(rt < 0)
	{
		perror("connect fail:");
		return -1;
	}
	
	printf("connect server success\n");
	
	while(1)
	{
		printf("please press enter to start record in 3s...\n");
		
		//等待回车键
		getchar();

		//启动录音
		system("arecord -d3 -c1 -r16000 -traw -fS16_LE cmd.pcm");	
		
		//向语音识别引擎系统发送cmd.pcm
		rt = tcp_send_pcm(socket_fd,"cmd.pcm");
		if(rt < 0)
			continue;
		
		//从语音识别引擎系统接收XML结果，并将结果保存到result.xml	
		rt = tcp_recv_xml(socket_fd);	
		
		if(rt < 0)
			continue;
		
		//分析result.xml
		xmlChar *id = parse_xml("result.xml");
		
		if(id)
		{
			
			//打印id号
			printf("id=%s\n",id);
			
			switch(atoi(id))
			{
				case(6):but1 = 1;audio_int = 1;break;
				case(7):but1 = 1;audio_int = 1;break;
			}
			//根据id号响应不同的操作
			if(atoi(id) == 1)
			{
				//显示开灯图片
				printf("开灯!\n");
				//lcd_draw_bmp_file("open_light.bmp");
			}
			
			if(atoi(id) == 2)
			{
				//显示关灯图片
				printf("关灯!\n");
				//lcd_draw_bmp_file("close_light.bmp");		
			}			
		}
		
		system("rm cmd.pcm");	
		system("rm result.xml");	
		
	}


	//关闭套接字
	close(socket_fd);
	
	return 0;
}