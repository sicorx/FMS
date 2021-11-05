#include "include.h"

extern char process_name[];
void print_start_log(void)
{
	fileLog(INFO, "+------------------------------------------------------------------------------+\n");
	fileLog(INFO, "+     PROCESS(%s) START OK !!                                            +\n",process_name);
	fileLog(INFO, "+------------------------------------------------------------------------------+\n");
}

void print_end_log(void)
{
	sleep(1);
	fileLog(INFO, "+------------------------------------------------------------------------------+\n");
	fileLog(INFO, "+     PROCESS SHUTDOWN COMPLETE !!                                             +\n");
	fileLog(INFO, "+------------------------------------------------------------------------------+\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
}

void fileLog(int flag, const char *fmt, ... )
{
	char szBuf[1024] = {0,};

    va_list   ap;
    va_start (ap, fmt);
    vsnprintf(szBuf, 1023, fmt, ap);
    va_end   (ap);

	time_t	current_time;
	struct	tm *struct_timer;	
	time(&current_time);
	struct_timer = localtime(&current_time);

	FILE* fp = fopen(LOG_FILE, "a");

#ifdef COLOR
	if(flag == WARNING)
	{
		fprintf(fp, "[%04d-%02d-%02d/%02d:%02d:%02d] %s%s%s",
			struct_timer->tm_year+1900,struct_timer->tm_mon+1,struct_timer->tm_mday,
			struct_timer->tm_hour,struct_timer->tm_min,struct_timer->tm_sec, TERMINAL_COLOR_YELLOW, szBuf, TERMINAL_COLOR_RESET);
	}
	else if(flag == CRITICAL)
	{
		fprintf(fp, "[%04d-%02d-%02d/%02d:%02d:%02d] %s%s%s",
			struct_timer->tm_year+1900,struct_timer->tm_mon+1,struct_timer->tm_mday,
			struct_timer->tm_hour,struct_timer->tm_min,struct_timer->tm_sec, TERMINAL_COLOR_RED, szBuf, TERMINAL_COLOR_RESET);
	}
	else
	{
		fprintf(fp, "[%04d-%02d-%02d/%02d:%02d:%02d] %s",
			struct_timer->tm_year+1900,struct_timer->tm_mon+1,struct_timer->tm_mday,
			struct_timer->tm_hour,struct_timer->tm_min,struct_timer->tm_sec, szBuf);
	}
#else
	if(flag == WARNING)
	{
		fprintf(fp, "[%04d-%02d-%02d/%02d:%02d:%02d] [WARNING] %s",
			struct_timer->tm_year+1900,struct_timer->tm_mon+1,struct_timer->tm_mday,
			struct_timer->tm_hour,struct_timer->tm_min,struct_timer->tm_sec, szBuf);
	}
	else if(flag == CRITICAL)
	{
		fprintf(fp, "[%04d-%02d-%02d/%02d:%02d:%02d] [CRITICAL] %s",
			struct_timer->tm_year+1900,struct_timer->tm_mon+1,struct_timer->tm_mday,
			struct_timer->tm_hour,struct_timer->tm_min,struct_timer->tm_sec, szBuf);
	}
	else
	{
		fprintf(fp, "[%04d-%02d-%02d/%02d:%02d:%02d] %s",
			struct_timer->tm_year+1900,struct_timer->tm_mon+1,struct_timer->tm_mday,
			struct_timer->tm_hour,struct_timer->tm_min,struct_timer->tm_sec, szBuf);
	}
#endif


	fclose(fp);
}

void DataMsg(register char *buffer, int len, int flag)
{
	int i=0;

	time_t	current_time;
	struct	tm *struct_timer;	
	time(&current_time);
	struct_timer = localtime(&current_time);
	
	FILE* fp = fopen(LOG_FILE, "a");

	if(flag==RECV)
	{
		fprintf(fp, "[%04d-%02d-%02d/%02d:%02d:%02d] RECV(%d) ",
			struct_timer->tm_year+1900,struct_timer->tm_mon+1,struct_timer->tm_mday,
			struct_timer->tm_hour,struct_timer->tm_min,struct_timer->tm_sec, len);
	}
	else if(flag==SEND)
	{
		fprintf(fp, "[%04d-%02d-%02d/%02d:%02d:%02d] SEND(%d) ",
			struct_timer->tm_year+1900,struct_timer->tm_mon+1,struct_timer->tm_mday,
			struct_timer->tm_hour,struct_timer->tm_min,struct_timer->tm_sec, len);
	}

	for(i=0; i<len; i++)
	{
		fprintf(fp,"[%02X]",(unsigned char)buffer[i]);
	}
	fprintf(fp,"\n");

	fclose(fp);

}
void err_exit(int flag, const char *fmt, ... )
{
	char szBuf[1024] = {0,};

    va_list   ap;
    va_start (ap, fmt);
    vsnprintf(szBuf, 1023, fmt, ap);
    va_end   (ap);

	time_t	current_time;
	struct	tm *struct_timer;	
	time(&current_time);
	struct_timer = localtime(&current_time);

	FILE* fp = fopen(LOG_FILE, "a");

#ifdef COLOR
	if(flag == WARNING)
	{
		fprintf(fp, "[%04d-%02d-%02d/%02d:%02d:%02d] %s%s%s",
			struct_timer->tm_year+1900,struct_timer->tm_mon+1,struct_timer->tm_mday,
			struct_timer->tm_hour,struct_timer->tm_min,struct_timer->tm_sec, TERMINAL_COLOR_YELLOW, szBuf, TERMINAL_COLOR_RESET);
	}
	else if(flag == CRITICAL)
	{
		fprintf(fp, "[%04d-%02d-%02d/%02d:%02d:%02d] %s%s%s",
			struct_timer->tm_year+1900,struct_timer->tm_mon+1,struct_timer->tm_mday,
			struct_timer->tm_hour,struct_timer->tm_min,struct_timer->tm_sec, TERMINAL_COLOR_RED, szBuf, TERMINAL_COLOR_RESET);
	}
	else
	{
		fprintf(fp, "[%04d-%02d-%02d/%02d:%02d:%02d] %s",
			struct_timer->tm_year+1900,struct_timer->tm_mon+1,struct_timer->tm_mday,
			struct_timer->tm_hour,struct_timer->tm_min,struct_timer->tm_sec, szBuf);
	}
#else
	if(flag == WARNING)
	{
		fprintf(fp, "[%04d-%02d-%02d/%02d:%02d:%02d] [WARNING] %s",
			struct_timer->tm_year+1900,struct_timer->tm_mon+1,struct_timer->tm_mday,
			struct_timer->tm_hour,struct_timer->tm_min,struct_timer->tm_sec, szBuf);
	}
	else if(flag == CRITICAL)
	{
		fprintf(fp, "[%04d-%02d-%02d/%02d:%02d:%02d] [CRITICAL] %s",
			struct_timer->tm_year+1900,struct_timer->tm_mon+1,struct_timer->tm_mday,
			struct_timer->tm_hour,struct_timer->tm_min,struct_timer->tm_sec, szBuf);
	}
	else
	{
		fprintf(fp, "[%04d-%02d-%02d/%02d:%02d:%02d] %s",
			struct_timer->tm_year+1900,struct_timer->tm_mon+1,struct_timer->tm_mday,
			struct_timer->tm_hour,struct_timer->tm_min,struct_timer->tm_sec, szBuf);
	}
#endif

	fclose(fp);
	exit(1);
}
