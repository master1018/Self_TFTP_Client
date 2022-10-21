#include "tftp_client_msg.h"
#include "tftp_clinet_io.h"

sTFTPClientMsgSendQueue g_TFTPClientMsgSendQueue;
SOCKADDR_IN g_serverAddr;
SOCKADDR_IN g_clientAddr;
SOCKET g_clientSock;

uint8_t g_send_timing = 0;

/*
*	@brief build socket and bind to the dst
*	@return return true when success build socket and bind
*/
bool tftp_clinet_io_build_connect(char* ip, int port)
{
	printf_s("connect to %s [%u]...\n", ip, port);
	bool retVal = true;
	g_serverAddr.sin_family = AF_INET;
	g_serverAddr.sin_addr.S_un.S_addr = inet_addr(ip);
	g_serverAddr.sin_port = htons(port);

	g_clientSock = socket(AF_INET, SOCK_DGRAM, 0);
	if (g_clientSock == INVALID_SOCKET)
	{
		retVal = false;
	}

	
	if (bind(g_clientSock, (LPSOCKADDR)&g_clientAddr, sizeof(g_clientAddr)))
	{
		retVal = false;
	}
	
	return retVal;
}

void test_send(uint8_t* pMsg)
{
	pTFTPClientHeader pHeader = (pTFTPClientHeader)pMsg;
	int a = sendto(g_clientSock, (char*)(pMsg + sizeof(sTFTPClientHeader)), pHeader->size, 0, (LPSOCKADDR)&g_serverAddr, sizeof(g_serverAddr));
	for (int i = 0; i < pHeader->size; i++)
	{
		printf_s("%c", (char*)(pMsg + sizeof(sTFTPClientHeader))[i]);
	}
	for (int i = 0; i < pHeader->size; i++)
	{
		printf_s("%02x ", (pMsg + sizeof(sTFTPClientHeader))[i]);
	}
	printf("\nreturn %d\n", a);
}

/*
*	@brief	add msg to the send queue
*/
void tftp_io_add_msg(uint8_t* pMsg)
{
	assert(g_TFTPClientMsgSendQueue.num < MAX_NUM_OF_TFTP_CLIENT_SEND_MSG);
	pTFTPClientHeader pHeader = (pTFTPClientHeader)pMsg;
	// init element
	memset(g_TFTPClientMsgSendQueue.msg[g_TFTPClientMsgSendQueue.num], 0, sizeof(MAX_TFTP_CLIENT_SEND_MSG_LENGTH));
	memcpy(g_TFTPClientMsgSendQueue.msg[g_TFTPClientMsgSendQueue.num], pMsg, sizeof(pMsg));
	g_TFTPClientMsgSendQueue.num++;
}

/*
*	@brief	send msg that save in the queue
*/
void tftp_io_send_msg()
{
	uint8_t numOfPkts = g_TFTPClientMsgSendQueue.num;

	for (int i = 0; i < numOfPkts; i++)
	{
		uint8_t* pMsg = g_TFTPClientMsgSendQueue.msg[i];
		pTFTPClientHeader pHeader = (pTFTPClientHeader)pMsg;
		while (1)
		{
			// waiting for recv thread
			if (1 == g_send_timing)
			{
				break;
			}
		}
		sendto(g_clientSock, (char*)(pMsg + sizeof(sTFTPClientHeader)), pHeader->size, 0, (LPSOCKADDR)&g_serverAddr, sizeof(g_serverAddr));
		g_send_timing = 0;
	}
	g_TFTPClientMsgSendQueue.num = 0;
}

/*
*	@brief	handle msg that recv from the server
*/
void tftp_client_io_handle_recv(uint8_t *pMsg)
{
	uint16_t nOperationCode = pMsg[1];
	switch (nOperationCode)
	{
	default:
		break;
	}
}

/*
*	@brief	recv pkt from the server
*/
void tftp_client_io_recv_thread()
{
	while (1)
	{
		uint8_t buf[MAX_TFTP_CLIENT_RECV_MSG_LENGTH] = { '\0' };
		int serverAddrLen = sizeof(g_serverAddr);
		int pktSize = recvfrom(g_clientSock, (char*)buf, MAX_TFTP_CLIENT_RECV_MSG_LENGTH, 0, (LPSOCKADDR)&g_serverAddr, &serverAddrLen);
		if (pktSize > 0)
		{
			g_send_timing = 1;
		}

	}
}