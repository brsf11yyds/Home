﻿/*
* 语音听写(iFly Auto Transform)技术能够实时地将语音转换成对应的文字。
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../../lib/inc/qisr.h"
#include "../../lib/inc/msp_cmn.h"
#include "../../lib/inc/msp_errors.h"
#include "speech_recognizer.h"

#include <errno.h>

#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#define DEF_PORT  "54321"

#define FRAME_LEN	640 
#define	BUFFER_SIZE	4096
#define SAMPLE_RATE_16K     (16000)
#define SAMPLE_RATE_8K      (8000)
#define MAX_GRAMMARID_LEN   (32)
#define MAX_PARAMS_LEN      (1024)

const char * ASR_RES_PATH   = "fo|res/asr/common.jet"; //离线语法识别资源路径
const char * GRM_BUILD_PATH = "res/asr/GrmBuilld"; //构建离线语法识别网络生成数据保存路径
const char * GRM_FILE       = "cmd.bnf"; //构建离线识别语法网络所用的语法文件
const char * LEX_NAME       = "contact"; //更新离线识别语法的contact槽（语法文件为此示例中使用的call.bnf）

typedef struct _UserData
{
	int     build_fini; //标识语法构建是否完成
	int     update_fini;//标识更新词典是否完成
	int     errcode;    //记录语法构建或更新词典回调错误码
	char    grammar_id[MAX_GRAMMARID_LEN]; //保存语法构建返回的语法ID
}UserData;


const char *get_audio_file(void); //选择进行离线语法识别的语音文件
int build_grammar(UserData *udata); //构建离线识别语法网络
int update_lexicon(UserData *udata); //更新离线识别语法词典
int run_asr(UserData *udata); //进行离线语法识别
void *recv_accept(void *arg);


char *gec210_ip;

int sockfd_asr = -1;
int sockfd =-1;

void init_sock(void)
{
	// recv socket
	struct sockaddr_in sin;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	int on = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(1));

	bzero((char *) &sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);	
	sin.sin_port = htons(atoi(DEF_PORT));

	int ret = bind(sockfd, (struct sockaddr *)&sin, sizeof(struct sockaddr));
	if(ret == -1)
	{
		perror("bind() failed");
		exit(0);
	}

	listen(sockfd, 3);

	printf("11111111111111111111 ... \n");
	socklen_t addrlen = sizeof(struct sockaddr);
	sockfd_asr = accept(sockfd, (struct sockaddr *)&sin, &addrlen);
	printf("22222222222222222222 ... \n");
/* 	while(1)
	{		
		
		if(sockfd_asr < 0)
		{
			if(errno == EINTR)
				continue;
			perror("accpet()");
			exit(1);
		}
		else
			break;
	}
	*/
	
	printf("链接的地址和端口号: %s: %hu \n", inet_ntoa(sin.sin_addr),ntohs(sin.sin_port));
}


const char *get_audio_file(void)
{
	char *pcm = calloc(1, 96000);
	
	printf("waiting for data...\n");

	gec210_ip = calloc(1, 32);

	//音频文件大小:96KB
	int n, total_bytes = 96000, m=0;
	char *tmp = pcm;

	//通过网络接收96KB字节
	while(total_bytes > 0)
	{
		n = read(sockfd_asr, tmp, total_bytes);

		if(n == -1)
		{
			perror("read() failed");
			exit(0);
		}
		if(n == 0)
		{
			printf("client disconnect\n");
			exit(0);
		}

		total_bytes -= n;
		tmp += n;
		m += n;
	}

	printf("[%d]recv finished. get %d bytes\n", __LINE__, m);

	//将接收到的音频数据，写入到./pcm/cmd.pcm文件
	int pcmfd = open("./pcm/cmd.pcm", O_WRONLY|O_CREAT|O_TRUNC, 0644);
	m = write(pcmfd, pcm, 96000);
	printf("%d bytes has been wirtten into cmd.pcm\n", m);
	close(pcmfd);
	
	//free(pcm); 
	//返回写入的文件路径
	return "pcm/cmd.pcm";

}

int build_grm_cb(int ecode, const char *info, void *udata)
{
	UserData *grm_data = (UserData *)udata;

	if (NULL != grm_data) {
		grm_data->build_fini = 1;
		grm_data->errcode = ecode;
	}

	if (MSP_SUCCESS == ecode && NULL != info) {
		printf("构建语法成功！ 语法ID:%s\n", info);
		if (NULL != grm_data)
			snprintf(grm_data->grammar_id, MAX_GRAMMARID_LEN - 1, "%s", info);
	}
	else
		printf("构建语法失败！%d\n", ecode);

	return 0;
}

int build_grammar(UserData *udata)
{
	FILE *grm_file           = NULL;
	char *grm_content        = NULL;
	unsigned int grm_cnt_len = 0;
	int ret                  = 0;
	char *grm_build_params   = calloc(1, MAX_PARAMS_LEN);

	grm_file = fopen(GRM_FILE, "rb");	
	if(NULL == grm_file) {
		printf("打开\"%s\"文件失败！[%s]\n", GRM_FILE, strerror(errno));
		return -1; 
	}

	fseek(grm_file, 0, SEEK_END);
	grm_cnt_len = ftell(grm_file);
	fseek(grm_file, 0, SEEK_SET);

	grm_content = (char *)malloc(grm_cnt_len + 1);
	if (NULL == grm_content)
	{
		printf("内存分配失败!\n");
		fclose(grm_file);
		grm_file = NULL;
		return -1;
	}
	fread((void*)grm_content, 1, grm_cnt_len, grm_file);
	grm_content[grm_cnt_len] = '\0';
	fclose(grm_file);
	grm_file = NULL;

	snprintf(grm_build_params, MAX_PARAMS_LEN - 1, 
		"engine_type = local, \
		asr_res_path = %s, sample_rate = %d, \
		grm_build_path = %s, ",
		ASR_RES_PATH,
		SAMPLE_RATE_16K,
		GRM_BUILD_PATH
		);
	ret = QISRBuildGrammar("bnf", grm_content, grm_cnt_len, grm_build_params, build_grm_cb, udata);

	free(grm_content);
	free(grm_build_params);
	grm_content = NULL;

	return ret;
}


static void show_result(char *string, char is_over)
{
	printf("\rResult:\n%s", string);
	if(is_over)
		putchar('\n');

	static int first_time = 1;
	if(first_time == 1)
	{
		if(!access("result.xml", F_OK))	
			system("rm result.xml");
		first_time = 0;
	}

	FILE *result;
	result = fopen("result.xml", "a");
	fwrite(string, strlen(string), 1, result);
	fclose(result);
	
	int count = write(sockfd_asr, string, strlen(string));
	if(count == -1)
	{
		perror("write socket failed");
		exit(0);
	}
	printf("%d bytes has been sent to socket.\n", count);
}

static char *g_result = NULL;
static unsigned int g_buffersize = BUFFER_SIZE;

void on_result(const char *result, char is_last)
{
	if(result)
	{
		size_t left = g_buffersize - 1 - strlen(g_result);
		size_t size = strlen(result);

		if(left < size)
		{
			g_result = (char*)realloc(g_result, g_buffersize + BUFFER_SIZE);

			if(g_result)
				g_buffersize += BUFFER_SIZE;
			else
			{
				printf("mem alloc failed\n");
				return;
			}
		}
		strncat(g_result, result, size);
		show_result(g_result, is_last);
	}
}


void on_speech_begin()
{
	if(g_result)
	{
		free(g_result);
	}
	g_result = (char*)malloc(BUFFER_SIZE);
	g_buffersize = BUFFER_SIZE;
	memset(g_result, 0, g_buffersize);

	printf("Start Listening...\n");
}


void on_speech_end(int reason)
{
	if (reason == END_REASON_VAD_DETECT)
		printf("\nSpeaking done \n");
	else
		printf("\nRecognizer error %d\n", reason);
}

static void demo_file(const char* audio_file, const char* session_begin_params)
{
	int	errcode = 0;
	FILE*	f_pcm = NULL;
	char*	p_pcm = NULL;
	unsigned long pcm_count = 0;
	unsigned long pcm_size  = 0;
	unsigned long read_size = 0;

	struct speech_rec iat;
	struct speech_rec_notifier recnotifier =
	{
		on_result,				//该函数将语音识别后的结果文件result.xml，进行读取，通过网络发送给开发板
		on_speech_begin,
		on_speech_end
	};

	if (NULL == audio_file)
		goto iat_exit;

	//打开音频文件
	f_pcm = fopen(audio_file, "rb");
	if (NULL == f_pcm)
	{
		printf("\nopen [%s] failed: %s\n",
				audio_file, strerror(errno));
		goto iat_exit;
	}
	//偏移到音频文件头部
	fseek(f_pcm, 0, SEEK_END);

	//获取音频文件大小
	pcm_size = ftell(f_pcm);

	//重新偏移到音频文件头部
	fseek(f_pcm, 0, SEEK_SET);

	//申请内存空间
	p_pcm = (char *)malloc(pcm_size);
	if (NULL == p_pcm)
	{
		printf("\nout of memory! \n");
		goto iat_exit;
	}

	//读取1次，大小为pcm_size
	read_size = fread((void *)p_pcm, 1, pcm_size, f_pcm);
	if (read_size != pcm_size)
	{
		printf("\nread [%s] error!\n", audio_file);
		goto iat_exit;
	}

	//语音识别引擎初始化,并根据session_begin_params离线语法识别参数
	//SR_USER,表示语音来源于音频文件
	errcode = sr_init(&iat, session_begin_params, SR_USER, &recnotifier);
	if(errcode != 0)
	{
		printf("speech recognizer init failed : %d\n", errcode);
		goto iat_exit;
	}

	//语音识别引擎监听开始
	errcode = sr_start_listening(&iat);
	if(errcode != 0)
	{
		printf("\nsr_start_listening failed! error code:%d\n", errcode);
		goto iat_exit;
	}

	//将音频文件的数据写入到语音识别引擎，进行语音识别
	while(1)
	{
		unsigned int len = 10 * FRAME_LEN; /* 200ms audio */
		int ret = 0;

		if (pcm_size < 2 * len)
			len = pcm_size;
		if (len <= 0)
			break;
		//将音频文件的数据写入到语音识别引擎，进行语音识别，每次大小为6.4KB
		ret = sr_write_audio_data(&iat, &p_pcm[pcm_count], len);

		if(0 != ret)
		{
			printf("\nwrite audio data failed! error code:%d\n", ret);
			goto iat_exit;
		}

		pcm_count += (long)len;
		pcm_size  -= (long)len;		
	}

	//语音识别引擎监听停止
	errcode = sr_stop_listening(&iat);
	if(errcode)
	{
		printf("\nsr_stop_listening failed! error code:%d \n", errcode);
		goto iat_exit;
	}

iat_exit:
	//释放用到的资源
	if(NULL != f_pcm)
	{
		fclose(f_pcm);
		f_pcm = NULL;
	}
	if(NULL != p_pcm)
	{
		free(p_pcm);
		p_pcm = NULL;
	}

	sr_stop_listening(&iat);
	sr_uninit(&iat);
}

int run_asr(UserData *udata)
{
	//分配缓冲区大小为1024字节
	char *asr_params = calloc(1, MAX_PARAMS_LEN);
	const char *asr_audiof = NULL;

	//离线语法识别参数设置
	snprintf(asr_params, MAX_PARAMS_LEN - 1, 
			"engine_type = local, "
			"asr_res_path = %s, sample_rate = %d, "
			"grm_build_path = %s, local_grammar = %s, "
			"result_type = xml, result_encoding = UTF-8, ",
			ASR_RES_PATH,		//离线语法识别资源路径
			SAMPLE_RATE_16K,	//采样率16K
			GRM_BUILD_PATH,		//构建离线语法识别网络生成数据保存路径		
			udata->grammar_id	//保存语法构建返回的语法ID
			);

	//初始化网络，并等待网络TCP连接
	init_sock();

	while(1)
	{
		//通过TCP接收音频数据，并返回保存好音频文件的路径
		asr_audiof = get_audio_file();

		//分析音频文件内容
		demo_file(asr_audiof, asr_params); 			
		
		
	}


	//关闭客户端
	close(sockfd_asr);
	
	//关闭socket
	close(sockfd);


	free(asr_params); 
	
	return 0;
}

int main(int argc, char* argv[])
{
	system("rm msc -rf");
	system("cp .msc msc -rf");
	//system("date -s 12/15/2016");

	const char *login_config  = "appid = 583467f7"; //登录参数
	UserData asr_data;
	int ret = 0 ;

	ret = MSPLogin(NULL, NULL, login_config); //第一个参数为用户名，第二个参数为密码，传NULL即可，第三个参数是登录参数
	if (MSP_SUCCESS != ret)
	{
		printf("登录失败：%d\n", ret);
		//goto exit;
	}

	memset(&asr_data, 0, sizeof(UserData));
	// 第一次使用某语法进行识别，需要先构建语法网络，获取语法ID
	// 之后使用此语法进行识别，无需再次构建
		printf("构建离线识别语法网络...\n");
		if(build_grammar(&asr_data) != MSP_SUCCESS)
		{
			printf("构建语法调用失败！\n");
			goto exit;
		}

		while (1 != asr_data.build_fini)
			usleep(200 * 1000);
		
		if (MSP_SUCCESS != asr_data.errcode)
			goto exit;
	
		printf("离线识别语法网络构建完成，开始识别...\n");	

		if((ret=run_asr(&asr_data)) != MSP_SUCCESS)
		{
			printf("离线语法识别出错: %d \n", ret);
			goto exit;
		}

 exit:
	//将识别到的结果进行输出
	MSPLogout();
	printf("语音识别完毕.\n"); 
	return 0;
}
