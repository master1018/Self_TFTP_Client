#include "tftp_client_msg.h"
#include "tftp_clinet_io.h"

sTFTPClientMsgSendQueue g_TFTPClientMsgSendQueue;
SOCKADDR_IN g_serverAddr;
SOCKADDR_IN g_clientAddr;
SOCKET g_clientSock;

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
	/*SOCKADDR_IN addr;
	addr.sin_port = 0;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if ((bind(g_clientSock, (LPSOCKADDR)&addr, sizeof(addr)))) {   //转化成LPSOCKADDR才行
		printf("错误码：%d    ", WSAGetLastError());
		printf("服务器端绑定失败\n");
		exit(-1);
	}*/
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

void tftp_io_send_msg()
{
	uint8_t numOfPkts = g_TFTPClientMsgSendQueue.num;

	for (int i = 0; i < numOfPkts; i++)
	{
		uint8_t* pMsg = g_TFTPClientMsgSendQueue.msg[i];
		pTFTPClientHeader pHeader = (pTFTPClientHeader)pMsg;

		sendto(g_clientSock, (char*)(pMsg + sizeof(sTFTPClientHeader)), pHeader->size, 0, (LPSOCKADDR)&g_serverAddr, sizeof(g_serverAddr));
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