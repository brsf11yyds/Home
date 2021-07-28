#include "read.h"
#include <string.h>
#include <stdio.h>
#include <malloc.h>
struct list_node
{
	char id[10];   
	char pwd[10];
	struct list_node *next;	
};
struct list_node * data0;
int list_read(struct list_node * head )
{
	FILE *fp;
 
   	fp = fopen( "data.txt" , "r+" );
   	if(feof(fp) != 1)
	{
		struct list_node * TSN;
		struct list_node * TSO;
		TSO = head;
		while(!feof(fp))
		{
			TSN = (struct list_node*)malloc(sizeof(struct list_node));
			fscanf(fp,"%s",TSN->id);
			fscanf(fp,"%s",TSN->pwd);
			TSO->next = TSN;
			TSO = TSN;
		}
		TSN->next = NULL;
		return 0;
		
	}
	else return -1;
   	fclose(fp);
}

int list_add_head(struct list_node * head ,char * id,char * pwd)
{
	struct list_node * p = NULL;
	p = (struct list_node*)malloc(sizeof(struct list_node));
	if(p == NULL)
	{
		printf("申请空间失败！\n");
		return -1;
	}
	
	strcpy(p->id,id);
	strcpy(p->pwd,pwd);
	head->next = p;
	
	return 0;	
}


//查找节点里面有没有这个数据
int find_list_node(struct list_node * head , char *id)
{
	int ok=0;
	if(head->next == NULL)
	{
		printf("没有节点!\n");
		return -1;
	}
	
	//创建一个替身
	struct list_node * TS = NULL;
	for(TS = head->next ; TS != NULL ; TS = TS->next)
	{
		if(strcmp(TS->id,id) == 0)
		{
			printf("找到 ID  = %s\n",TS->id);
			//printf("密码 = %s\n",TS->pwd);
			ok=1;
			break;
		}
		
	}
	if(ok==0) 
	{
		return 0;
	}
	else return 1;
}

//查找节点密码
int find_pwd_list_node(struct list_node * head , char *id,char *pwd)
{
	int ok=0;
	if(head->next == NULL)
	{
		printf("没有节点!\n");
		return -1;
	}
	
	//创建一个替身
	struct list_node * TS = NULL;
	for(TS = head->next ; TS != NULL ; TS = TS->next)
	{
		if(strcmp(TS->id,id) == 0)
		{
			if(strcmp(TS->pwd,pwd) == 0)
			{
				ok = 1;
			}
			break;
		}
		
	}
	if(ok==0) 
	{
		printf("密码错误！\n");
		return 0;
	}
	else return 1;
}





int data()
{
    data0 = (struct list_node*)malloc(sizeof(struct list_node));
   
	data0->next = NULL;	
	list_read(data0);
	int menu;
	char id[10];
	int find;
	int find2;
	char pwd[10];

	
	while(1)
	{
		printf("欢迎使用智能家居管理系统\n请登录\n");
		
		printf("请输入用户id\n");
		scanf("%s",id);
		find = find_list_node(data0,id);
		if(find == 1)
		{
			printf("请输入密码\n");
			scanf("%s",pwd);
			find2 = find_pwd_list_node(data0,id,pwd);
			if(find2 == 1)
			{
				printf("登录成功！");
				break;
			}
			else continue;
		}
		else
		{
			printf("无效id!\n");
			continue;
		}

		
	}
	return 0;
}



