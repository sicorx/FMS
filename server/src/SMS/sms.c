#include "sms.h"

char g_db_server[32];
char g_db_user[32];
char g_db_passwd[32];
char g_db_database[32];
char g_linux_user[32];
char process_name[32];

struct alarm_list *alarmList[MAX_ALARM_LIMIT_COUNT];
struct receiver_list *receiverList[MAX_RECEIVER_LIMIT_COUNT];

int euckr2utf8(char *source, char *dest, int dest_size)
{
    iconv_t it;
    char *pout;
    size_t in_size, out_size;
 
    it = iconv_open("UTF-8", "EUC-KR");
    in_size = strlen(source);
    out_size = dest_size;
    pout = dest;
    if (iconv(it, &source, &in_size, &pout, &out_size) < 0)
	{
        return(-1);
	}
    iconv_close(it);
    return(pout - dest);
    /* return(out_size); */
}

int utf82euckr(char *source, char *dest, int dest_size)
{
    iconv_t it;
    char *pout;
    size_t in_size, out_size;
 
    it = iconv_open("EUC-KR", "UTF-8");
    in_size = strlen(source);
    out_size = dest_size;
    pout = dest;
    if (iconv(it, &source, &in_size, &pout, &out_size) < 0)
        return(-1);
    iconv_close(it);
    return(pout - dest);
    /* return(out_size); */
}

void list_init(void)
{
	int i;	

	for(i=0; i<MAX_ALARM_LIMIT_COUNT; i++)
	{
		alarmList[i] = (struct alarm_list *)malloc(sizeof(struct alarm_list));
		memset(alarmList[i], 0x00, sizeof(struct alarm_list));
	}

	for(i=0; i<MAX_RECEIVER_LIMIT_COUNT; i++)
	{
		receiverList[i] = (struct receiver_list *)malloc(sizeof(struct receiver_list));
		memset(receiverList[i], 0x00, sizeof(struct receiver_list));
	}

}

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

int get_alarm_occur_list(void)
{
	MYSQL *conn;
	MYSQL_RES *result;
	MYSQL_ROW row;
	char query[1024];
	int i=0;

	conn = mysql_init(NULL);

	/* Connect to database */
	if (!mysql_real_connect(conn, g_db_server, g_db_user, g_db_passwd, g_db_database, 0, NULL, CLIENT_MULTI_RESULTS))
	{
		fileLog(WARNING, "[%s:%d] MYSQL connection error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
		mysql_close(conn);
		return -1;
	}

	sprintf(query,	" SELECT "
					" l.ALARM_SEQ, l.EQUIP_SEQ, l.TAG_SEQ, l.ALARM_GRADE_CD, l.CURR_VALUE, l.TH_VALUE_LOW, l.TH_VALUE_HIGH, l.SMS_YN, l.SMS_SEND_YN, l.SMS_SEND_R_YN, l.ALARM_STATUS, "
					" (SELECT NV_NM FROM TB_NAVIGATION WHERE NV_SEQ=(SELECT UP_NV_SEQ FROM TB_NAVIGATION WHERE NV_SEQ=e.NV_SEQ)) AS NV_NM, "
					" e.EQUIP_NM, t.TAG_NM, t.NORMAL_STR, t.ALARM_STR "
					" FROM "
					" TB_ALARM_LOG l LEFT JOIN "
					" TB_EQUIP e ON l.EQUIP_SEQ=e.EQUIP_SEQ LEFT JOIN "
					" TB_TAG t ON l.TAG_SEQ=t.TAG_SEQ "
					" WHERE l.SMS_YN='Y' AND l.SMS_SEND_YN='N' "
					" LIMIT %d; ", 
				MAX_ALARM_LIMIT_COUNT);
#ifdef DEBUG
	fileLog(INFO, "[%s:%d]\n", __FUNCTION__, __LINE__);
	//fileLog(INFO, "[%s:%d] %s\n", __FUNCTION__, __LINE__, query);
#endif

	if(mysql_query(conn, query) != 0)
	{
		fileLog(WARNING, "[%s:%d] MYSQL query error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
		mysql_close(conn);
		return -1;
	}

	do
	{
		result = mysql_store_result(conn);

		while(result && (row = mysql_fetch_row(result)) != NULL)
		{
			if(row[0] != NULL) 
			{
				alarmList[i]->aseq = atoi(row[0]);
#ifdef DEBUG
				fileLog(INFO, "aseq=[%d]\n", alarmList[i]->aseq);
#endif
			}

			if(row[1] != NULL) 
			{
				alarmList[i]->eseq = atoi(row[1]);
#ifdef DEBUG
				fileLog(INFO, "eseq=[%d]\n", alarmList[i]->eseq);
#endif
			}

			if(row[2] != NULL) 
			{
				alarmList[i]->tseq = atoi(row[2]);
#ifdef DEBUG
				fileLog(INFO, "tseq=[%d]\n", alarmList[i]->tseq);
#endif
			}

			if(row[3] != NULL) 
			{
				alarmList[i]->alarm_grade_cd = atoi(row[3]);
#ifdef DEBUG
				fileLog(INFO, "alarm_grade_cd=[%d]\n", alarmList[i]->alarm_grade_cd);
#endif
			}

			if(row[4] != NULL) 
			{
				alarmList[i]->curr_value = atof(row[4]);
#ifdef DEBUG
				fileLog(INFO, "curr_value=[%f]\n", alarmList[i]->curr_value);
#endif
			}

			if(row[5] != NULL) 
			{
				alarmList[i]->th_value_low = atof(row[5]);
#ifdef DEBUG
				fileLog(INFO, "th_value_low=[%f]\n", alarmList[i]->th_value_low);
#endif
			}

			if(row[6] != NULL) 
			{
				alarmList[i]->th_value_high = atof(row[6]);
#ifdef DEBUG
				fileLog(INFO, "th_value_high=[%f]\n", alarmList[i]->th_value_high);
#endif
			}

			if(row[7] != NULL) 
			{
				strcpy(alarmList[i]->sms_yn, row[7]);
#ifdef DEBUG
				fileLog(INFO, "sms_yn=[%s]\n", alarmList[i]->sms_yn);
#endif
			}

			if(row[8] != NULL) 
			{
				strcpy(alarmList[i]->sms_send_yn, row[8]);
#ifdef DEBUG
				fileLog(INFO, "sms_send_yn=[%s]\n", alarmList[i]->sms_send_yn);
#endif
			}

			if(row[9] != NULL) 
			{
				strcpy(alarmList[i]->sms_send_r_yn, row[9]);
#ifdef DEBUG
				fileLog(INFO, "sms_send_r_yn=[%s]\n", alarmList[i]->sms_send_r_yn);
#endif
			}

			if(row[10] != NULL) 
			{
				alarmList[i]->alarm_status = atoi(row[10]);
#ifdef DEBUG
				fileLog(INFO, "alarm_status=[%d]\n", alarmList[i]->alarm_status);
#endif
			}
			
			if(row[11] != NULL) 
			{
				strcpy(alarmList[i]->nv_nm, row[11]);
#ifdef DEBUG
				fileLog(INFO, "nv_nm=[%s]\n", alarmList[i]->nv_nm);
#endif
			}

			if(row[12] != NULL) 
			{
				strcpy(alarmList[i]->equip_nm, row[12]);
#ifdef DEBUG
				fileLog(INFO, "equip_nm=[%s]\n", alarmList[i]->equip_nm);
#endif
			}

			if(row[13] != NULL) 
			{
				strcpy(alarmList[i]->tag_nm, row[13]);
#ifdef DEBUG
				fileLog(INFO, "tag_nm=[%s]\n", alarmList[i]->tag_nm);
#endif
			}

			if(row[14] != NULL) 
			{
				strcpy(alarmList[i]->normal_str, row[14]);
#ifdef DEBUG
				fileLog(INFO, "normal_str=[%s]\n", alarmList[i]->normal_str);
#endif
			}

			if(row[15] != NULL) 
			{
				strcpy(alarmList[i]->alarm_str, row[15]);
#ifdef DEBUG
				fileLog(INFO, "alarm_str=[%s]\n", alarmList[i]->alarm_str);
#endif
			}
#ifdef DEBUG
			fileLog(INFO, "------------------------------------\n");
#endif
			i++;
		}

		if(result)
		{
			mysql_free_result(result);
		}

	}while(mysql_next_result(conn) == 0);

	mysql_close(conn);

	return i;
}


int get_alarm_clear_list(void)
{
	MYSQL *conn;
	MYSQL_RES *result;
	MYSQL_ROW row;
	char query[1024];
	int i=0;

	conn = mysql_init(NULL);

	/* Connect to database */
	if (!mysql_real_connect(conn, g_db_server, g_db_user, g_db_passwd, g_db_database, 0, NULL, CLIENT_MULTI_RESULTS))
	{
		fileLog(WARNING, "[%s:%d] MYSQL connection error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
		mysql_close(conn);
		return -1;
	}

	sprintf(query,	" SELECT "
					" l.ALARM_SEQ, l.EQUIP_SEQ, l.TAG_SEQ, l.ALARM_GRADE_CD, l.CURR_VALUE, l.TH_VALUE_LOW, l.TH_VALUE_HIGH, l.SMS_YN, l.SMS_SEND_YN, l.SMS_SEND_R_YN, l.ALARM_STATUS, "
					" (SELECT NV_NM FROM TB_NAVIGATION WHERE NV_SEQ=(SELECT UP_NV_SEQ FROM TB_NAVIGATION WHERE NV_SEQ=e.NV_SEQ)) AS NV_NM, "
					" e.EQUIP_NM, t.TAG_NM, t.NORMAL_STR, t.ALARM_STR "
					" FROM "
					" TB_ALARM_LOG l LEFT JOIN "
					" TB_EQUIP e ON l.EQUIP_SEQ=e.EQUIP_SEQ LEFT JOIN "
					" TB_TAG t ON l.TAG_SEQ=t.TAG_SEQ "
					" WHERE l.SMS_YN='Y' AND l.SMS_SEND_YN='Y' AND l.SMS_SEND_R_YN='N' AND l.REC_DT IS NOT NULL "
					" LIMIT %d; ", 
				MAX_ALARM_LIMIT_COUNT);
#ifdef DEBUG
	fileLog(INFO, "[%s:%d]\n", __FUNCTION__, __LINE__);
	//fileLog(INFO, "[%s:%d] %s\n", __FUNCTION__, __LINE__, query);
#endif

	if(mysql_query(conn, query) != 0)
	{
		fileLog(WARNING, "[%s:%d] MYSQL query error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
		mysql_close(conn);
		return -1;
	}

	do
	{
		result = mysql_store_result(conn);

		while(result && (row = mysql_fetch_row(result)) != NULL)
		{
			if(row[0] != NULL) 
			{
				alarmList[i]->aseq = atoi(row[0]);
#ifdef DEBUG
				fileLog(INFO, "aseq=[%d]\n", alarmList[i]->aseq);
#endif
			}

			if(row[1] != NULL) 
			{
				alarmList[i]->eseq = atoi(row[1]);
#ifdef DEBUG
				fileLog(INFO, "eseq=[%d]\n", alarmList[i]->eseq);
#endif
			}

			if(row[2] != NULL) 
			{
				alarmList[i]->tseq = atoi(row[2]);
#ifdef DEBUG
				fileLog(INFO, "tseq=[%d]\n", alarmList[i]->tseq);
#endif
			}

			if(row[3] != NULL) 
			{
				alarmList[i]->alarm_grade_cd = atoi(row[3]);
#ifdef DEBUG
				fileLog(INFO, "alarm_grade_cd=[%d]\n", alarmList[i]->alarm_grade_cd);
#endif
			}

			if(row[4] != NULL) 
			{
				alarmList[i]->curr_value = atof(row[4]);
#ifdef DEBUG
				fileLog(INFO, "curr_value=[%f]\n", alarmList[i]->curr_value);
#endif
			}

			if(row[5] != NULL) 
			{
				alarmList[i]->th_value_low = atof(row[5]);
#ifdef DEBUG
				fileLog(INFO, "th_value_low=[%f]\n", alarmList[i]->th_value_low);
#endif
			}

			if(row[6] != NULL) 
			{
				alarmList[i]->th_value_high = atof(row[6]);
#ifdef DEBUG
				fileLog(INFO, "th_value_high=[%f]\n", alarmList[i]->th_value_high);
#endif
			}

			if(row[7] != NULL) 
			{
				strcpy(alarmList[i]->sms_yn, row[7]);
#ifdef DEBUG
				fileLog(INFO, "sms_yn=[%s]\n", alarmList[i]->sms_yn);
#endif
			}

			if(row[8] != NULL) 
			{
				strcpy(alarmList[i]->sms_send_yn, row[8]);
#ifdef DEBUG
				fileLog(INFO, "sms_send_yn=[%s]\n", alarmList[i]->sms_send_yn);
#endif
			}

			if(row[9] != NULL) 
			{
				strcpy(alarmList[i]->sms_send_r_yn, row[9]);
#ifdef DEBUG
				fileLog(INFO, "sms_send_r_yn=[%s]\n", alarmList[i]->sms_send_r_yn);
#endif
			}

			if(row[10] != NULL) 
			{
				alarmList[i]->alarm_status = atoi(row[10]);
#ifdef DEBUG
				fileLog(INFO, "alarm_status=[%d]\n", alarmList[i]->alarm_status);
#endif
			}
			
			if(row[11] != NULL) 
			{
				strcpy(alarmList[i]->nv_nm, row[11]);
#ifdef DEBUG
				fileLog(INFO, "nv_nm=[%s]\n", alarmList[i]->nv_nm);
#endif
			}

			if(row[12] != NULL) 
			{
				strcpy(alarmList[i]->equip_nm, row[12]);
#ifdef DEBUG
				fileLog(INFO, "equip_nm=[%s]\n", alarmList[i]->equip_nm);
#endif
			}

			if(row[13] != NULL) 
			{
				strcpy(alarmList[i]->tag_nm, row[13]);
#ifdef DEBUG
				fileLog(INFO, "tag_nm=[%s]\n", alarmList[i]->tag_nm);
#endif
			}

			if(row[14] != NULL) 
			{
				strcpy(alarmList[i]->normal_str, row[14]);
#ifdef DEBUG
				fileLog(INFO, "normal_str=[%s]\n", alarmList[i]->normal_str);
#endif
			}

			if(row[15] != NULL) 
			{
				strcpy(alarmList[i]->alarm_str, row[15]);
#ifdef DEBUG
				fileLog(INFO, "alarm_str=[%s]\n", alarmList[i]->alarm_str);
#endif
			}
#ifdef DEBUG
			fileLog(INFO, "------------------------------------\n");
#endif
			i++;
		}

		if(result)
		{
			mysql_free_result(result);
		}

	}while(mysql_next_result(conn) == 0);

	mysql_close(conn);

	return i;
}

int get_receiver_list(int idx)
{
	MYSQL *conn;
	MYSQL_RES *result;
	MYSQL_ROW row;
	char query[1024];
	int i=0;

	conn = mysql_init(NULL);

	/* Connect to database */
	if (!mysql_real_connect(conn, g_db_server, g_db_user, g_db_passwd, g_db_database, 0, NULL, CLIENT_MULTI_RESULTS))
	{
		fileLog(WARNING, "[%s:%d] MYSQL connection error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
		mysql_close(conn);
		return -1;
	}

	sprintf(query, " SELECT USER_ID, USER_NM, REPLACE(USER_TEL, '-', '') as user_tel FROM TB_USER_TAG t LEFT JOIN TB_USER u USING (USER_ID) WHERE u.USE_YN='Y' AND u.SMS_YN='Y' AND t.TAG_SEQ=%d AND t.ALARM_GRADE_SEQ=%d; ", alarmList[i]->tseq, alarmList[idx]->alarm_grade_cd);
#ifdef DEBUG
	fileLog(INFO, "[%s:%d] %s\n", __FUNCTION__, __LINE__, query);
#endif

	if(mysql_query(conn, query) != 0)
	{
		fileLog(WARNING, "[%s:%d] MYSQL query error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
		mysql_close(conn);
		return -1;
	}

	do
	{
		result = mysql_store_result(conn);

		while(result && (row = mysql_fetch_row(result)) != NULL)
		{
			if(row[0] != NULL)
			{
				strcpy(receiverList[i]->user_id, row[0]);
#ifdef DEBUG
				fileLog(INFO, "receiverList[%d]->user_id=[%s]\n", i, receiverList[i]->user_id);
#endif
			}
			
			if(row[1] != NULL)
			{
				strcpy(receiverList[i]->user_nm, row[1]);
#ifdef DEBUG
				fileLog(INFO, "receiverList[%d]->user_nm=[%s]\n", i, receiverList[i]->user_nm);
#endif
			}
			
			if(row[2] != NULL)
			{
				strcpy(receiverList[i]->user_tel, row[2]);
#ifdef DEBUG
				fileLog(INFO, "receiverList[%d]->user_tel=[%s]\n", i, receiverList[i]->user_tel);
#endif
			}

			i++;
		}

		if(result)
		{
			mysql_free_result(result);
		}

	}while(mysql_next_result(conn) == 0);

	mysql_close(conn);

	return i;
}


int mysql_con_init(void)
{
	MYSQL *conn;

	conn = mysql_init(NULL);

	/* Connect to database */
	if (!mysql_real_connect(conn, g_db_server, g_db_user, g_db_passwd, g_db_database, 0, NULL, CLIENT_MULTI_RESULTS))
	{
		fileLog(WARNING, "[%s:%d] MYSQL connection error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
		mysql_close(conn);
		return -1;
	}
#ifdef DEBUG
	fileLog(INFO, "[%s:%d] MYSQL connection(HOST:%s, USER:%s, PSWD:%s, DBName:%s) success]\n", __FUNCTION__, __LINE__, g_db_server, g_db_user, g_db_passwd, g_db_database);
#endif
	mysql_close(conn);
	return 1;
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


void space_rtrim(char *value)
{
    int j;

    for(j = strlen(value) - 1; ; j--)
    {
        if( value[j] == 0x20 || value[j] == '\n' ||
            value[j] == 0x09 || value[j] == 0x0D)   /* 0x0D = CR */
        {
            value[j] = 0x00;
        }
        else
        {
            break;
        }
    }
}

int get_key_value(const char *filename, const char *key, char *value)
{
    int i = 0, len, rvalue = -1;
    char buf[256] = {0x00, };
    FILE *fp = NULL;

    if((len = strlen(key)) == 0 || key == NULL)
    {
        fileLog(CRITICAL, "Key is Empty\n");
        return -1;
    }

    if(value == NULL)
    {
        fileLog(CRITICAL, "Buffer Error\n");
        return -1;
    }

    if((fp = fopen(filename, "r")) == NULL)
    {	
        fileLog(CRITICAL, "Filename=[%s] open error!! (%s)\n", filename, (char *)strerror(errno));
        return -1;
    }

    while(fgets(buf, sizeof(buf), fp) != NULL)
    {
        if(buf[0] == '\n' || buf[0] == '#')
        {
            continue;
        }
        else if(!strncmp(key, buf, len))
        {
            for(i = len; i < (int)strlen(buf); i++)
            {
                /* 0x09[TAB] 0x20[SPACE] */
                if(buf[i] == 0x09 || buf[i] == 0x20 || buf[i] == '=')
                {
                    continue;
                }
                break;
            }
            strncpy(value, buf+i, strlen(buf) - i -1);
            rvalue = 0;
            break;
        }
    }

    if(fclose(fp) != 0)
    {
        fileLog(CRITICAL, "file(%s) close error!! (%s)\n", filename, (char *)strerror(errno));
    }

    /* trim trailing space */
    space_rtrim(value);

    //fflush(stderr);

    return rvalue;
}

void get_conf(void)
{
    if(get_key_value(CONF_FILE, "DB_SERVER", g_db_server) == -1)		err_exit(CRITICAL, "Can't Find DB_SERVER FILE_FULLPATH=[%s]\n", CONF_FILE);
    if(get_key_value(CONF_FILE, "DB_USER", g_db_user) == -1)			err_exit(CRITICAL, "Can't Find DB_USER FILE_FULLPATH=[%s]\n", CONF_FILE);
    if(get_key_value(CONF_FILE, "DB_PASSWD", g_db_passwd) == -1)		err_exit(CRITICAL, "Can't Find DB_PASSWD FILE_FULLPATH=[%s]\n", CONF_FILE);
	if(get_key_value(CONF_FILE, "DB_DATABASE", g_db_database) == -1)	err_exit(CRITICAL, "Can't Find DB_DATABASE FILE_FULLPATH=[%s]\n", CONF_FILE);
	if(get_key_value(CONF_FILE, "LINUX_USER", g_linux_user) == -1)		err_exit(CRITICAL, "Can't Find LINUX_USER FILE_FULLPATH=[%s]\n", CONF_FILE);
		
#ifdef DEBUG
	fileLog(CRITICAL, "DB_USER=[%s]\n",g_db_user);
	fileLog(CRITICAL, "DB_PASSWD=[%s]\n",g_db_passwd);
	fileLog(CRITICAL, "DB_DATABASE=[%s]\n",g_db_database);
	fileLog(CRITICAL, "LINUX_USER=[%s]\n",g_linux_user);
	fileLog(CRITICAL, "DB_SERVER=[%s]\n",g_db_server);
#endif

}

char* fillSpace(char text[], int fullSize)
{
	int diff = 0;
	int i;
	diff = fullSize - strlen(text);

	char *ret;
	ret = (char *)malloc(fullSize);
	memset(ret, 0, fullSize);

	strcat(ret, text);

	for(i=0; i<diff; i++)
	{
		strcat(ret, " ");
	}

	return ret;
}

char* send_sms_format(char sendStr[], int i, int j)
{
	char msg[160];
	char *ret;
	char trans[160];
	int k = 0;

	time_t	current_time;
	struct	tm *struct_timer;	
	time(&current_time);
	struct_timer = localtime(&current_time);
	char dt[64];
	char curr[32];

	// 수신메세지 작성
	memset(msg, 0, sizeof(msg));
	memset(dt, 0, sizeof(dt));
	memset(curr, 0, sizeof(curr));

	strcat(msg, "NEW/");
	strcat(msg, alarmList[i]->nv_nm);
	strcat(msg, "/");
	strcat(msg, alarmList[i]->equip_nm);
	strcat(msg, "/");
	strcat(msg, alarmList[i]->tag_nm);
	if(alarmList[i]->tseq == 15912 || alarmList[i]->tseq == 15915 || alarmList[i]->tseq == 15918 ||alarmList[i]->tseq == 15921 ||alarmList[i]->tseq == 15924 ||alarmList[i]->tseq == 15927 ||alarmList[i]->tseq == 15930|| alarmList[i]->tseq==16626)
	{
		sprintf(curr, " %.2f", alarmList[i]->curr_value);
		strcat(msg, curr);
	}
	strcat(msg, "/");
	alarmList[i]->alarm_status == 0 ? strcat(msg, alarmList[i]->alarm_str) : strcat(msg, alarmList[i]->normal_str);
	strcat(msg, "/");
	sprintf(dt, "%04d-%02d-%02d/%02d:%02d:%02d",
			struct_timer->tm_year+1900, struct_timer->tm_mon+1, struct_timer->tm_mday,
			struct_timer->tm_hour, struct_timer->tm_min, struct_timer->tm_sec);
	strcat(msg, dt);
	msg[strlen(msg)] = 0;
	for(k=strlen(msg); k < 160; k++)
	{
		msg[k] = 0;
	}

	// 문자 포맷 작성
	ret = fillSpace(receiverList[j]->user_tel, 12);
	strcpy(sendStr, ret);							//수신 전화번호
	free(ret);

	utf82euckr(msg, trans, sizeof(trans));
	ret = fillSpace(trans, 160);
	strcat(sendStr, ret);							//수신 메세지
	free(ret);

	ret = fillSpace(RESERVED_TIME, 2);
	strcat(sendStr, ret);							//발송 예약시간
	free(ret);

	ret = fillSpace("", 3);
	strcat(sendStr, ret);							//LMS Prefix
	free(ret);

	ret = fillSpace("O",1);
	strcat(sendStr, ret);							//온라인/배치 구분
	free(ret);

	ret = fillSpace("", 12);
	strcat(sendStr, ret);							//회신번호
	free(ret);

	ret = fillSpace("Z0000",5);
	strcat(sendStr, ret);							//발송 사유코드
	free(ret);

	ret = fillSpace("", 5);
	strcat(sendStr, ret);							//어플리케이션 구분
	free(ret);

	ret = fillSpace("S",1);
	strcat(sendStr, ret);							//발송메세지 형식 (Short/Long/Wap)
	free(ret);

	ret = fillSpace(SMS_LO, 1);
	strcat(sendStr, ret);							//자사/타사 구분(L/O)
	free(ret);

	ret = fillSpace("", 15);
	strcat(sendStr, ret);							//발송자 User ID
	free(ret);

	ret = fillSpace("", 15);
	strcat(sendStr, ret);							//어플리케이션 ID
	free(ret);

	ret = fillSpace("", 80);
	strcat(sendStr, ret);							//WAP PUSH URL
	free(ret);

	ret = fillSpace("4098", 5);
	strcat(sendStr, ret);							//Source TID
	free(ret);
	
	ret = fillSpace("", 43);
	strcat(sendStr, ret);							//Reserved
	free(ret);

	return sendStr;
}

int sendToSmsServer(char smsStr[])
{
	int client_fd;
	struct sockaddr_in client_addr;

	client_fd = socket(PF_INET, SOCK_STREAM, 0);

	client_addr.sin_addr.s_addr = inet_addr(SMS_SERVER_IP);
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(SMS_PORT);

	if(connect(client_fd, (struct sockaddr *)&client_addr, sizeof(client_addr)) == -1) //서버 연결 실패
	{
		fileLog(CRITICAL, "[%s:%d] Fail : Connect to Sms Server\n", __FUNCTION__, __LINE__);
		return -1;
	}
	else //서버 연결 성공
	{
		if(send(client_fd, smsStr, strlen(smsStr), 0) == -1) //Send 실패
		{
			close(client_fd);
			fileLog(WARNING, "[%s:%d] FAIL : Send to Sms Server\n", __FUNCTION__, __LINE__);
			return -1;
		}
		else //Send 성공
		{
#ifdef DEBUG
			fileLog(WARNING, "[%s:%d] smsStr=[%s]\n", __FUNCTION__, __LINE__, smsStr);
			fileLog(WARNING, "------------------------------------------------\n");
#endif
		}
	}
	close(client_fd);
	return 1;
}

int updateAlarmOccurStatus(int aseq)
{
	MYSQL *conn;
	char query[1024];

	conn = mysql_init(NULL);

	/* Connect to database */
	if (!mysql_real_connect(conn, g_db_server, g_db_user, g_db_passwd, g_db_database, 0, NULL, CLIENT_MULTI_RESULTS))
	{
		fileLog(CRITICAL, "[%s:%d] MYSQL connection error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
		mysql_close(conn);
		return -1;
	}

	sprintf(query,	" UPDATE TB_ALARM_LOG SET SMS_SEND_YN='Y' WHERE ALARM_SEQ=%d; ", aseq);
#ifdef DEBUG
	fileLog(INFO, "[%s:%d] %s\n", __FUNCTION__, __LINE__, query);
#endif

	if(mysql_query(conn, query) != 0)
	{
		fileLog(CRITICAL, "[%s:%d] MYSQL query error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
		mysql_close(conn);
		return -1;
	}

	return 1;	
}


int updateAlarmClearStatus(int aseq)
{
	MYSQL *conn;
	char query[1024];

	conn = mysql_init(NULL);

	/* Connect to database */
	if (!mysql_real_connect(conn, g_db_server, g_db_user, g_db_passwd, g_db_database, 0, NULL, CLIENT_MULTI_RESULTS))
	{
		fileLog(CRITICAL, "[%s:%d] MYSQL connection error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
		mysql_close(conn);
		return -1;
	}

	sprintf(query,	" UPDATE TB_ALARM_LOG SET SMS_SEND_R_YN='Y' WHERE ALARM_SEQ=%d; ", aseq);
#ifdef DEBUG
	fileLog(INFO, "[%s:%d] %s\n", __FUNCTION__, __LINE__, query);
#endif

	if(mysql_query(conn, query) != 0)
	{
		fileLog(CRITICAL, "[%s:%d] MYSQL query error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
		mysql_close(conn);
		return -1;
	}

	return 1;	
}

int main(int argc, char** argv)
{
	char *temp = NULL;
	int i=0, j=0;
	int status=0;
	int ret = 0, rcv_cnt=0;
	char sendStr[BUF_LEN];
	char smsStr[BUF_LEN];

	memset(process_name, 0x00, sizeof(process_name));
	temp = strdup(argv[0]);
	strcpy(process_name, (char *)basename(temp));
	free(temp);


    /*init_signal();*/


	print_start_log();
	
	get_conf();
	mysql_con_init();
	list_init();

	while(1)
	{
		ret = get_alarm_occur_list(); //알람 조회		
		if(ret > 0)
		{
			for(i=0; i<ret; i++)
			{
				rcv_cnt = get_receiver_list(i);	//수신자 조회
				if(rcv_cnt > 0)
				{
					for(j=0; j<rcv_cnt; j++)
					{					
						memset(sendStr, 0, sizeof(sendStr));
						strcpy(smsStr, send_sms_format(sendStr, i, j));
						
						status = sendToSmsServer(smsStr);					
					}
					if(status == 1) updateAlarmOccurStatus(alarmList[i]->aseq);
				}
				else
				{
					updateAlarmOccurStatus(alarmList[i]->aseq);
				}
			}	
		}

		usleep(500000);
		
		ret = get_alarm_clear_list(); //알람 조회		
		if(ret > 0)
		{
			for(i=0; i<ret; i++)
			{
				rcv_cnt = get_receiver_list(i);	//수신자 조회
				if(rcv_cnt > 0)
				{
					for(j=0; j<rcv_cnt; j++)
					{					
						memset(sendStr, 0, sizeof(sendStr));
						strcpy(smsStr, send_sms_format(sendStr, i, j));
						
						status = sendToSmsServer(smsStr);					
					}			
					if(status == 1) updateAlarmClearStatus(alarmList[i]->aseq);
				}
				else
				{
					updateAlarmOccurStatus(alarmList[i]->aseq);
				}
			}	
		}
		usleep(500000);

	}

	print_end_log();
	return 1;
}