#include "tftp_client_msg.h"
#include "tftp_clinet_io.h"
#include "tftp_client_build.h"

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
void tftp_client_io_add_msg(uint8_t* pMsg)
{
	assert(g_TFTPClientMsgSendQueue.num < MAX_NUM_OF_TFTP_CLIENT_SEND_MSG);
	pTFTPClientHeader pHeader = (pTFTPClientHeader)pMsg;
	// init element
	memset(g_TFTPClientMsgSendQueue.msg[g_TFTPClientMsgSendQueue.num], 0, sizeof(MAX_TFTP_CLIENT_SEND_MSG_LENGTH));
	uint8_t* base = g_TFTPClientMsgSendQueue.msg[g_TFTPClientMsgSendQueue.num];
	memcpy(base, pMsg, pHeader->size + sizeof(sTFTPClientHeader));
	g_TFTPClientMsgSendQueue.num++;
}


/*
*	@brief	recv pkt from the server
*/
int tftp_client_io_recv_msg(uint8_t *pMsg)
{
	uint8_t buf[MAX_TFTP_CLIENT_RECV_MSG_LENGTH] = { '\0' };
	int serverAddrLen = sizeof(g_serverAddr);
	while (1)
	{
		int pktSize = recvfrom(g_clientSock, (char*)buf, MAX_TFTP_CLIENT_RECV_MSG_LENGTH, 0, (LPSOCKADDR)&g_serverAddr, &serverAddrLen);
		if (pktSize > 0)
		{
			memcpy(pMsg, buf, sizeof(pktSize));
			return pktSize;
		}
	}
	return -1;
}

/*
*	@brief	handle msg that recv from the server
*/
tuple<uint16_t, uint16_t> tftp_client_io_handle_recv()
{
	uint8_t* pMsg = (uint8_t*)malloc(sizeof(uint8_t) * MAX_TFTP_CLIENT_RECV_MSG_LENGTH);
	assert(pMsg != NULL);
	// (0, 0) denote invalid msg
	tuple<uint16_t, uint16_t> retVal(0, 0);
	if (tftp_client_io_recv_msg(pMsg) < 0)
	{
		return retVal;
	}
	uint16_t nOperationCode = pMsg[1];
	switch (nOperationCode)
	{
	case ACK_MSG:
	{
		retVal = make_tuple(nOperationCode, ((pTFTPClientAck)pMsg)->blockNumber);
		break;
	}
	default:
		break;
	}
	return retVal;
}

/*
*	@brief	send msg that save in the queue
*/
uint32_t tftp_client_io_send_msg()
{
	uint8_t numOfPkts = g_TFTPClientMsgSendQueue.num;
	uint16_t nOperationCode, nBlockNumber;
	for (int i = 0; i < numOfPkts; i++)
	{
		uint8_t* pMsg = g_TFTPClientMsgSendQueue.msg[i];
		pTFTPClientHeader pHeader = (pTFTPClientHeader)pMsg;
		//for (int i = 0; i < pHeader->size + sizeof(sTFTPClientHeader); i++)
		//	printf_s("%02x ", pMsg[i]);
		//printf_s("\n");
		printf_s("%u\n", g_serverAddr.sin_port);
		sendto(g_clientSock, (char*)(pMsg + sizeof(sTFTPClientHeader)), pHeader->size, 0, (LPSOCKADDR)&g_serverAddr, sizeof(g_serverAddr));
		g_TFTPClientMsgSendQueue.num--;
		tie(nOperationCode, nBlockNumber) = tftp_client_io_handle_recv();
		// TODO recv msg and handle
		switch (nOperationCode)
		{
		// 当收到回应包时，继续流程
		case ACK_MSG:
		{
			if (nBlockNumber == i)
				break;
		}
		default:
			break;
		}
	}
	return g_TFTPClientMsgSendQueue.num;
}

void tftp_client_io_ul(uint8_t* fileName)
{
	FILE* fp = fopen((char*)fileName, "r");
	if (NULL == fp)
	{
		printf_s("file %s no found.\n", fileName);
		return;
	}
	fseek(fp, 0, SEEK_END);
	int nSize = ftell(fp); 
	fseek(fp, 0, SEEK_SET);
	assert(nSize > 0);
	
	uint8_t* pData = (uint8_t*)malloc(sizeof(uint8_t) * (nSize + 5));
	assert(pData != NULL);
	fread(pData, 1, nSize, fp);
	fclose(fp);

	int offset = 0;
	uint8_t* base = pData;

	for (; offset < nSize - 512; offset += 512)
	{
		uint8_t* pMsg = (uint8_t*)malloc(sizeof(uint8_t) * MAX_TFTP_CLIENT_RECV_MSG_LENGTH);
		assert(pMsg != NULL);
		tftp_client_build_DATA(pMsg, offset / 512 + 1, base + offset, 512);
		tftp_client_io_add_msg(pMsg);
		free(pMsg);
		pMsg = NULL;
	}
	uint8_t* pMsg = (uint8_t*)malloc(sizeof(uint8_t) * 1024);
	assert(pMsg != NULL);
	tftp_client_build_DATA(pMsg, offset / 512 + 1, base + offset, nSize - offset);
	tftp_client_io_add_msg(pMsg);
	free(pMsg);
	pMsg = NULL;

}