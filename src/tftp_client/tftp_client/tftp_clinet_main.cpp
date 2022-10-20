#include "tftp_client_msg.h"
#include "tftp_clinet_io.h"
#include "tftp_client_build.h"
#define MAX_COMMAND_LENGTH		(10)
#define MAX_PARAM_NUM			(3)
#define MAX_PARAM_LENGTH		(1024)

#define INIT_STATUS				(0)
#define CONNECT_STATUS			(1)

int command_parse(char* command, char params[][MAX_PARAM_LENGTH], int nNumOfParams)
{
	if (0 == strcmp(command, QUIT))
	{
		return 0;
	}
	else if (0 == strcmp(command, CONNECT))
	{
		if (true == tftp_clinet_io_build_connect(params[0], nNumOfParams == 2 ? atoi(params[1]) : 69))
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
		printf_s("download data: %s\n", params[0]);
		// test
		uint8_t* pMsg = (uint8_t*)malloc(sizeof(uint8_t) * 1024);
		tftp_client_build_WRQ(pMsg, (uint8_t *)params[0], (uint8_t *)"netascii");
		
	}
}

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

int get_params_from_input(char* buff, char params[][MAX_PARAM_LENGTH])
{
	int nNumOfParams = split(params, buff, " ");
	return nNumOfParams;
}

int main()
{
	// init
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	char command[MAX_COMMAND_LENGTH];
	char buff[2056];
	char params[MAX_PARAM_NUM][MAX_PARAM_LENGTH];
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
	return 0;
}