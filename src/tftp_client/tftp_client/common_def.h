#ifndef _TFTP_CLIENT_COMMON_DEF_H_ 
#define _TFTP_CLIENT_COMMON_DEF_H_ 
#include <map>
#include <tuple>
#include <string>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <assert.h>
#include <Windows.h>
#pragma comment(lib,"Ws2_32.lib")
using namespace std;
// command that valid
#define CONNECT		"connect"
#define QUIT		"quit"
#define PUT			"put"
#define GET			"get"

typedef unsigned char			uint8_t;
typedef unsigned short int		uint16_t;
typedef unsigned int			uint32_t;
#endif // !_TFTP_CLIENT_COMMON_DEF_H_ 

