#ifndef _TFTP_CLIENT_BUILD_H_
#define _TFTP_CLIENT_BUILD_H_
#include "common_def.h"
void tftp_client_build_WRQ(uint8_t* pMsg, uint8_t* fileName, uint8_t* transferMode);
void tftp_client_build_DATA(uint8_t* pMsg, uint16_t nBlockNumber, uint8_t* pData, uint32_t nSizeOfData);
#endif // !_TFTP_CLIENT_BUILD_H_

