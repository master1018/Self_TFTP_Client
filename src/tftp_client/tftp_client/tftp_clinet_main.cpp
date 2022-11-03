#include "tftp_client_msg.h"
#include "tftp_clinet_io.h"
#include "tftp_client_build.h"

#define MAX_COMMAND_LENGTH		(10)
#define MAX_PARAM_NUM			(3)
#define MAX_PARAM_LENGTH		(1024)
#define MAX_DIR_PATH_LEN		(50)

#define INIT_STATUS				(0)
#define CONNECT_STATUS			(1)

extern SOCKADDR_IN g_clientAddr;
extern SOCKADDR_IN g_serverAddr;
int port;
char usr_dir[MAX_DIR_PATH_LEN] = { '\0' };

int split(char dst[][MAX_PARAM_LENGTH], char* str, const char* spl)
{
	int n = 0;
	char* result = NULL;
	result = strtok(str, spl);
	while (result != NULL)
	{
		strcpy(dst[n++], result);
		result = strtok(NULL, spl);
	}
	return n;
}

int parse_for_config()
{
	FILE* fp = fopen(".\\local\\tftp_client.ini", "r");
	if (NULL == fp)
	{
		printf_s("file tftp_client.config not found.\n");
		return 0;
	}

	map<string, string> mConfig;
	char tmp[2048] = { '\0' };
	int line = 0;
	while (fscanf(fp,"%s", tmp) != EOF) 
	{
		line++;
		char dst[2][MAX_PARAM_LENGTH];
		if (2 != split(dst, tmp, "="))
		{
			printf_s("tftp_client.config: error config in line %d.\n", line);
		}
		mConfig[dst[0]] = dst[1];
	}
	fclose(fp);

	g_clientAddr.sin_family = AF_INET;
	g_clientAddr.sin_addr.S_un.S_addr = inet_addr(mConfig["client_ip"].c_str());
	g_clientAddr.sin_port = htons(atoi(mConfig["client_port"].c_str()));

	strcpy(usr_dir, mConfig["usr_dir"].c_str());

	
	return 1;
}

int init()
{
	int retVal = 1;
	retVal &= parse_for_config();
	return retVal;
}

int command_parse(char* command, char params[][MAX_PARAM_LENGTH], int nNumOfParams)
{
	if (0 == strcmp(command, QUIT))
	{
		return 0;
	}
	else if (0 == strcmp(command, CONNECT))
	{
		port = nNumOfParams == 2 ? atoi(params[1]) : 69;
		if (true == tftp_clinet_io_build_connect(params[0], port))
		{
			printf_s("connect sucess.\n");
		}
		else
		{
			printf_s("connect failed.\n");
		}
		return 1;
	}
	else if (0 == strcmp(command, PUT))
	{
		printf_s("upload data: %s\n", params[0]);
		FILE* fp = fopen((char*)params[0], "rb");
		if (NULL == fp)
		{
			printf_s("file %s no found.\n", params[0]);
			return 1;
		}
		// build msg
		uint8_t* pMsg = (uint8_t*)malloc(sizeof(uint8_t) * MAX_TFTP_CLIENT_RECV_MSG_LENGTH);
		tftp_client_build_WRQ(pMsg, (uint8_t *)params[0], nNumOfParams == 1 ? (uint8_t *)"netascii" :\
			atoi(params[1]) == 1 ? (uint8_t *)"netascii" : (uint8_t *)"octet");
		tftp_client_io_add_msg(pMsg);
		// 1 for netascii, 2 for octet
		if (0 == tftp_client_io_ul((uint8_t*)params[0], nNumOfParams == 1 ? 1 : atoi(params[1])))
		{
			printf_s("upload failed.\n");
		}
		else
		{
			if (0 == tftp_client_io_send_msg())
			{
				printf_s("upload success.\n");
			}
			else
			{
				printf_s("upload failed.\n");
			}
		}
		g_clientAddr.sin_port = 0;
		g_serverAddr.sin_port = htons(port);
		return 1;
	}
	else if (0 == strcmp(command, GET))
	{
		printf_s("download data: %s\n", params[0]);
		uint8_t filePath[1024] = { '\0' };
		strcpy((char *)filePath, usr_dir);
		strcat((char*)filePath, params[0]);
		FILE* fp = fopen((char *)filePath, "w");
		assert(fp != NULL);
		fclose(fp);
		// build 
		uint8_t* pMsg = (uint8_t*)malloc(sizeof(uint8_t) * MAX_TFTP_CLIENT_RECV_MSG_LENGTH);
		tftp_client_build_RRQ(pMsg, (uint8_t*)params[0], nNumOfParams == 1 ? (uint8_t*)"netascii" : \
			atoi(params[1]) == 1 ? (uint8_t*)"netascii" : (uint8_t*)"octet");
		tftp_client_io_add_msg(pMsg);
		if (0 == tftp_client_io_dl(filePath, nNumOfParams == 1 ? 1 : atoi(params[1])))
		{
			printf_s("download success.\n");
		}
		else
		{
			printf_s("download failed.\n");
		}
		g_clientAddr.sin_port = 0;
		g_serverAddr.sin_port = htons(port);
		return 1;
	}
}


int get_params_from_input(char* buff, char params[][MAX_PARAM_LENGTH])
{
	int nNumOfParams = split(params, buff, " ");
	return nNumOfParams;
}

void print_ui()
{
	printf("************************************************************************************************************\n");
	printf("¡¡¡¡¡¡¡¡¡¡ ¡¡ __		|	TFTP Client Version 1.1\n");
	printf("¡¡¡¡¡¡¡¡¡¡£¯£¾¡¡ ¥Õ		|	command:\n");
	printf("¡¡¡¡¡¡¡¡¡¡| ¡¡_¡¡ _		|	--	connect [IP] [Port]\n");
	printf("¡¡ ¡¡¡¡¡¡£¯`¥ß£ßx¥Î		|		Create connect between client and server, and the port default 69.\n");
	printf("¡¡¡¡ ¡¡ /¡¡¡¡¡¡  |		|	--	put [fileName] [mode(1 for netascii, 2 for octet]\n");
	printf("¡¡¡¡¡¡ /¡¡ ©c¡¡  |		|		Upload data to the server.\n");
	printf("¡¡ ¡¡ |¡¡ |  |   |		|	--	get [fileName] [mode(1 for netascii, 2 for octet]\n");
	printf("¡¡£¯£þ|¡¡  |  |  |		|		Download data from the server.\n");
	printf("¡¡| (£þ©c£ß_©c_)__)		|	--	quit\n");
	printf("¡¡£Ü¶þ¤Ä			|		End client\n");
	printf("************************************************************************************************************\n");
}

int main()
{
	if (!init())
	{
		printf_s("tftp client init failed.\n");
		return 0;
	}
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	char command[MAX_COMMAND_LENGTH];
	char buff[2056];
	char params[MAX_PARAM_NUM][MAX_PARAM_LENGTH];
	print_ui();
	printf_s("\n");
	while (1)
	{
		printf_s("tftp>");
		scanf("%s", command);
		gets_s(buff);
		int nNumOfParams = get_params_from_input(buff, params);
		if (!command_parse(command, params, nNumOfParams))
		{
			break;
		}
	}
	system("pause");
	return 0;
}