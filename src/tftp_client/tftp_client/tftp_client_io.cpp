#include "tftp_client_msg.h"
#include "tftp_clinet_io.h"

sTFTPClientMsgSendQueue g_TFTPClientMsgSendQueue;
SOCKADDR_IN g_addr;
SOCKET g_clientSock;

bool tftp_clinet_io_build_connect(char* ip, int port)
{
	printf_s("connect to %s [%u]...\n", ip, port);

	g_addr.sin_family = AF_INET;
	g_addr.sin_addr.S_un.S_addr = inet_addr(ip);
	g_addr.sin_port = htons(port);

	g_clientSock = socket(AF_INET, SOCK_DGRAM, 0);
	if (g_clientSock == INVALID_SOCKET)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void test_send(uint8_t* pMsg)
{
	sendto(g_clientSock, (char*)pMsg, sizeof(pMsg), 0, (LPSOCKADDR)&g_addr, sizeof(g_addr));
	test_send(pMsg);
}

void tftp_io_send_msg()
{
	uint8_t numOfPkts = g_TFTPClientMsgSendQueue.num;

	for (int i = 0; i < numOfPkts; i++)
	{
		uint8_t* pMsg = g_TFTPClientMsgSendQueue.msg[i];
		pTFTPClientHeader pHeader = (pTFTPClientHeader)pMsg;

		sendto(g_clientSock, (char*)(pMsg + sizeof(sTFTPClientHeader)), pHeader->size, 0, (LPSOCKADDR)&g_addr, sizeof(g_addr));
		/*uint16_t msgType = ((uint16_t*)pMsg)[0];
		switch (msgType)
		{
		case RRQ_REQUEST_MSG:
		{
			return sendto(g_clientSock, (char *)pMsg, sizeof(pMsg), 0, (LPSOCKADDR)&g_addr, sizeof(g_addr));
			break;
		}
		case WRQ_REQUEST_MSG:
		{
			return sendto(g_clientSock, (char*)pMsg, sizeof(pMsg), 0, (LPSOCKADDR)&g_addr, sizeof(g_addr));
			break;
		}
		case DATA_MSG:
		case ACK_MSG:
		case ERROR_MSG:
		default:
			break;
		}*/
	}
}