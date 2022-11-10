#ifndef _TFTP_CLIENT_STAT_H_
#define _TFTP_CLIENT_STAT_H_
#include "common_def.h"
#include <time.h>
typedef struct TFTP_CLIENT_STAT_MSG {
	uint32_t ulPkts;
	uint32_t dlPkts;
	uint32_t ulSize;
	uint32_t dlSize;
	uint32_t numReTran;  // num of pkts reTransfer
	uint32_t sizeReTran;
	clock_t start, end;
	uint8_t state;
	uint8_t erroInfo[256];
	double time;
	void init()
	{
		this->start = this->end = 0;
		this->ulPkts = this->dlPkts = this->numReTran = 0;
		this->ulSize = this->dlSize = this->sizeReTran = 0;
		this->time = 0.0;
		this->state = 0;
		memset(this->erroInfo, 0, 256);
	}

	void update_time()
	{
		this->time += (double)(this->end - this->start) / CLOCKS_PER_SEC;
	}

	double cal_rate_ul()
	{
		return (double)(this->ulSize * 1.0 / this->time);
	}

	double cal_rate_dl()
	{
		return (double)(this->dlSize * 1.0 / this->time);
	}

}sTFTPClientStatMsg;

DWORD WINAPI tftp_client_stat_thread(LPVOID lpParameter);

#endif // !_TFTP_CLIENT_STAT_H_
