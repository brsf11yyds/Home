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
			//��ȡ�ַ���
		    key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
		    printf("cmd: %s\n", key);
		    xmlFree(key);

			//��ȡ�ڵ�����
		    id = xmlGetProp(cur, (const xmlChar *)"id");
		    printf("id: %s\n", id);

		    xmlFree(doc);
		    return id;
 	    }
		cur = cur->next;
	}
	//�ͷ��ĵ�ָ��
	xmlFree(doc);
    return NULL;
}

xmlChar *parse_xml(char *xmlfile)
{
	xmlDocPtr doc;
	xmlNodePtr cur1, cur2;

	//����һ��xml�ļ���������һ��xml�ĵ��Ķ���ָ��
	doc = xmlParseFile(xmlfile);
	if (doc == NULL)
	{
		fprintf(stderr,"Document not parsed successfully. \n");
		return NULL;
	}
	
	//����ĵ��ĸ��ڵ�
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

	//�ͷ��ĵ�ָ��
	xmlFreeDoc(doc);
	return NULL;
}

int  tcp_send_pcm(int socket_fd,const char *pcm_file)
{
	int fd;
	
	//��ֻ����ʽ��pcm�ļ�
	fd = open(pcm_file,O_RDONLY);
	
	if(fd < 0)
	{
		printf("open %s\n",pcm_file);
		
		return -1;
		
	}
	
	// ȡ��PCM���ݵĴ�С
	off_t pcm_size = lseek(fd, 0, SEEK_END);
	
	// ���¶�λ���ļ���ͷ��
	lseek(fd, 0, SEEK_SET);
	
	// ����1��pcm_size�ֽڴ�С�Ļ�����
	char *pcm_buf = calloc(1, pcm_size);
	
	// ��ȡPCM�ļ�����
	read(fd, pcm_buf, pcm_size);

	// ��PCM�ļ����͸�����ʶ������ϵͳ
	int m = send(socket_fd, pcm_buf, pcm_size,0);
	
	printf("%d bytes has been sent\n", m);

	free(pcm_buf);	
	
	return 0;
}


int tcp_recv_xml(int socket_fd)
{
	
	//calloc��̬�������ڴ���Զ���ʼ�����ڴ�ռ�Ϊ��
	//����1��1024�ֽڴ�С�Ļ�����
	char *xml_buf = calloc(1, 1024);

	// ��ubuntu����XML���
	int n = recv(socket_fd, xml_buf, 1024,0);

	if(n <= 0)
	{
		perror("tcp recv:");
		
		return -1;
	}
	
	printf("%d bytes has been recv\n", n);
	
	//����result.xml�ļ������
	int fd = open("result.xml",O_CREAT|O_RDWR|O_TRUNC);
	
	if(fd < 0)
	{
		printf("create result.xml fail\n");
		
		return -1;
		
	}
	
	//�����յ�XMLд�뵽result.xml�ļ�
	n = write(fd,xml_buf,n);
	
	printf("%d bytes has been write to result.xml\n", n);

	//�ر�result.xml�ļ�
	close(fd);
	
	return 0;
}

//��Ļ�ֱ���Ϊ800x480�����ص������=800x480
//ÿ��32λ�����ص�ռ���ڴ�ռ�4���ֽ�
char color_buf[800*480*4]={0};
char bmp_buff[800*480*3]={0};


int lcd_draw_bmp_file(const char *pbmp_path)
{
	int lcd_fd;
	int bmp_fd;
	int i=0,j=0;

	//��LCD�豸�����Կɶ���д��ʽ����
	lcd_fd = open("/dev/fb0",O_RDWR);
	
	if(lcd_fd < 0)
	{
		printf("open lcd fail\n");
		return -1;		
		
	}
	
	//��bmpͼƬ����ֻ����ʽ����
	bmp_fd = open(pbmp_path,O_RDONLY);
	
	if(bmp_fd < 0)
	{
		printf("open %s fail\n",pbmp_path);
		return -1;		
		
	}	
	
	//bmp�ļ���λ��rgb��ɫ�������ʼλ��
	lseek(bmp_fd,54,SEEK_SET);
	
	//��bmp���е�rgb����ȫ����ȡ
	read(bmp_fd,bmp_buff,800*480*3);
	
	//���ڲ�����Ҫ����1.bmp
	close(bmp_fd);
	
	
	
	//����ʾλ�ö�λ������ԭ��
	lseek(lcd_fd,0,SEEK_SET);
	
	
	//��bmp��rgb���ݸ���color_buf
	for(i=0,j=0; i<800*480*4; i+=4,j+=3)
	{
		color_buf[i]  =bmp_buff[j];		//b
		color_buf[i+1]=bmp_buff[j+1];	//g		
		color_buf[i+2]=bmp_buff[j+2];	//r		
		color_buf[i+3]=0;				//a			
	}
	
	//��LCD�豸д������
	for(i=479; i>=0; i--)
		write(lcd_fd,&color_buf[800*i*4],800*4);		

	for(i=479; i>=0; i--)
		write(lcd_fd,&color_buf[800*i*4],800*4);		
	//�ر�LCD�豸
	close(lcd_fd);	
}


int audio ()
{
	int socket_fd;
	
	int rt;
	int len;
/*
	struct timeval {
		time_t tv_sec; 	// �� 
		long tv_usec; 	// ΢��
	};
*/
	//3�볬ʱ
	struct timeval timeout={3,0};
	
	//�����׽��֣�Э��ΪIPv4������ΪTCP
	socket_fd = socket(AF_INET,SOCK_STREAM,0);
	
	if(socket_fd<0)
	{
		perror("create socket fail:");
		
		return -1;
	}
	
	struct sockaddr_in	 dest_addr;
	
	dest_addr.sin_family 		= AF_INET;					//IPv4
	dest_addr.sin_port   		= htons(54321);				//Ŀ�Ķ˿�Ϊ54321
	dest_addr.sin_addr.s_addr	= inet_addr("192.168.1.77");	//Ŀ��IP��ַ��д

	//���÷��ͳ�ʱ
	rt = setsockopt(socket_fd,SOL_SOCKET,SO_SNDTIMEO,(const char *)&timeout,sizeof timeout);
	
	if(rt < 0)
	{
		perror("setsockopt SO_RCVTIMEO:");
		return -1;
	}
	
	//���ý��ճ�ʱ
	rt = setsockopt(socket_fd,SOL_SOCKET,SO_RCVTIMEO,(const char *)&timeout,sizeof timeout);
	
	if(rt < 0)
	{
		perror("setsockopt SO_RCVTIMEO:");
		return -1;
	}
	
	//Ҫ���������������ӣ���ͬ�ڵ�½��Ϸ��
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
		
		//�ȴ��س���
		getchar();

		//����¼��
		system("arecord -d3 -c1 -r16000 -traw -fS16_LE cmd.pcm");	
		
		//������ʶ������ϵͳ����cmd.pcm
		rt = tcp_send_pcm(socket_fd,"cmd.pcm");
		if(rt < 0)
			continue;
		
		//������ʶ������ϵͳ����XML���������������浽result.xml	
		rt = tcp_recv_xml(socket_fd);	
		
		if(rt < 0)
			continue;
		
		//����result.xml
		xmlChar *id = parse_xml("result.xml");
		
		if(id)
		{
			
			//��ӡid��
			printf("id=%s\n",id);
			
			switch(atoi(id))
			{	
				case(1):led_but1 = 1;led_but3=1;audio_int = 1;led_but5=1;led_but7=1;audio_int = 1;break;
				case(2):led_but2 = 1;led_but4=1;audio_int = 1;led_but6=1;led_but8=1;audio_int = 1;break;
				case(6):but1 = 1;audio_int = 1;break;
				case(7):but1 = 1;audio_int = 1;break;
				case(8):but2 = 1;audio_int = 1;break;
				case(9):but3 = 1;audio_int = 1;break;
				case(10):but4 = 1;audio_int = 1;break;
				case(11):but5 = 1;audio_int = 1;break;
				case(12):led_but9 = 1;audio_int = 1;break;
				case(999):but6 = 1;led_but10=1;audio_int = 1;break;

			}
			//����id����Ӧ��ͬ�Ĳ���
			if(atoi(id) == 1)
			{
				//��ʾ����ͼƬ
				printf("����!\n");
				//lcd_draw_bmp_file("open_light.bmp");
			}
			
			if(atoi(id) == 2)
			{
				//��ʾ�ص�ͼƬ
				printf("�ص�!\n");
				//lcd_draw_bmp_file("close_light.bmp");		
			}			
		}
		
		system("rm cmd.pcm");	
		system("rm result.xml");	
		
	}


	//�ر��׽���
	close(socket_fd);
	
	return 0;
}