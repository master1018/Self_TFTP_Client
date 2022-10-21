#include "tftp_client_msg.h"

extern uint8_t g_send_timing;

void tftp_client_build_add_header(uint8_t* pMsg, pTFTPClientHeader pHeader)
{
	memcpy(pMsg, pHeader, sizeof(pHeader));
}

void tftp_client_build_WRQ(uint8_t* pMsg, uint8_t* fileName, uint8_t* transferMode)
{
	g_send_timing = 1;
	pTFTPClinetRq pRrqMsg		=		(pTFTPClinetRq)malloc(sizeof(sTFTPClinetRq));
	assert(pRrqMsg != NULL);
	pRrqMsg->operationCode		=		htons(RRQ_REQUEST_MSG);
	strcpy((char *)pRrqMsg->fileName, (char *)fileName);
	strcpy((char*)pRrqMsg->transferMode, (char*)transferMode);
	uint32_t lenFileName						= strlen((char *)fileName);
	uint32_t lenTransferMode					=		strlen((char*)transferMode);
	pRrqMsg->fileName[lenFileName]				=		'\0';
	pRrqMsg->transferMode[lenTransferMode]		=		'\0';
	uint16_t sizeofPkt = sizeof(pRrqMsg->operationCode) + lenFileName  + lenTransferMode + 2;
	
	pTFTPClientHeader pHeader = (pTFTPClientHeader)malloc(sizeof(sTFTPClientHeader));
	assert(pHeader != NULL);
	pHeader->size = sizeofPkt;
	tftp_client_build_add_header(pMsg, pHeader);
	
	uint8_t* base = pMsg;
	base += sizeof(sTFTPClientHeader);
	memcpy(base, &(pRrqMsg->operationCode), sizeof(pRrqMsg->operationCode));
	base += sizeof(pRrqMsg->operationCode);
	memcpy(base, pRrqMsg->fileName, lenFileName + 1);
	base += lenFileName + 1;
	memcpy(base, pRrqMsg->transferMode, lenTransferMode + 1);
}