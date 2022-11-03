#ifndef _TFTP_CLIENT_STAT_H_
#define _TFTP_CLIENT_STAT_H_
#include "common_def.h"
#include <time.h>
typedef struct TFTP_CLIENT_STAT_MSG {
	uint32_t numReTran;  // num of pkts reTransfer
	clock_t start, end;
	
	void init_time()
	{
		this->start = this->end = 0;
	}


	double cal_time()
	{
		return (double)(this->end - this->start) / CLOCKS_PER_SEC;
	}

}sTFTPClientStatMsg;
#endif // !_TFTP_CLIENT_STAT_H_
