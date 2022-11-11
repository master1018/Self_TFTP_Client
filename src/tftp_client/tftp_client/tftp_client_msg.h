#include "common_def.h"
#ifndef _FTP_CLIENT_MSG_H_
#define _FTP_CLIENT_MSG_H_

#define MAX_TFTP_CLINET_DATA_BYTES				(512)
#define MAX_TFTP_CLINET_ERROR_MESSAGE_LENGTH	(50)
#define MAX_TFTP_CLIENT_SEND_MSG_LENGTH			(2056)
#define MAX_NUM_OF_TFTP_CLIENT_SEND_MSG			(100000)
#define MAX_TFTP_CLIENT_RECV_MSG_LENGTH			MAX_TFTP_CLIENT_SEND_MSG_LENGTH

#define RRQ_REQUEST_MSG								(1)
#define WRQ_REQUEST_MSG								(2)
#define DATA_MSG									(3)
#define ACK_MSG										(4)
#define ERROR_MSG									(5)

typedef struct TFTP_CLINET_RQ {
	uint16_t operationCode;  // 1 for RRQ and 2 for WRQ
	uint8_t fileName[1024];       // end with ascii 0
	uint8_t transferMode[10];   // end with ascii 0
}sTFTPClinetRq, *pTFTPClinetRq;

typedef struct TFTP_CLIENT_DATA {
	uint16_t operationCode;
	uint16_t blockNumber;
	uint8_t data[MAX_TFTP_CLINET_DATA_BYTES];  
}sTFTPClinetData, *pTFTPClientData;

typedef struct TFTP_CLINET_ACK {
	uint16_t operationCode;
	uint16_t blockNumber;
}sTFTPClinetAck, *pTFTPClientAck;

typedef struct TFTP_CLINET_ERROR {
	uint16_t operationCode;
	uint16_t errorCode;
	uint8_t errorMsg[MAX_TFTP_CLINET_ERROR_MESSAGE_LENGTH];
}sTFTPClinetError, *pTFTPClinetError;

typedef struct TFTP_CLIENT_HEADER {
	uint16_t size;
}sTFTPClientHeader, *pTFTPClientHeader;

typedef struct TFTP_CLIENT_MSG_SEND_Queue {
	uint8_t msg[MAX_NUM_OF_TFTP_CLIENT_SEND_MSG][MAX_TFTP_CLIENT_SEND_MSG_LENGTH];
	uint32_t num;
	void init()
	{
		this->num = 0;
		for (int i = 0; i < MAX_NUM_OF_TFTP_CLIENT_SEND_MSG; i++)
			for (int j = 0; j < MAX_TFTP_CLIENT_SEND_MSG_LENGTH; j++)
				(this->msg)[i][j] = 0;
	}
}sTFTPClientMsgSendQueue, sTFTPClientMsgRecvQueue;
#endif

