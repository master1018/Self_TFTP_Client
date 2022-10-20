#include "tftp_client_msg.h"

void tftp_client_build_add_header(uint8_t* pMsg, pTFTPClientHeader pHeader)
{
	memcpy(pMsg, pHeader, sizeof(pHeader));
}

void tftp_client_build_WRQ(uint8_t* pMsg, uint8_t* fileName, uint8_t* transferMode)
{
	pTFTPClinetRq pRrqMsg		=		(pTFTPClinetRq)malloc(sizeof(sTFTPClinetRq));
	assert(pRrqMsg != NULL);
	pRrqMsg->operationCode		=		RRQ_REQUEST_MSG;
	strcpy((char *)pRrqMsg->fileName, (char *)fileName);
	strcpy((char*)pRrqMsg->transferMode, (char*)transferMode);
	uint16_t sizeofPkt = sizeof(pRrqMsg->operationCode) + sizeof(pRrqMsg->fileName) + sizeof(pRrqMsg->transferMode);
	
	pTFTPClientHeader pHeader = (pTFTPClientHeader)malloc(sizeof(sTFTPClientHeader));
	assert(pHeader != NULL);

	tftp_client_build_add_header(pMsg, pHeader);
	memcpy(pMsg + sizeof(sTFTPClientHeader), pRrqMsg, sizeofPkt);
}