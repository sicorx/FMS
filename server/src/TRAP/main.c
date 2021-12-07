#include "main.h"

/*
struct snmptrapv2
{
	unsigned char	version;
	unsigned char	community[16];
	
	unsigned int	requestId;
	unsigned char	errorStatus;
	unsigned char	errorIndex;
}
*/

char g_db_server[32];
char g_db_user[32];
char g_db_passwd[32];
char g_db_database[32];
char g_linux_user[32];
char process_name[32];

struct key_value
{
	unsigned char ip[16];
	unsigned char key[32];
	unsigned char value[32];
	unsigned char trap[64];

	unsigned int alarm;
	unsigned char version;
};

struct dbData
{
	unsigned int	eseq;
	unsigned int	tseq;
	unsigned char	equip_nm[32];
	unsigned char	tag_nm[32];
	unsigned char	tag_id[64];
	unsigned char	e_alarm_yn[4];
	unsigned char	e_sms_yn[4];
	unsigned char	e_use_yn[4];	
	unsigned char	t_alarm_yn[4];
	unsigned char	t_sms_yn[4];
	unsigned char	t_use_yn[4];	
	unsigned char	alarm_yn[4];
	unsigned char	sms_yn[4];
	unsigned int	trap_on;
	unsigned int	trap_off;

};


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
	else if(flag == DATA)
	{
		fprintf(fp, "%s", szBuf);
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
	else if(flag == DATA)
	{
		fprintf(fp, "%s", szBuf);
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

void alarm_insert(struct key_value p1, struct dbData d1)
{
	int ret;
	int query_state;
	char query[1024];
	MYSQL *conn;

	conn = mysql_init(NULL);

	/* Connect to database */
	if (!mysql_real_connect(conn, g_db_server, g_db_user, g_db_passwd, g_db_database, 0, NULL, CLIENT_MULTI_STATEMENTS)) 
	{
		fileLog(WARNING, "[%s:%d] MYSQL connection error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
		ret = -1;
	}

	if(p1.alarm == d1.trap_on)
	{
		fileLog(WARNING, "TRAP ON\n");
		sprintf(query,  " INSERT INTO `TB_ALARM_LOG` "
						" (EQUIP_SEQ, TAG_SEQ, ALARM_GRADE_CD, CURR_VALUE, TH_VALUE_LOW, TH_VALUE_HIGH, SMS_YN, SMS_SEND_YN, SMS_SEND_R_YN) "
						" VALUES ('%d', '%d', 3, 0, 0, 0, 'Y', 'N', 'N'); ", d1.eseq, d1.tseq );
	}
	else if(p1.alarm == d1.trap_off)
	{
		fileLog(WARNING, "TRAP OFF\n");
		//클리어 작업이랑 사용 미사용 작업 필요
		sprintf(query,  "UPDATE TB_ALARM_LOG SET "
						"ALARM_STATUS=1, "
						"REC_DT=now() "
						"WHERE "
						"EQUIP_SEQ=%d AND TAG_SEQ=%d AND REC_DT IS NULL;",
						d1.eseq, d1.tseq
						);
	}
	else
	{
		fileLog(WARNING, "DON'T MATCHING TRAP ALARM!\n");
	}
	
	fileLog(WARNING, "[%s:%d] %s\n", __FUNCTION__, __LINE__, query); 

	query_state = mysql_query(conn, query);
	if (query_state != 0)
	{
		fileLog(WARNING, "[%s:%d] MYSQL query error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
		ret = -1;
	}

	mysql_close(conn);

}
int dbCheck(struct key_value p1)
{
	struct dbData d1;

	fileLog(CRITICAL, "[%s:%d] IP=[%s]\n", __FUNCTION__, __LINE__,p1.ip);
	fileLog(CRITICAL, "[%s:%d] version=[%d]\n", __FUNCTION__, __LINE__, p1.version);
	fileLog(CRITICAL, "[%s:%d] alarm=[%d]\n", __FUNCTION__, __LINE__, p1.alarm);
	fileLog(CRITICAL, "[%s:%d] TRAP=[%s]\n", __FUNCTION__, __LINE__, p1.trap);

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
		return -1;
	}

	sprintf(query, " SELECT "
				   " e.EQUIP_SEQ, t.TAG_SEQ, e.EQUIP_NM, t.TAG_NM, t.TAG_ID, "
				   " e.ALARM_YN E_ALARM_YN, e.SMS_YN E_SMS_YN, e.USE_YN E_USE_YN, "
				   " t.ALARM_YN T_ALARM_YN, t.SMS_YN T_SMS_YN, t.USE_YN E_USE_YN, "
				   " t.CRITICAL_ALARM_YN, t.CRITICAL_SMS_YN, "
				   " t.TRAP_ON, t.TRAP_OFF "
				   " FROM TB_EQUIP e LEFT JOIN TB_TAG t using(EQUIP_SEQ) "
				   " WHERE e.EQUIP_GET_IP='%s' AND TAG_ID='%s' ", 
				   p1.ip, p1.trap );

	fileLog(INFO, "[%s:%d] %s\n", __FUNCTION__, __LINE__, query);

	if(mysql_query(conn, query) != 0)
	{
		fileLog(WARNING, "[%s:%d] MYSQL query error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
		return -1;
	}

	do
	{
		result = mysql_store_result(conn);

		while(result && (row = mysql_fetch_row(result)) != NULL)
		{
			if(row[0] != NULL) d1.eseq = atoi(row[0]);
			if(row[1] != NULL) d1.tseq = atoi(row[1]);

			if(row[2] != NULL) strcpy(d1.equip_nm, row[2]);
			if(row[3] != NULL) strcpy(d1.tag_nm, row[3]);
			if(row[4] != NULL) strcpy(d1.tag_id, row[4]);
			
			if(row[5] != NULL) strcpy(d1.e_alarm_yn, row[5]);
			if(row[6] != NULL) strcpy(d1.e_sms_yn, row[6]);
			if(row[7] != NULL) strcpy(d1.e_use_yn, row[7]);
			
			if(row[8] != NULL) strcpy(d1.t_alarm_yn, row[8]);
			if(row[9] != NULL) strcpy(d1.t_sms_yn, row[9]);
			if(row[10] != NULL) strcpy(d1.t_use_yn, row[10]);
			
			if(row[11] != NULL) strcpy(d1.alarm_yn, row[11]);
			if(row[12] != NULL) strcpy(d1.sms_yn, row[12]);

			if(row[13] != NULL) d1.trap_on = atoi(row[13]);
			if(row[14] != NULL) d1.trap_off = atoi(row[14]);
			
			i++;
		}

		if(result)
		{
			mysql_free_result(result);
		}

	}while(mysql_next_result(conn) == 0);

	mysql_close(conn);

	if(i>0)
	{
		fileLog(INFO, "d1.eseq=[%d]\n", d1.eseq);
		fileLog(INFO, "d1.tseq=[%d]\n", d1.tseq);	
		fileLog(INFO, "d1.equip_nm=[%s]\n", d1.equip_nm);
		fileLog(INFO, "d1.tag_nm=[%s]\n", d1.tag_nm);
		fileLog(INFO, "d1.tag_id=[%s]\n", d1.tag_id);
		fileLog(INFO, "d1.e_alarm_yn=[%s]\n", d1.e_alarm_yn);
		fileLog(INFO, "d1.e_sms_yn=[%s]\n", d1.e_sms_yn);
		fileLog(INFO, "d1.e_use_yn=[%s]\n", d1.e_use_yn);
		fileLog(INFO, "d1.t_alarm_yn=[%s]\n", d1.t_alarm_yn);
		fileLog(INFO, "d1.t_sms_yn=[%s]\n", d1.t_sms_yn);
		fileLog(INFO, "d1.t_use_yn=[%s]\n", d1.t_use_yn);
		fileLog(INFO, "d1.alarm_yn=[%s]\n", d1.alarm_yn);
		fileLog(INFO, "d1.sms_yn=[%s]\n", d1.sms_yn);
		fileLog(INFO, "d1.trap_on=[%d]\n", d1.trap_on);
		fileLog(INFO, "d1.trap_off=[%d]\n", d1.trap_off);

		alarm_insert(p1, d1);

	}
	else
	{
		fileLog(INFO, "DON'T MATCHING TRAP OID!\n");
	}

	return 1;

}
void snmpv1_parsing(unsigned char real[], int length, struct key_value p1)
{
	int i, j;

	int key_length = 0;
	int val_length = 0;
	//struct key_value p1;
	
	unsigned char alarm[32] = { 0x06, 0x0A, 0x2B, 0x06, 0x01, 0x06, 0x03, 0x01, 0x01, 0x04, 0x01, 0x00, };
	unsigned char trap[32] = { 0x00, };
	unsigned char temp[64] = { 0x00, };

	fileLog(INFO, "[%s:%d] MESSAGE\n", __FUNCTION__, __LINE__);
	for(i=0; i<length; i++)
	{
		fileLog(DATA, "%02X ", real[i]);
	}
	fileLog(DATA, "\n");

	fileLog(CRITICAL, "[%s:%d] IP=[%s]\n", __FUNCTION__, __LINE__,p1.ip);
	fileLog(CRITICAL, "[%s:%d] version=[%d]\n", __FUNCTION__, __LINE__, p1.version);
	fileLog(CRITICAL, "[%s:%d] alarm=[%d]\n", __FUNCTION__, __LINE__, p1.alarm);
	fileLog(CRITICAL, "[%s:%d] TRAP=[%s]\n", __FUNCTION__, __LINE__, p1.trap);

	//memset(p1.alarm, 0, sizeof(p1.alarm));
	memset(p1.trap, 0, sizeof(p1.trap));


	for(i=0; i<length; i++)
	{
		switch (real[i])
		{
			case 0x30 : i += 1; break;

			case 0x02 :			//int
			case 0x04 :			//string
			case 0x42 : 		//oid
			case 0x43 :			//timetick
				i += 1 + real[i+1];
				break;

			case 0x06 : 		//oid			

				for(j=2; j < real[i+1]+2; j++)
				{
					if(real[i+j]==0x2B)
					{
						sprintf(p1.trap, ".1.3");
					}
					else
					{
						sprintf(temp, ".%d", real[i+j]);
						strcat(p1.trap, temp);
					}
				}
				dbCheck(p1);

				i += 1 + real[i+1];

				break;
		}
	}

}

void snmpv2_parsing(unsigned char real[], int length, struct key_value p1)
{
	int i, j, k=0;

	int key_length = 0;
	int val_length = 0;
	//struct key_value p1;
	
	unsigned char alarm[32] = { 0x06, 0x0A, 0x2B, 0x06, 0x01, 0x06, 0x03, 0x01, 0x01, 0x04, 0x01, 0x00, };
	unsigned char trap[32] = { 0x00, };
	unsigned char temp[64] = { 0x00, };

	fileLog(INFO, "[%s:%d] MESSAGE\n", __FUNCTION__, __LINE__);
	for(i=0; i<length; i++)
	{
		fileLog(DATA, "%02X ", real[i]);
	}
	fileLog(DATA, "\n");

	fileLog(CRITICAL, "[%s:%d] IP=[%s]\n", __FUNCTION__, __LINE__,p1.ip);
	fileLog(CRITICAL, "[%s:%d] version=[%d]\n", __FUNCTION__, __LINE__, p1.version);
	fileLog(CRITICAL, "[%s:%d] alarm=[%d]\n", __FUNCTION__, __LINE__, p1.alarm);
	fileLog(CRITICAL, "[%s:%d] TRAP=[%s]\n", __FUNCTION__, __LINE__, p1.trap);

	//memset(p1.alarm, 0, sizeof(p1.alarm));
	memset(p1.trap, 0, sizeof(p1.trap));


	for(i=0; i<length; i++)
	{
		switch (real[i])
		{
			case 0x30 : i += 1; break;

			case 0x02 :			//int
			case 0x04 :			//string
			case 0x06 : 		//oid
			case 0x42 : 		//oid
			case 0x43 :			//timetick			

				if(k % 2 == 0)
				{
					memset(p1.key, 0, sizeof(p1.key));
					memset(p1.value, 0, sizeof(p1.value));
					key_length = real[i+1]+2;
					memcpy(p1.key, &real[i], key_length);					
				}
				else
				{
					val_length = real[i+1]+2;
					memcpy(p1.value, &real[i], val_length);					
				}			
				
				if(p1.value[0]==0x06)
				{
					if(memcmp(p1.key, alarm, key_length) == 0)
					{
						p1.alarm = p1.value[val_length-1];
					}
					else
					{
						for(j=2; j < val_length; j++)
						{
							if(real[i+j]==0x2B)
							{
								sprintf(p1.trap, ".1.3");
							}
							else
							{
								sprintf(temp, ".%d", real[i+j]);
								strcat(p1.trap, temp);
							}
						}
					}
				}

				i += 1 + real[i+1];

				k++;
				break;
		}
	}
	dbCheck(p1);
}

void snmptrap_parsing(unsigned char buf[], int nbyte, struct key_value p1)
{
	int i, k;
	int j = 0;
	unsigned char real[1024];
	unsigned int length = 0;

	fileLog(INFO, "[%s:%d]ORG MESSAGE\n", __FUNCTION__, __LINE__);
	for(i=0; i<nbyte; i++)
	{
		fileLog(DATA, "%02X ", buf[i]);
	}
	fileLog(DATA, "\n");
		
	fileLog(CRITICAL, "[%s:%d] IP=[%s]\n", __FUNCTION__, __LINE__,p1.ip);
	fileLog(CRITICAL, "[%s:%d] version=[%d]\n", __FUNCTION__, __LINE__, p1.version);
	fileLog(CRITICAL, "[%s:%d] alarm=[%d]\n", __FUNCTION__, __LINE__, p1.alarm);
	fileLog(CRITICAL, "[%s:%d] TRAP=[%s]\n", __FUNCTION__, __LINE__, p1.trap);
	k=0;
	memset(real, 0, sizeof(real));
	for(i=0; i<nbyte; i++)
	{
		if(k == 2)
		{
			if(buf[i-1]==0x81)
			{
				length = buf[i];
				memcpy(real, &buf[i+1], length);
			}
			else
			{
				length = buf[i-1];
				memcpy(real, &buf[i], length);
			}


			break;
		}
		else
		{
			switch (buf[i])
			{
				case 0x30 : k += 1;
				case 0xA4 :
				case 0xA7 : i += 1; break;

				case 0x02 :			//int
				{
					if(j==0)
					{
						p1.version = buf[i+2];
					}
					else if(j==1)
					{
						p1.alarm = buf[i+2];
					}
					else if(j==2)
					{
						if(p1.alarm==0x06) p1.alarm = buf[i+2];
					}
				}
				j++;

				case 0x04 :			//string
				case 0x06 : 		//oid
				case 0x40 : 		//ip addr
				case 0x43 :			//timetick
					i += 1 + buf[i+1];
					break;
			}
		}
	}

	if(p1.version == VERSION2)
	{
		snmpv2_parsing(&real, length, p1);
	}
	else if(p1.version == VERSION1)
	{
		snmpv1_parsing(&real, length, p1);
	}

}

int main(int argc, char *argv[]) 
{
    struct sockaddr_in servaddr, cliaddr;
    int s, nbyte, addrlen = sizeof(struct sockaddr);
    unsigned char buf[MAXLINE+1];
	struct key_value p1;
	
	get_conf();
	mysql_con_init();

    //소켓 생성
    if((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) 
	{
        fileLog(INFO, "socket fail");
        exit(0);
    }
    
    // 서버 구조
    memset(&cliaddr, 0, addrlen);
    memset(&servaddr, 0, addrlen);

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SNMP_SOCK_PORT);

    // 서버 로컬 주소로 bind()
    if(bind(s, (struct sockaddr *)&servaddr, addrlen) < 0) 
	{
        fileLog(INFO, "bind fail");
        exit(0);
    }
    

    while(1)
    {
		memset(&buf, 0, MAXLINE+1);

		fileLog(INFO, "Wait Request\n");

         //전송 받은 메시지 nbyte 저장
        nbyte = recvfrom(s, buf, MAXLINE , 0, (struct sockaddr *)&cliaddr, &addrlen);
        if(nbyte< 0) 
		{
            fileLog(INFO, "recvfrom fail");
            exit(1);
        }

        buf[nbyte] = 0; //마지막 값에 0
	
        if(!strncmp(buf, "end of file", 10)) 
		{ //마지막 메시지가 end of file이면 종료
            fileLog(INFO, "file close");
            break; //while문 빠져나가기
        } 
		else 
		{
        	fileLog(INFO, "[IP:%s] %d byte recved\n", inet_ntoa(cliaddr.sin_addr), nbyte);
			sprintf(p1.ip, "%s", inet_ntoa(cliaddr.sin_addr));
			snmptrap_parsing(&buf, nbyte, p1);
        }
		fileLog(INFO, "--------------------------------------------------------\n");
    }
    close(s);
	return 0;
}