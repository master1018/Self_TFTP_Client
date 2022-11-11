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
	fprintf(fp, "%s\n", g_TFTPClientStatMsg.action);
	fprintf(fp, "STATE: %s\n", g_TFTPClientStatMsg.state == 1 ? "SUCCESS" : "FAILED");
	if (g_TFTPClientStatMsg.state == 0)
		fprintf(fp, "%s\n", g_TFTPClientStatMsg.erroInfo);
	fprintf(fp, "+---------------------------------------------------------------------------------------------------------------------------------------------------+\n");
	fprintf(fp, "|	numUlPkts	|	numDlPkts	|	numRetranPkts	|	ulRate	|	dlRate	|\n");
	fprintf(fp, "+---------------------------------------------------------------------------------------------------------------------------------------------------+\n");
	fprintf(fp, "| %10d			| %10d			| %10d			| %8.2lf kb/s		| %8.2lf kb/s     |\n", g_TFTPClientStatMsg.ulPkts, g_TFTPClientStatMsg.dlPkts, g_TFTPClientStatMsg.numReTran,\
		g_TFTPClientStatMsg.cal_rate_ul() / 1000, g_TFTPClientStatMsg.cal_rate_dl() / 1000);
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
		/*else if (g_signal == 2)
		{
			printf("send %dpkts | %dbytes, retran %dpkts | %dbytes\n", g_TFTPClientStatMsg.ulPkts, g_TFTPClientStatMsg.ulSize, g_TFTPClientStatMsg.numReTran, g_TFTPClientStatMsg.sizeReTran);
			printf("rate: %.lfkb/s\n", g_TFTPClientStatMsg.cal_rate_ul() / 1000);
			g_signal = 0;
		}
		else if (g_signal == 3)
		{
			printf("send %dpkts | %dbytes, retran %dpkts | %dbytes\n", g_TFTPClientStatMsg.dlPkts, g_TFTPClientStatMsg.dlSize, g_TFTPClientStatMsg.numReTran, g_TFTPClientStatMsg.sizeReTran);
			printf("rate: %.lfkb/s\n", g_TFTPClientStatMsg.cal_rate_dl() / 1000);
			g_signal = 0;
		}
		*/
	}
	return 0;
}