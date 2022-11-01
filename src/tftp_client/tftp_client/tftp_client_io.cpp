#include "tftp_client_msg.h"
#include "tftp_clinet_io.h"
#include "tftp_client_build.h"

sTFTPClientMsgSendQueue g_TFTPClientMsgSendQueue;
sTFTPClientMsgRecvQueue g_TFTPClientMsgRecvQueue;
SOCKADDR_IN g_serverAddr;
SOCKADDR_IN g_clientAddr;
SOCKET g_clientSock;

uint8_t g_send_timing = 0;

void memcpy_self(uint8_t* dst, uint8_t* src, uint32_t size)
{
	for (int i = 0; i < size; i++)
	{
		dst[i] = src[i];
	}
}

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
	uint8_t* base = pMsg;
	int serverAddrLen = sizeof(g_serverAddr);
	while (1)
	{
		int pktSize = recvfrom(g_clientSock, (char*)buf, MAX_TFTP_CLIENT_RECV_MSG_LENGTH, 0, (LPSOCKADDR)&g_serverAddr, &serverAddrLen);
		//printf("recv %d\n", pktSize);
		if (pktSize > 0)
		{
			memcpy_self(base, buf, pktSize);
			return pktSize;
		}
	}
	return -1;
}

void tftp_client_io_handle_error(pTFTPClinetError pMsg)
{
	printf_s("[error code %d] %s\n", pMsg->errorCode >> 8, (char*)(pMsg->errorMsg));
}

/*
*	@brief	handle msg that recv from the server
*/
tuple<uint16_t, uint16_t, uint16_t> tftp_client_io_handle_recv()
{
	uint8_t* pMsg = (uint8_t*)malloc(sizeof(uint8_t) * MAX_TFTP_CLIENT_RECV_MSG_LENGTH);
	assert(pMsg != NULL);
	// (0, 0) denote invalid msg
	tuple<uint16_t, uint16_t, uint16_t> retVal(0, 0, 0);
	uint32_t pktSize = tftp_client_io_recv_msg(pMsg);
	if (pktSize < 0)
	{
		return retVal;
	}
	uint16_t nOperationCode = pMsg[1];
	switch (nOperationCode)
	{
	case ACK_MSG:
	{
		retVal = make_tuple(nOperationCode, ntohs(((pTFTPClientAck)pMsg)->blockNumber), 0);
		printf("%d\n", ((pTFTPClientAck)pMsg)->blockNumber);
		break;
	}
	case ERROR_MSG:
	{
		tftp_client_io_handle_error((pTFTPClinetError)pMsg);
		break;
	}
	case DATA_MSG:
	{
		retVal = make_tuple(nOperationCode, ntohs(((pTFTPClientAck)pMsg)->blockNumber), pktSize == 16 * 2 + 512 ? 0 : 1);
		memcpy_self(g_TFTPClientMsgRecvQueue.msg[g_TFTPClientMsgRecvQueue.num], pMsg, pktSize);
		g_TFTPClientMsgRecvQueue.num += 1;
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
	uint32_t numOfPkts = g_TFTPClientMsgSendQueue.num;
	uint16_t nOperationCode, nBlockNumber, flag;
	flag = 0;
	for (uint32_t i = 0; i < numOfPkts; i++)
	{
		uint8_t* pMsg = g_TFTPClientMsgSendQueue.msg[i];
		pTFTPClientHeader pHeader = (pTFTPClientHeader)pMsg;
		//for (int i = 0; i < pHeader->size + sizeof(sTFTPClientHeader); i++)
		//	printf_s("%02x ", pMsg[i]);
		//printf_s("\n");
		//printf_s("%u\n", g_serverAddr.sin_port);
		//printf("send %d\n", i + 1);
		sendto(g_clientSock, (char*)(pMsg + sizeof(sTFTPClientHeader)), pHeader->size, 0, (LPSOCKADDR)&g_serverAddr, sizeof(g_serverAddr));
		memset(g_TFTPClientMsgSendQueue.msg[i], 0, 2056);
		g_TFTPClientMsgSendQueue.num--;
		if (flag == 1)
			break;
		RECV:
		tie(nOperationCode, nBlockNumber, flag) = tftp_client_io_handle_recv();
		printf_s("recv %d %d\n", nOperationCode, nBlockNumber);
		if (!nOperationCode && !nBlockNumber)
		{
			return -1;
		}
		//printf_s("recv %d\n", i + 1);
		// TODO recv msg and handle
		switch (nOperationCode)
		{
		// 当收到回应包时，继续流程
		case ACK_MSG:
		{
			if (nBlockNumber == i)
				break;
			else
				goto RECV;
		}
		case DATA_MSG:
		{
			uint8_t* pMsg = (uint8_t*)malloc(sizeof(sTFTPClinetAck));
			tftp_client_build_ACK(pMsg, nBlockNumber);
			tftp_client_io_add_msg(pMsg);
			i--;
			break;
		}
		default:
			break;
		}
	}
	return g_TFTPClientMsgSendQueue.num;
}

uint32_t tftp_client_io_ul(uint8_t* fileName, uint8_t mode)
{
	FILE* fp = fopen((char*)fileName, mode == OCTET_MODE ? "rb" : "r");
	assert(fp != NULL);
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
	if (nSize - offset == 512)
	{
		tftp_client_build_DATA(pMsg, offset / 512 + 2, 0, 0);
		tftp_client_io_add_msg(pMsg);
	}
	free(pMsg);
	pMsg = NULL;
	return 1;
}

uint32_t tftp_client_io_dl(uint8_t* savePath, uint8_t mode)
{
	if (0 != tftp_client_io_send_msg())
	{
		return 1;
	}
	// save data
	FILE* fp = fopen((char*)savePath, mode == 1 ? "w" : "wb");
	uint32_t count = 0;
	for (int i = 0; i < g_TFTPClientMsgRecvQueue.num; i++)
	{
		fwrite(g_TFTPClientMsgRecvQueue.msg[i], 1, strlen((char*)g_TFTPClientMsgRecvQueue.msg[i]), fp);
		memset(g_TFTPClientMsgRecvQueue.msg[i], 0, 2056);
		count++;
	}
	fclose(fp);
	g_TFTPClientMsgRecvQueue.num -= count;
	return g_TFTPClientMsgRecvQueue.num;
}