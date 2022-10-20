#ifndef _TFTP_CLIENT_IO_H_
#define _TFTP_CLIENT_IO_H_
#include "common_def.h"
bool tftp_clinet_io_build_connect(char* ip, int port);
void test_send(uint8_t* pMsg);
#endif 

