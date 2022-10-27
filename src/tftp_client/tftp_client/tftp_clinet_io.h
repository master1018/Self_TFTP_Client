#ifndef _TFTP_CLIENT_IO_H_
#define _TFTP_CLIENT_IO_H_
#include "common_def.h"
bool tftp_clinet_io_build_connect(char* ip, int port);
void test_send(uint8_t* pMsg);
void tftp_client_io_add_msg(uint8_t* pMsg);
uint32_t tftp_client_io_ul(uint8_t* fileName);
uint32_t tftp_client_io_send_msg();
#endif 

