#include "include.h"

extern int equip_count;
extern struct equip_conn_info *conn_info[];
extern struct equip_status *gp_status[];

extern char g_db_server[32];
extern char g_db_user[32];
extern char g_db_passwd[32];
extern char g_db_database[32];

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

int get_equip_count(void)
{
	MYSQL *conn;
	MYSQL_RES *result;
	char query[256];
	int ret=0;

	conn = mysql_init(NULL);

	/* Connect to database */
	if (!mysql_real_connect(conn, g_db_server, g_db_user, g_db_passwd, g_db_database, 0, NULL, CLIENT_MULTI_RESULTS))
	{
		fileLog(WARNING, "[%s:%d] MYSQL connection error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
		return -1;
	}

#ifdef _TEST
	sprintf(query, " SELECT * FROM TB_EQUIP WHERE EQUIP_SEQ=%d ", EQUIP_SEQ);
#else
	sprintf(query, " SELECT * FROM TB_EQUIP WHERE USE_YN='Y' limit %d; ", MAX_SERVICE_COUNT);
#endif
	
	if(mysql_query(conn, query) != 0)
	{
		fileLog(WARNING, "[%s:%d] MYSQL query error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
		return -1;
	}


	result = mysql_store_result(conn);
	
	ret = mysql_num_rows(result);
	mysql_free_result(result);

	mysql_close(conn);

	return ret;
}

int get_equip_config(void)
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
		return -1;
	}

#ifdef _TEST
	sprintf(query, " SELECT EQUIP_SEQ, HEADER, TAIL, EQUIP_GET_IP, EQUIP_GET_PORT, EQUIP_ID, AI_CNT, DI_CNT, DO_CNT, NET_ERR_CNT, MODEL_SEQ, TIMEOUT, EXT_ID, COMM_METHOD_CD, SNMP_READ_COMMUNITY FROM TB_EQUIP WHERE EQUIP_SEQ=%d; ", EQUIP_SEQ );
	#ifdef DEBUG
	fileLog(INFO, "[%s:%d] %s\n", __FUNCTION__, __LINE__, query);
	#endif
#else
	sprintf(query, " SELECT EQUIP_SEQ, HEADER, TAIL, EQUIP_GET_IP, EQUIP_GET_PORT, EQUIP_ID, AI_CNT, DI_CNT, DO_CNT, NET_ERR_CNT, MODEL_SEQ, TIMEOUT, EXT_ID, COMM_METHOD_CD, SNMP_READ_COMMUNITY FROM TB_EQUIP WHERE USE_YN='Y' limit %d; ", MAX_SERVICE_COUNT);
	#ifdef DEBUG
	fileLog(INFO, "[%s:%d] %s\n", __FUNCTION__, __LINE__, query);
	#endif
#endif

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
			if(row[0] != NULL) conn_info[i]->eseq = atoi(row[0]);
			if(row[1] != NULL) conn_info[i]->header = atoi(row[1]);
			if(row[2] != NULL) conn_info[i]->tail = atoi(row[2]);
			if(row[3] != NULL) strcpy(conn_info[i]->ip, row[3]);
			if(row[4] != NULL) conn_info[i]->port = atoi(row[4]);
			if(row[5] != NULL) conn_info[i]->id = atoi(row[5]);

			if(row[6] != NULL) conn_info[i]->ai_count = atoi(row[6]);
			if(row[7] != NULL) conn_info[i]->di_count = atoi(row[7]);
			if(row[8] != NULL) conn_info[i]->do_count = atoi(row[8]);
			if(row[9] != NULL) conn_info[i]->net_err_count = atoi(row[9]);
			if(row[10] != NULL) conn_info[i]->model_seq = atoi(row[10]);
			if(row[11] != NULL) conn_info[i]->timeout = atoi(row[11]);
			if(row[12] != NULL) conn_info[i]->ext_addr = atoi(row[12]);
			if(row[13] != NULL) conn_info[i]->interface_type =	!strcasecmp(row[13], "ETHERNET") ? DEV_ETHERNET_TYPE :
																!strcasecmp(row[13], "SERIAL") ? DEV_SERIAL_TYPE :
																!strcasecmp(row[13], "SNMP") ? DEV_SNMP_TYPE : DEV_UNKNOWN_TYPE;
			if(row[14] != NULL) strcpy(conn_info[i]->snmp_read_community, row[14]);

			i++;

		}

		if(result)
		{
			mysql_free_result(result);
		}

	}while(mysql_next_result(conn) == 0);

	mysql_close(conn);

	return 1;
}

int reload_equip_config(int index, int eseq)
{
	MYSQL *conn;
	MYSQL_RES *result;
	MYSQL_ROW row;
	char query[1024];

	conn = mysql_init(NULL);

	/* Connect to database */
	if (!mysql_real_connect(conn, g_db_server, g_db_user, g_db_passwd, g_db_database, 0, NULL, CLIENT_MULTI_RESULTS))
	{
		fileLog(WARNING, "[%s:%d] MYSQL connection error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
		return -1;
	}

	sprintf(query, " SELECT EQUIP_SEQ, HEADER, TAIL, EQUIP_GET_IP, EQUIP_GET_PORT, EQUIP_ID, AI_CNT, DI_CNT, DO_CNT, NET_ERR_CNT, MODEL_SEQ, TIMEOUT, EXT_ID, COMM_METHOD_CD, SNMP_READ_COMMUNITY FROM TB_EQUIP WHERE USE_YN='Y' AND EQUIP_SEQ=%d ", eseq);
	#ifdef DEBUG
	fileLog(INFO, "[%s:%d] %s\n", __FUNCTION__, __LINE__, query);
	#endif

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
			if(row[0] != NULL) conn_info[index]->eseq = atoi(row[0]);
			if(row[1] != NULL) conn_info[index]->header = atoi(row[1]);
			if(row[2] != NULL) conn_info[index]->tail = atoi(row[2]);
			if(row[3] != NULL) strcpy(conn_info[index]->ip, row[3]);
			if(row[4] != NULL) conn_info[index]->port = atoi(row[4]);
			if(row[5] != NULL) conn_info[index]->id = atoi(row[5]);

			if(row[6] != NULL) conn_info[index]->ai_count = atoi(row[6]);
			if(row[7] != NULL) conn_info[index]->di_count = atoi(row[7]);
			if(row[8] != NULL) conn_info[index]->do_count = atoi(row[8]);
			if(row[9] != NULL) conn_info[index]->net_err_count = atoi(row[9]);
			if(row[10] != NULL) conn_info[index]->model_seq = atoi(row[10]);
			if(row[11] != NULL) conn_info[index]->timeout = atoi(row[11]);
			if(row[12] != NULL) conn_info[index]->ext_addr = atoi(row[12]);
			if(row[13] != NULL) conn_info[index]->interface_type =	!strcasecmp(row[13], "ETHERNET") ? DEV_ETHERNET_TYPE :
																!strcasecmp(row[13], "SERIAL") ? DEV_SERIAL_TYPE :
																!strcasecmp(row[13], "SNMP") ? DEV_SNMP_TYPE : DEV_UNKNOWN_TYPE;
			if(row[14] != NULL) strcpy(conn_info[index]->snmp_read_community, row[14]);
		}

		if(result)
		{
			mysql_free_result(result);
		}

	}while(mysql_next_result(conn) == 0);

	mysql_close(conn);

	return 1;
}

void load_ai_conf(int index)
{
	MYSQL *conn;
	MYSQL_RES *result;
	MYSQL_ROW row;
	char query[1024];
	int count=0;

    struct equip_ai *pAI = gp_status[index]->_ai;
    struct equip_conn_info *pConnInfo = conn_info[index];

	//fileLog(INFO, "[%s:%d] index=[%d]\n", __FUNCTION__, __LINE__, index);

	conn = mysql_init(NULL);

	/* Connect to database */
	if (!mysql_real_connect(conn, g_db_server, g_db_user, g_db_passwd, g_db_database, 0, NULL, CLIENT_MULTI_RESULTS))
	{
		fileLog(WARNING, "[%s:%d] MYSQL connection error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
	}

	sprintf(query,  " SELECT TAG_SEQ, TAG_NM, TAG_ID, DATA_TYPE, RATE, USE_YN, ALARM_YN, SMS_YN, SAVE_YN, "
					" SAVE_TERM, UPDATE_TERM, ALARM_TERM, DATA_ADD, CURRENT_STATUS, CURRENT_VALUE, "
					" INFO_ALARM_YN, INFO_SMS_YN, INFO_LOW, INFO_HIGH, "
					" WARNING_ALARM_YN, WARNING_SMS_YN, WARNING_LOW, WARNING_HIGH, "
					" CRITICAL_ALARM_YN, CRITICAL_SMS_YN, CRITICAL_LOW, CRITICAL_HIGH "
					" FROM TB_TAG "
					" WHERE EQUIP_SEQ=%d AND ADIO='AI' "
					" LIMIT %d; "
				, pConnInfo->eseq, pConnInfo->ai_count);
#ifdef DEBUG
	fileLog(INFO, "[%s:%d] %s\n", __FUNCTION__, __LINE__, query);
#endif
	
	if(mysql_query(conn, query) != 0)
	{
		fileLog(WARNING, "[%s:%d] MYSQL query error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
	}

	do
	{
		result = mysql_store_result(conn);
		count = mysql_num_rows(result);
		
		if(count < 1) return;
		
		int i=0;
		while(result && (row = mysql_fetch_row(result)) != NULL)
		{
			if(row[0] != NULL) pAI->tseq = atoi(row[0]);
			if(row[1] != NULL) strcpy(pAI->tag_nm, row[1]);
			if(row[2] != NULL) 
			{
				strcpy(pAI->tid, row[2]);
				pAI->dataOid.Name = pAI->tid;
			}
			if(row[3] != NULL)
			{
				if(!strncmp(row[3], "I", 1))		pAI->data_type = TYPE_INT;
				else if(!strncmp(row[3], "F", 1))	pAI->data_type = TYPE_FLOAT;
				else if(!strncmp(row[3], "D", 1))	pAI->data_type = TYPE_DOUBLE;
				else								pAI->data_type = TYPE_INT;
			}
			if(row[4] != NULL) pAI->rate = atoi(row[4]);
			if(row[5] != NULL) 
			{
				if(!strncmp(row[5], "Y", 1))		pAI->use_yn = YES;
				else if(!strncmp(row[5], "N", 1))	pAI->use_yn = NO;
				else								pAI->use_yn = YES;				
			}
			if(row[6] != NULL) 
			{
				if(!strncmp(row[6], "Y", 1))		pAI->alarm_yn = YES;
				else if(!strncmp(row[6], "N", 1))	pAI->alarm_yn = NO;
				else								pAI->alarm_yn = YES;
			}
			if(row[7] != NULL)
			{
				if(!strncmp(row[7], "Y", 1))		pAI->sms_yn = YES;
				else if(!strncmp(row[7], "N", 1))	pAI->sms_yn = NO;
				else								pAI->sms_yn = YES;
			}
			if(row[8] != NULL)
			{
				if(!strncmp(row[8], "Y", 1))		pAI->save_yn = YES;
				else if(!strncmp(row[8], "N", 1))	pAI->save_yn = NO;
				else								pAI->save_yn = YES;
			}
			if(row[9] != NULL) pAI->save_term = atoi(row[9]);
			if(row[10] != NULL) pAI->update_term = atoi(row[10]);
			if(row[11] != NULL) pAI->alarm_term = atoi(row[11]);
			if(row[12] != NULL) pAI->data_add = atoi(row[12]);
			if(row[13] != NULL)
			{
				if(!strncmp(row[13], "N", 1))		pAI->curr_status = NORMAL;
				else if(!strncmp(row[13], "I", 1))	pAI->curr_status = INFO;
				else if(!strncmp(row[13], "W", 1))	pAI->curr_status = WARNING;
				else if(!strncmp(row[13], "C", 1))	pAI->curr_status = CRITICAL;
				else								pAI->curr_status = NORMAL;
				if(pAI->curr_status != NORMAL)		
				{
					pAI->send_flag	 = 1; // 태그를 읽었을때 이미 알람이면 미처리
					pAI->last_status = pAI->curr_status;
				}
			}			
			if(row[14] != NULL) pAI->curr_val = atof(row[14]);
			

			if(row[15] != NULL)
			{
				if(!strncmp(row[15], "Y", 1))		pAI->info_alarm_yn = YES;
				else if(!strncmp(row[15], "N", 1))	pAI->info_alarm_yn = NO;
				else								pAI->info_alarm_yn = YES;
			}
			if(row[16] != NULL)
			{
				if(!strncmp(row[16], "Y", 1))		pAI->info_sms_yn = YES;
				else if(!strncmp(row[16], "N", 1))	pAI->info_sms_yn = NO;
				else								pAI->info_sms_yn = YES;
			}
			if(row[17] != NULL) pAI->info_low		= atof(row[17]);
			if(row[18] != NULL) pAI->info_high		= atof(row[18]);

			if(row[19] != NULL)
			{
				if(!strncmp(row[19], "Y", 1))		pAI->warning_alarm_yn = YES;
				else if(!strncmp(row[19], "N", 1))	pAI->warning_alarm_yn = NO;
				else								pAI->warning_alarm_yn = YES;
			}
			if(row[20] != NULL)
			{
				if(!strncmp(row[20], "Y", 1))		pAI->warning_sms_yn = YES;
				else if(!strncmp(row[20], "N", 1))	pAI->warning_sms_yn = NO;
				else								pAI->warning_sms_yn = YES;
			}
			if(row[21] != NULL) pAI->warning_low	= atof(row[21]);
			if(row[22] != NULL) pAI->warning_high	= atof(row[22]);

			if(row[23] != NULL)
			{
				if(!strncmp(row[23], "Y", 1))		pAI->critical_alarm_yn = YES;
				else if(!strncmp(row[23], "N", 1))	pAI->critical_alarm_yn = NO;
				else								pAI->critical_alarm_yn = YES;
			}
			if(row[24] != NULL)
			{
				if(!strncmp(row[24], "Y", 1))		pAI->critical_sms_yn = YES;
				else if(!strncmp(row[24], "N", 1))	pAI->critical_sms_yn = NO;
				else								pAI->critical_sms_yn = YES;					
			}
			if(row[25] != NULL) pAI->critical_low	= atof(row[25]);
			if(row[26] != NULL) pAI->critical_high	= atof(row[26]);
#ifdef DEBUG
			fileLog(INFO, "tseq[%2d],tag_nm[%s],data_type[%s],"
						  "rate[%.3f],use_yn[%s],alarm_yn[%s],sms_yn[%s],save_yn[%s],"
						  //"save_term[%d],update_term[%d],alarm_term[%d],"
						  "data_add[%d],curr_status[%s],curr_val[%.2f],"
						  "iayn[%s],isyn[%s],il[%.2f],ih[%.2f],"
						  "wayn[%s],wsyn[%s],wl[%.2f],wh[%.2f],"
						  "cayn[%s],csyn[%s],cl[%.2f],ch[%.2f]\n",
				pAI->tseq,
				pAI->tag_nm,
				(pAI->data_type==TYPE_INT) ? "I" : (pAI->data_type==TYPE_FLOAT) ? "F" : (pAI->data_type==TYPE_DOUBLE) ? "D" : "I",
				pAI->rate,
				(pAI->use_yn==YES) ? "Y" : (pAI->use_yn==NO) ? "N" : "Y",
				(pAI->alarm_yn==YES) ? "Y" : (pAI->alarm_yn==NO) ? "N" : "Y",
				(pAI->sms_yn==YES) ? "Y" : (pAI->sms_yn==NO) ? "N" : "Y",
				(pAI->save_yn==YES) ? "Y" : (pAI->save_yn==NO) ? "N" : "Y",
				//pAI->save_term, pAI->update_term, pAI->alarm_term,
				pAI->data_add,
				(pAI->curr_status==INFO) ? "I" : (pAI->curr_status==WARNING) ? "W" : (pAI->curr_status==CRITICAL) ? "C" : "N",
				pAI->curr_val,				
				(pAI->info_alarm_yn==YES) ? "Y" : (pAI->info_alarm_yn==NO) ? "N" : "Y",
				(pAI->info_sms_yn==YES) ? "Y" : (pAI->info_sms_yn==NO) ? "N" : "Y",
				pAI->info_low, pAI->info_high, 
				(pAI->warning_alarm_yn==YES) ? "Y" : (pAI->warning_alarm_yn==NO) ? "N" : "Y",
				(pAI->warning_sms_yn==YES) ? "Y" : (pAI->warning_sms_yn==NO) ? "N" : "Y",
				pAI->warning_low, pAI->warning_high,
				(pAI->critical_alarm_yn==YES) ? "Y" : (pAI->critical_alarm_yn==NO) ? "N" : "Y",
				(pAI->critical_sms_yn==YES) ? "Y" : (pAI->critical_sms_yn==NO) ? "N" : "Y",
				pAI->critical_low, pAI->critical_high
			);
#endif
			i++;
			pAI++;
		}

		if(result) { mysql_free_result(result); }

	}while(mysql_next_result(conn) == 0);

	mysql_close(conn);

}

void load_di_conf(int index)
{
	MYSQL *conn;
	MYSQL_RES *result;
	MYSQL_ROW row;
	char query[1024];
	int count=0;

    struct equip_di *pDI = gp_status[index]->_di;
    struct equip_conn_info *pConnInfo = conn_info[index];

	conn = mysql_init(NULL);

	/* Connect to database */
	if (!mysql_real_connect(conn, g_db_server, g_db_user, g_db_passwd, g_db_database, 0, NULL, CLIENT_MULTI_RESULTS))
	{
		fileLog(WARNING, "[%s:%d] MYSQL connection error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
	}

	sprintf(query,	" SELECT TAG_SEQ, TAG_NM, TAG_ID, DATA_TYPE, RATE, USE_YN, ALARM_YN, SMS_YN, SAVE_YN, "
					" SAVE_TERM, UPDATE_TERM, ALARM_TERM, DATA_ADD, CURRENT_STATUS, CURRENT_VALUE, "
					" CRITICAL_ALARM_YN, CRITICAL_SMS_YN, CRITICAL_LOW, CRITICAL_HIGH "
					" FROM TB_TAG "
					" WHERE EQUIP_SEQ=%d AND ADIO='DI' "
					" LIMIT %d; "
				, pConnInfo->eseq, pConnInfo->di_count);
#ifdef DEBUG
	fileLog(INFO, "[%s:%d] %s\n", __FUNCTION__, __LINE__, query);
#endif
	
	if(mysql_query(conn, query) != 0)
	{
		fileLog(WARNING, "[%s:%d] MYSQL query error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
	}

	do
	{
		result = mysql_store_result(conn);
		count = mysql_num_rows(result);
		
		if(count > 0) 
		{
			int i=0;
			while(result && (row = mysql_fetch_row(result)) != NULL)
			{
				if(row[0] != NULL) pDI->tseq = atoi(row[0]);
				if(row[1] != NULL) strcpy(pDI->tag_nm, row[1]);
				if(row[2] != NULL) 
				{
					strcpy(pDI->tid, row[2]);
					pDI->dataOid.Name = pDI->tid;
				}
				if(row[3] != NULL)
				{
					if(!strncmp(row[3], "I", 1))		pDI->data_type = TYPE_INT;
					else if(!strncmp(row[3], "F", 1))	pDI->data_type = TYPE_FLOAT;
					else if(!strncmp(row[3], "D", 1))	pDI->data_type = TYPE_DOUBLE;
					else								pDI->data_type = TYPE_INT;
				}
				if(row[4] != NULL) pDI->rate = atoi(row[4]);
				if(row[5] != NULL) 
				{
					if(!strncmp(row[5], "Y", 1))		pDI->use_yn = YES;
					else if(!strncmp(row[5], "N", 1))	pDI->use_yn = NO;
					else								pDI->use_yn = YES;				
				}
				if(row[6] != NULL) 
				{
					if(!strncmp(row[6], "Y", 1))		pDI->alarm_yn = YES;
					else if(!strncmp(row[6], "N", 1))	pDI->alarm_yn = NO;
					else								pDI->alarm_yn = YES;					
				}
				if(row[7] != NULL)
				{
					if(!strncmp(row[7], "Y", 1))		pDI->sms_yn = YES;
					else if(!strncmp(row[7], "N", 1))	pDI->sms_yn = NO;
					else								pDI->sms_yn = YES;							
				}
				if(row[8] != NULL)
				{
					if(!strncmp(row[8], "Y", 1))		pDI->save_yn = YES;
					else if(!strncmp(row[8], "N", 1))	pDI->save_yn = NO;
					else								pDI->save_yn = YES;							
				}
				if(row[9] != NULL) pDI->save_term = atoi(row[9]);
				if(row[10] != NULL) pDI->update_term = atoi(row[10]);
				if(row[11] != NULL) pDI->alarm_term = atoi(row[11]);
				if(row[12] != NULL) pDI->data_add = atoi(row[12]);
				if(row[13] != NULL)
				{
					if(!strncmp(row[13], "N", 1))		pDI->curr_status = NORMAL;
					else if(!strncmp(row[13], "I", 1))	pDI->curr_status = INFO;
					else if(!strncmp(row[13], "W", 1))	pDI->curr_status = WARNING;
					else if(!strncmp(row[13], "C", 1))	pDI->curr_status = CRITICAL;
					else								pDI->curr_status = NORMAL;
				}
				if(row[14] != NULL) pDI->curr_val	= atoi(row[14]);
				
				if(row[15] != NULL)
				{
					if(!strncmp(row[15], "Y", 1))		pDI->critical_alarm_yn = YES;
					else if(!strncmp(row[15], "N", 1))	pDI->critical_alarm_yn = NO;
					else								pDI->critical_alarm_yn = YES;							
				}
				if(row[16] != NULL)
				{
					if(!strncmp(row[16], "Y", 1))		pDI->critical_sms_yn = YES;
					else if(!strncmp(row[16], "N", 1))	pDI->critical_sms_yn = NO;
					else								pDI->critical_sms_yn = YES;							
				}

				if(row[17] != NULL) pDI->normal_val	= atoi(row[17]);
				if(row[18] != NULL) pDI->alarm_val	= atoi(row[18]);
#ifdef DEBUG
				fileLog(INFO,	"tseq[%4d],tag_nm[%s],tid[%s],data_type[%s],rate[%f],"
								"use_yn[%s],alarm_yn[%s],sms_yn[%s],save_yn[%s],"
								//"save_term[%d],update_term[%d],alarm_term[%d],"
								"curr_status[%s],curr_val[%d],"
								"ca_yn[%s],cs_yn[%s],nv[%d],av[%d]\n", 
					pDI->tseq,
					pDI->tag_nm,
					pDI->tid,
					(pDI->data_type==TYPE_INT) ? "I" : (pDI->data_type==TYPE_FLOAT) ? "F" : (pDI->data_type==TYPE_DOUBLE) ? "D" : "I",
					pDI->rate,
					(pDI->use_yn==YES) ? "Y" : (pDI->use_yn==NO) ? "N" : "Y",
					(pDI->alarm_yn==YES) ? "Y" : (pDI->alarm_yn==NO) ? "N" : "Y",
					(pDI->sms_yn==YES) ? "Y" : (pDI->sms_yn==NO) ? "N" : "Y",
					(pDI->save_yn==YES) ? "Y" : (pDI->save_yn==NO) ? "N" : "Y",
					//pDI->save_term, pDI->update_term, pDI->alarm_term,
					(pDI->curr_status==NORMAL) ? "N" : (pDI->curr_status==INFO) ? "I" : (pDI->curr_status==WARNING) ? "W" : (pDI->curr_status==CRITICAL) ? "C" : "N",
					pDI->curr_val,
					(pDI->critical_alarm_yn==YES) ? "Y" : (pDI->critical_alarm_yn==NO) ? "N" : "Y",
					(pDI->critical_sms_yn==YES) ? "Y" : (pDI->critical_sms_yn==NO) ? "N" : "Y",
					pDI->normal_val,
					pDI->alarm_val
				);
#endif

				i++;
				pDI++;
			}
		}

		if(result) { mysql_free_result(result);	}

	}while(mysql_next_result(conn) == 0);

	mysql_close(conn);

}

int mysqlinsertData(int eseq, int tseq, float value)
{
	int ret = 1;
	int query_state;
	char query[128];

	MYSQL *conn;

	conn = mysql_init(NULL);

	/* Connect to database */
	if (!mysql_real_connect(conn, g_db_server, g_db_user, g_db_passwd, g_db_database, 0, NULL, CLIENT_MULTI_RESULTS)) 
	{
		fileLog(WARNING, "[%s:%d] MYSQL connection error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
		ret = -1;
	}

	sprintf(query, "CALL SP_INSERT_TAG_LOG(%d, %d, %f);", eseq, tseq, value);
#ifdef DEBUG
	fileLog(WARNING, "[%s:%d] %s\n", __FUNCTION__, __LINE__, query); 
#endif

	query_state = mysql_query(conn, query);
	if (query_state != 0)
	{
		fileLog(WARNING, "[%s:%d] MYSQL query error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
		ret = -1;
	}

	mysql_close(conn);

	return ret;
}

int mysqlUpdateTag(int eseq, int tseq, float value)
{
	int ret = 1;
	int query_state;
	char query[128];

	MYSQL *conn;

	conn = mysql_init(NULL);

	/* Connect to database */
	if (!mysql_real_connect(conn, g_db_server, g_db_user, g_db_passwd, g_db_database, 0, NULL, CLIENT_MULTI_STATEMENTS)) 
	{
		fileLog(WARNING, "[%s:%d] MYSQL connection error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
		ret = -1;
	}


	sprintf(query, "CALL SP_UPDATE_TAG_VALUE(%d, %.2f);",tseq, value);
#ifdef DEBUG
	fileLog(INFO, "[%s:%d] query=%s\n", __FUNCTION__, __LINE__, query);
#endif

	query_state = mysql_query(conn, query);
	if (query_state != 0)
	{
		fileLog(WARNING, "[%s:%d] MYSQL query error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
		ret = -1;
	}

	mysql_close(conn);

	return ret;

}


int alarm_insert(int flag, int index, int adio, int seq, int alarm_grade)
{
	int ret = 1;
	int query_state;
	char query[1024];
	int tseq = 0;
	float curr_val = 0;
	float th_low = 0;
	float th_high = 0;
	char sms_yn = 'N';

    struct equip_ai *pAI = gp_status[index]->_ai;
    struct equip_di *pDI = gp_status[index]->_di;
    struct equip_conn_info *pConnInfo = conn_info[index];
	
	if(adio==AI)
	{
		tseq = (pAI+seq)->tseq;
		curr_val = (pAI+seq)->curr_val;
		if(alarm_grade == CRITICAL)
		{
			if((pAI+seq)->sms_yn == YES && (pAI+seq)->critical_sms_yn == YES) sms_yn = 'Y';

			th_low = (pAI+seq)->critical_low;
			th_high = (pAI+seq)->critical_high;
		}
		else if(alarm_grade == WARNING)
		{
			if((pAI+seq)->sms_yn == YES && (pAI+seq)->critical_sms_yn == YES) sms_yn = 'Y';
			th_low = (pAI+seq)->warning_low;
			th_high = (pAI+seq)->warning_high;
		}
		else if(alarm_grade == INFO)
		{
			if((pAI+seq)->sms_yn == YES && (pAI+seq)->critical_sms_yn == YES) sms_yn = 'Y';
			th_low = (pAI+seq)->info_low;
			th_high = (pAI+seq)->info_high;
		}
	}
	else if(adio==DI)
	{
		tseq = (pDI+seq)->tseq;
		curr_val = (pDI+seq)->curr_val;
		th_low = (pDI+seq)->normal_val;
		th_high = (pDI+seq)->alarm_val;
		if((pDI+seq)->sms_yn == YES && (pDI+seq)->critical_sms_yn == YES) sms_yn = 'Y';
	}

	MYSQL *conn;

	conn = mysql_init(NULL);

	/* Connect to database */
	if (!mysql_real_connect(conn, g_db_server, g_db_user, g_db_passwd, g_db_database, 0, NULL, CLIENT_MULTI_STATEMENTS)) 
	{
		fileLog(WARNING, "[%s:%d] MYSQL connection error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
		ret = -1;
	}

	if(flag == OCCURE)
	{
		sprintf(query,  "INSERT INTO `TB_ALARM_LOG` "
						"(`EQUIP_SEQ`, `TAG_SEQ`, `ALARM_GRADE_CD`, `CURR_VALUE`, `TH_VALUE_LOW`, `TH_VALUE_HIGH`, `SMS_YN`) "
						"VALUES "
						"(%d, %d, %d, %f, %f, %f, '%c');"
						"UPDATE TB_TAG SET CURRENT_STATUS='%s' WHERE TAG_SEQ=%d ORDER BY REG_DT DESC LIMIT 1;",
						pConnInfo->eseq, tseq, alarm_grade, curr_val, th_low, th_high, sms_yn,
						alarm_grade==INFO ? "I" : alarm_grade==WARNING ? "W" : alarm_grade==CRITICAL ? "C" : "N", tseq
						);
	}
	else if(flag == CLEAR)
	{
		sprintf(query,  "UPDATE TB_ALARM_LOG SET "
						"ALARM_STATUS=1, "
						"REC_DT=now() "
						"WHERE "
						"EQUIP_SEQ=%d AND TAG_SEQ=%d AND ALARM_GRADE_CD=%d ORDER BY REG_DT DESC LIMIT 1;"
						"UPDATE TB_TAG SET CURRENT_STATUS='N' WHERE TAG_SEQ=%d ORDER BY REG_DT DESC LIMIT 1;",
						pConnInfo->eseq, tseq, alarm_grade,							
						tseq
						);
	}
#ifdef DEBUG
	fileLog(WARNING, "[%s:%d] %s\n", __FUNCTION__, __LINE__, query); 
#endif

	query_state = mysql_query(conn, query);
	if (query_state != 0)
	{
		fileLog(WARNING, "[%s:%d] MYSQL query error : %s\n", __FUNCTION__, __LINE__, mysql_error(conn));
		ret = -1;
	}

	mysql_close(conn);

	return ret;
}