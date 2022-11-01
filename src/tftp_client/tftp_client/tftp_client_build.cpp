#include "tftp_client_msg.h"

extern uint8_t g_send_timing;

void tftp_client_build_add_header(uint8_t* pMsg, pTFTPClientHeader pHeader)
{
	memcpy(pMsg, pHeader, sizeof(pHeader));
}

void tftp_client_build_WRQ(uint8_t* pMsg, uint8_t* fileName, uint8_t* transferMode)
{
	pTFTPClinetRq pWrqMsg		=		(pTFTPClinetRq)malloc(sizeof(sTFTPClinetRq));
	assert(pWrqMsg != NULL);
	pWrqMsg->operationCode		=		htons(WRQ_REQUEST_MSG);
	strcpy((char *)pWrqMsg->fileName, (char *)fileName);
	strcpy((char*)pWrqMsg->transferMode, (char*)transferMode);
	uint32_t lenFileName						= strlen((char *)fileName);
	uint32_t lenTransferMode					=		strlen((char*)transferMode);
	pWrqMsg->fileName[lenFileName]				=		'\0';
	pWrqMsg->transferMode[lenTransferMode]		=		'\0';
	uint16_t sizeofPkt = sizeof(pWrqMsg->operationCode) + lenFileName  + lenTransferMode + 2;
	
	pTFTPClientHeader pHeader = (pTFTPClientHeader)malloc(sizeof(sTFTPClientHeader));
	assert(pHeader != NULL);
	pHeader->size = sizeofPkt;
	tftp_client_build_add_header(pMsg, pHeader);
	
	uint8_t* base = pMsg;
	base += sizeof(sTFTPClientHeader);
	memcpy(base, &(pWrqMsg->operationCode), sizeof(pWrqMsg->operationCode));
	base += sizeof(pWrqMsg->operationCode);
	memcpy(base, pWrqMsg->fileName, lenFileName + 1);
	base += lenFileName + 1;
	memcpy(base, pWrqMsg->transferMode, lenTransferMode + 1);
}

void tftp_client_build_RRQ(uint8_t* pMsg, uint8_t* fileName, uint8_t* transferMode)
{
	pTFTPClinetRq pRrqMsg = (pTFTPClinetRq)malloc(sizeof(sTFTPClinetRq));
	assert(pRrqMsg != NULL);
	pRrqMsg->operationCode = htons(RRQ_REQUEST_MSG);
	strcpy((char*)pRrqMsg->fileName, (char*)fileName);
	strcpy((char*)pRrqMsg->transferMode, (char*)transferMode);
	uint32_t lenFileName = strlen((char*)fileName);
	uint32_t lenTransferMode = strlen((char*)transferMode);
	pRrqMsg->fileName[lenFileName] = '\0';
	pRrqMsg->transferMode[lenTransferMode] = '\0';
	uint16_t sizeofPkt = sizeof(pRrqMsg->operationCode) + lenFileName + lenTransferMode + 2;

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


void tftp_client_build_DATA(uint8_t* pMsg, uint16_t nBlockNumber, uint8_t* pData, uint32_t nSizeOfData)
{
	pTFTPClientData pDataMsg				=			(pTFTPClientData)malloc(sizeof(sTFTPClinetData));
	assert(pDataMsg != NULL);
	pDataMsg->operationCode					=			htons(DATA_MSG);
	pDataMsg->blockNumber					=			htons(nBlockNumber);
	memcpy(pDataMsg->data, pData, nSizeOfData);

	pTFTPClientHeader pHeader				=			(pTFTPClientHeader)malloc(sizeof(sTFTPClientHeader));
	assert(pHeader != NULL);
	pHeader->size = sizeof(uint16_t) * 2 + nSizeOfData * sizeof(uint8_t);
	tftp_client_build_add_header(pMsg, pHeader);

	uint8_t* base = pMsg;
	base += sizeof(sTFTPClientHeader);
	memcpy(base, &(pDataMsg->operationCode), sizeof(pDataMsg->operationCode));
	base += sizeof(pDataMsg->operationCode);
	memcpy(base, &(pDataMsg->blockNumber), sizeof(pDataMsg->blockNumber));
	base += sizeof(uint16_t);
	memcpy(base, pDataMsg->data, nSizeOfData);

}

void tftp_client_build_ACK(uint8_t* pMsg, uint16_t nBlockNumber)
{
	pTFTPClientAck pAckMsg		=		(pTFTPClientAck)malloc(sizeof(sTFTPClinetAck));
	assert(pAckMsg != NULL);
	pAckMsg->operationCode		=		htons(ACK_MSG);
	pAckMsg->blockNumber		=		htons(nBlockNumber);
	uint8_t* base = pMsg;
	memcpy(base, &(pAckMsg->operationCode), sizeof(pAckMsg->operationCode));
	base += sizeof(pAckMsg->operationCode);
	memcpy(base, &(pAckMsg->blockNumber), sizeof(pAckMsg->blockNumber));
}