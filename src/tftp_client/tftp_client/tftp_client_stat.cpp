#include "tftp_client_stat.h"

SYSTEMTIME sysTime;
uint8_t g_signal = 0;
sTFTPClientStatMsg g_TFTPClientStatMsg;

void tftp_client_stat_log()
{
	FILE* fp = fopen(".\\local\\log.txt", "a+");
	assert(fp != NULL);
	// get time
	GetLocalTime(&sysTime);
	fprintf(fp, "%4d-%2d-%2d %2d:%2d:%2d\n", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	fprintf(fp, "+---------------------------------------------------------------------------------------------------------------------------------------------------+\n");
	fprintf(fp, "|	numUlPkts	|	numDlPkts	|	numRetranPkts	|	ulRate	|	dlRate	|\n");
	fprintf(fp, "+---------------------------------------------------------------------------------------------------------------------------------------------------+\n");
	fprintf(fp, "| %10d			| %10d			| %10d     | %8.2lf bps     | %8.2lf bps     |\n", g_TFTPClientStatMsg.ulPkts, g_TFTPClientStatMsg.dlPkts, g_TFTPClientStatMsg.numReTran,\
		g_TFTPClientStatMsg.cal_rate_ul(), g_TFTPClientStatMsg.cal_rate_dl());
	fprintf(fp, "-----------------------------------------------------------------------------------------------------------------------------------------------------\n");
	fprintf(fp, "\n\n");
	fclose(fp);
}

DWORD WINAPI tftp_client_stat_thread(LPVOID lpParameter)
{
	// init
	while (1)
	{
		if (1 == g_signal)
		{
			tftp_client_stat_log();
			g_signal = 0;
			break;
		}
	}
	return 0;
}