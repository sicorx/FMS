#include "include.h"

char process_name[32];
int equip_count;
char modbus_server_mode = 'N';
int uid;

struct equip_conn_info *conn_info[MAX_SERVICE_COUNT];
struct equip_status *gp_status[MAX_SERVICE_COUNT];
struct eth_dev_info *gp_eth_dev_info=NULL;


pthread_once_t once_control = PTHREAD_ONCE_INIT;
pthread_once_t once_eth_dev = PTHREAD_ONCE_INIT;

pthread_mutex_t snmp_lock   = PTHREAD_MUTEX_INITIALIZER;

void mutex_init_once(void);
void mutex_init_once_eth_dev(void);
void print_config(void);
void load_equip_config(void);
int process_main(void);
void *equip_Thread_Ethernet(void *arg);
void *equip_Thread_SNMP(void *arg);

void *modbus_Server(void *arg);
void *modbus_Client(void *arg);
int eth_dev_compar(const void *info1, const void *info2);
int group_by_ip(void);

char g_db_server[32];
char g_db_user[32];
char g_db_passwd[32];
char g_db_database[32];
char g_linux_user[32];


void mutex_init_once(void)
{
    int idx, i;

    //fileLog(INFO, "STATUS 구조체 초기화..\n");

    for(idx = 0; idx < MAX_SERVICE_COUNT; idx++)
    {
        if(conn_info[idx] == NULL) continue;

        gp_status[idx]      = (struct equip_status *)malloc(sizeof(struct equip_status));
        gp_status[idx]->mux = (pthread_mutex_t *)  malloc(sizeof(pthread_mutex_t));
        pthread_mutex_init(gp_status[idx]->mux, NULL);
        gp_status[idx]->_ai = (struct equip_ai *) malloc(sizeof(struct equip_ai) * conn_info[idx]->ai_count);
        gp_status[idx]->_di = (struct equip_di *) malloc(sizeof(struct equip_di) * conn_info[idx]->di_count);
        gp_status[idx]->_do = (struct equip_do *) malloc(sizeof(struct equip_do) * conn_info[idx]->do_count);
        memset(gp_status[idx]->_ai, 0x00, sizeof(struct equip_ai) * conn_info[idx]->ai_count);
        memset(gp_status[idx]->_di, 0x00, sizeof(struct equip_di) * conn_info[idx]->di_count);
        memset(gp_status[idx]->_do, 0x00, sizeof(struct equip_do) * conn_info[idx]->do_count);
        gp_status[idx]->comm_err_count = 0;
    }

    initialize_snmp();

    for(i=0; i<equip_count; i++)
    {
        if(conn_info[i]->ai_count > 0)	load_ai_conf(i);
        if(conn_info[i]->di_count > 0)	load_di_conf(i);
        //load_do_conf(i);
        initialize_oid(i);
    }
	
}

void mutex_init_once_eth_dev(void)
{
	int idx=0, eth_dev_count=0;
	struct eth_dev_info *rkey=NULL, key;
	struct eth_dev_info *pEthDevInfo=NULL;
	struct equip_conn_info *pConnInfo=NULL;
	int (*compar)(const void *, const void *);

	eth_dev_count = group_by_ip();

	for(idx=0; idx<eth_dev_count; idx++)
	{
		pEthDevInfo = gp_eth_dev_info+idx;
		pthread_mutex_init(&pEthDevInfo->lock, NULL);
		pEthDevInfo->fd = -1;
		pEthDevInfo->index = idx;
	}

	compar = eth_dev_compar;

	for(idx=0; idx<equip_count; idx++)
	{
		pConnInfo = conn_info[idx];
		memset(&key, 0, sizeof(struct eth_dev_info));
		strcpy(key.ip, pConnInfo->ip);
		key.port = (int)pConnInfo->port;
		rkey = (struct eth_dev_info *) bsearch (&key, gp_eth_dev_info, eth_dev_count, sizeof(struct eth_dev_info), compar);
		if(NULL==rkey)
		pConnInfo->eth_dev_index = 0;
		else
		pConnInfo->eth_dev_index = rkey->index;
	}
}

int eth_dev_compar(const void *info1, const void *info2)
{
	int ret = 0;

	ret = strcmp(((struct eth_dev_info *)info1)->ip, ((struct eth_dev_info *)info2)->ip);
	if(0 != ret)
		return ret;
	else
		return ((struct eth_dev_info *)info1)->port - ((struct eth_dev_info *)info2)->port;
}

int group_by_ip(void)
{
	int i, eth_dev_count=0;
	struct eth_dev_info key;
	struct equip_conn_info *pConnInfo = NULL;
	int (*compar)(const void *, const void *);

	compar = eth_dev_compar;

	for(i=0; i<equip_count; i++)
	{
		pConnInfo = conn_info[i];

		memset(&key, 0, sizeof(key));
		strcpy(key.ip, pConnInfo->ip);
		key.port = (int)pConnInfo->port;

		if(NULL == bsearch(&key, gp_eth_dev_info, eth_dev_count, sizeof(struct eth_dev_info), compar))
		{
			eth_dev_count++;
			gp_eth_dev_info = (struct eth_dev_info *) realloc (gp_eth_dev_info, sizeof(struct eth_dev_info) * eth_dev_count);
			if(gp_eth_dev_info == NULL)
			{
				break;
			}
			memset( gp_eth_dev_info+(eth_dev_count-1), 0, sizeof(struct eth_dev_info));
			strcpy((gp_eth_dev_info+(eth_dev_count-1))->ip, pConnInfo->ip);
			gp_eth_dev_info[eth_dev_count-1].port = (int)pConnInfo->port;

			qsort(gp_eth_dev_info, eth_dev_count, sizeof(struct eth_dev_info), compar);
		}
	}

	return eth_dev_count;
}
void user_atexit(void)
{
	int i;
	for(i = 0; i < MAX_SERVICE_COUNT; i++)
	{
		free(conn_info[i]);
		if(gp_status[i])
		{
			free(gp_status[i]->mux);
			free(gp_status[i]->_ai);
			free(gp_status[i]->_di);
			free(gp_status[i]->_do);
		}
		free(gp_status[i]);
	}

	print_end_log();
}

void print_config(void)
{
	int i;
	for(i=0; i < equip_count; i++)
	{
		fileLog(INFO, "[%s:%d] eseq[%3d], it[%8s], header[%s], model_seq[%4d], tail[%s], ip[%15s], port[%5d], id[%2d], ai_count[%3d], di_count[%3d], do_count[%2d], "
					  "net_err_count[%2d], timeout[%3d], ext_addr[%2d], READ_COMMUNITY[%s]\n", 
		__FUNCTION__, __LINE__, 
		conn_info[i]->eseq, 
		conn_info[i]->interface_type == DEV_ETHERNET_TYPE ? STR_DEV_ETHERNET_TYPE : 
		conn_info[i]->interface_type == DEV_SERIAL_TYPE ? STR_DEV_SERIAL_TYPE : 
		conn_info[i]->interface_type == DEV_SNMP_TYPE ? STR_DEV_SNMP_TYPE : STR_DEV_UNKNOWN_TYPE,
		conn_info[i]->header == HEADER_MODBUS_TCP ? STR_HEADER_MODBUS_TCP : STR_HEADER_UNKNOWN_TYPE,
		conn_info[i]->model_seq,
		conn_info[i]->tail == TAIL_MODBUS_CRC16 ? STR_TAIL_MODBUS_CRC16 : STR_TAIL_UNKNOWN_TYPE,
		conn_info[i]->ip, conn_info[i]->port, conn_info[i]->id, 
		conn_info[i]->ai_count, conn_info[i]->di_count, conn_info[i]->do_count, 
		conn_info[i]->net_err_count, conn_info[i]->timeout, conn_info[i]->ext_addr, conn_info[i]->snmp_read_community);
	}
}

void load_equip_config(void)
{
	int i;	

	for(i=0; i<equip_count; i++)
	{
		conn_info[i] = (struct equip_conn_info *)malloc(sizeof(struct equip_conn_info));
		memset(conn_info[i], 0x00, sizeof(struct equip_conn_info));
	}

	get_equip_config();
	print_config();
}

int process_main(void)
{
    int i, *equip_num;
    int err_status;
	char err_buff[128] = {0x00, };
	pthread_t t_id;
	pthread_attr_t t_attr;
	
	load_equip_config();

    err_status = pthread_attr_init(&t_attr);
	if(0 != err_status) fileLog(WARNING, "Thread Option Initialize Fail: %s\n", strerror_r(err_status, err_buff, sizeof(err_buff)));

	err_status = pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);
	if(0 != err_status) fileLog(WARNING, "Thread Option(PTHREAD_CREATE_DETACHED) Setting Fail: %s\n", strerror_r(err_status, err_buff, sizeof(err_buff)));

	err_status = pthread_attr_setschedpolicy(&t_attr, SCHED_RR);
	if(0 != err_status) fileLog(WARNING, "Thread Option(SCHED_RR) Setting Fail: %s\n", strerror_r(err_status, err_buff, sizeof(err_buff)));

	err_status = pthread_attr_setinheritsched(&t_attr, PTHREAD_INHERIT_SCHED);
	if(0 != err_status) fileLog(WARNING, "Thread Option(PTHREAD_INHERIT_SCHED) Setting Fail: %s\n", strerror_r(err_status, err_buff, sizeof(err_buff)));

	err_status = pthread_attr_setscope(&t_attr, PTHREAD_SCOPE_SYSTEM);
	if(0 != err_status) fileLog(WARNING, "Thread Option(PTHREAD_SCOPE_SYSTEM) Setting Fail: %s\n", strerror_r(err_status, err_buff, sizeof(err_buff)));

    for(i = 0; i < equip_count; i++)
    {
		if(conn_info[i] == NULL) continue;

		equip_num  = (int *)malloc(sizeof(int));
		*equip_num = i;

		
        if(conn_info[i]->interface_type == DEV_ETHERNET_TYPE)	
		{
			err_status = pthread_create(&t_id, &t_attr,		equip_Thread_Ethernet, (void *)equip_num);
		}
        else if(conn_info[i]->interface_type == DEV_SNMP_TYPE)	
		{	
			err_status = pthread_create(&t_id, &t_attr,			equip_Thread_SNMP, (void *)equip_num);
		}
        else
            continue;

		if(0 != err_status)
		{
			fileLog(WARNING, "pThread Create Fail: %s\n", i + 1, (char *)strerror_r(err_status, err_buff, sizeof(err_buff)));
		}
    }

	if(modbus_server_mode == 'Y')
	{
		err_status = pthread_create(&t_id, &t_attr, modbus_Server, NULL);
		if(0 != err_status)
		{
			fileLog(WARNING, "Modbus Server pThread Create Fail: %s\n", (char *)strerror_r(err_status, err_buff, sizeof(err_buff)));
		}
	}

	return 0;

}

void *equip_Thread_Ethernet(void *arg)
{
	int equip_idx, fd=-1, is_comm_error=0;
	long sec_timeout = 0L, msec_timeout= 0L;
	char temp_buff[64];
	//char trap_buff[TRAP_MAX_LEN];
	unsigned char rwbuff[8192];
	struct timeval timeout;
	struct equip_conn_info *pConnInfo = NULL;
	struct equip_di *pDI = NULL, *pDI_Net = NULL;
	struct equip_status *pStatus = NULL;
	int (*communicate_func)(int, unsigned char *, int) = NULL;

	double di_diff, update_diff;
	
	time_t now;
	now = time(NULL);

	equip_idx = *((int *)arg);
	free(arg);

	/* server_Thread 가 서버와 스타트 트랩을 정상적으로 송수신하고 난 후 
	* 장애 트랩을 전송하게 몇 초간 쉰다. */
	sleep(1);

	memset(rwbuff, 0x00, sizeof(rwbuff));
	memset(temp_buff, 0x00, sizeof(temp_buff));
	//sprintf((char *)rwbuff, "module%d_timeout", equip_idx + 1);
	//get_key_value(CONF_FILE, (char *)rwbuff, temp_buff);
	sec_timeout  = (conn_info[equip_idx]->timeout) / 1L;
	msec_timeout = (conn_info[equip_idx]->timeout) % 1L;

	pConnInfo = conn_info[equip_idx];

#ifdef DEBUG
    fileLog(CRITICAL, "[%s:%d] Ethernet(%d) Thread Start..\n", __FUNCTION__, __LINE__, pConnInfo->eseq);
#endif
	pthread_once(&once_control, mutex_init_once);
	pthread_once(&once_eth_dev, mutex_init_once_eth_dev);

	pDI = gp_status[equip_idx]->_di;
	pDI_Net = pDI+(pConnInfo->di_count-1);
	pStatus = gp_status[equip_idx];


	for(;;)
	{
		
		if((pStatus->comm_err_count >= pConnInfo->net_err_count)
			&& (pDI_Net->use_yn == YES)
			&& (pDI_Net->alarm_yn == YES)
			&& (pDI_Net->critical_alarm_yn == YES)
			&& (pDI_Net->curr_status == NORMAL)
			)
		{
			
            pthread_mutex_lock(pStatus->mux);
			pDI_Net->curr_val = COM_ABNORMAL;
			pDI_Net->curr_status = CRITICAL;
			alarm_insert(OCCURE, equip_idx, DI, pConnInfo->di_count-1, CRITICAL);
			///////////////////////
			//통신불량 알람 DB 처리 루틴
			///////////////////////
            pthread_mutex_unlock(pStatus->mux);

            memset(rwbuff, 0x00, sizeof(rwbuff));
            set_module_status(rwbuff, equip_idx, COM_ABNORMAL);	//장비데이터 0으로 바꿈
#ifdef DEBUG
			fileLog(CRITICAL, "[%s:%d] eseq=[%d] Communication Fail Count: (%d/%d)\n", __FUNCTION__, __LINE__, pConnInfo->eseq, pStatus->comm_err_count, pConnInfo->net_err_count);
#endif
		}
			
        pthread_mutex_lock(&(gp_eth_dev_info+pConnInfo->eth_dev_index)->lock);
		do
		{
			switch(pConnInfo->model_seq)
			{
				//UPS 시작
				case UPS_HIPULSE_U				: communicate_func = communicate_HipulseU;					break;				
				
				//항온항습기 시작
				case MST						: communicate_func = communicate_mst;						break;

				case NEMO						: 
				case NEMO_ECU					: communicate_func = communicate_nemo;						break;

				case AR							:
				case AR_HP8KB					:
				case AR_36BYTE					: communicate_func = communicate_ar;						break;	
				
				case AR_OLD						:
				case AR_SPHCEX					:
				case SYSTRONIC_P				:
				case AR_HANP08					:
				case AR_HP8EPQ					:
				case AR_HP8EPF					:
				case SYSTRONIC_I				:
				case AR_HANE08					:
				case AR_STHCE					:
				case AR_HANE09					:
				case AR_SSHP8Y1					:
				case AR_HANE09_EXOR				: communicate_func = communicate_ar_old;					break;
				case AR_MODBUS					: communicate_func = communicate_ar_modbus;					break;				
				case SVU						: communicate_func = communicate_svu;						break;					
				case DDC_SHINSUNG				: communicate_func = communicate_ddc_shinsung;				break;
				case DDC_HANGUL					: communicate_func = communicate_ddc_hangul;				break;
				case DDC_HANGUL2				: communicate_func = communicate_ddc_hangul2;				break;
				case FAC1000					: communicate_func = communicate_fac1000;					break;

				case SEUNGIL					:
				case SEUNGIL_MLTM				: communicate_func = communicate_seungil;					break;
				case SYSKOREA					: communicate_func = communicate_syskorea;					break;
				case BY_CRC2004					: communicate_func = communicate_by_crc2004;				break;
				case DY_SS2000					: communicate_func = communicate_dy_ss2000;					break;
				case LIEBERT_CHTP				: communicate_func = communicate_liebert_chtp;				break;
				case AR_HANE09_DYRCU			: communicate_func = communicate_ar_dyrcu;					break;
				case AR_MODBUS_DYRCU			: communicate_func = communicate_ar_modbus_dyrcu;			break;
				case AR_HANE09_Q				: communicate_func = communicate_ar_question;				break;
				case DX100						: communicate_func = communicate_dx100;						break;
				case MeX100						: communicate_func = communicate_mex100;					break;

				case MST_ASCII					:
				case MST_HMM21					:
				case MST_COOLER					: communicate_func = communicate_mst_hmm21;					break;
				case OC485						: communicate_func = communicate_oc485;						break;
				case SEUNGIL_STHC_MB_FW36		: communicate_func = communicate_seungil_sthc_mb_fw36;		break;
				case SOLID						: communicate_func = communicate_solid;						break;
				case STHAV_MB_R1				: communicate_func = communicate_sthav_mb_r1;				break;

				case RTU_AR_MODBUS				: communicate_func = communicate_rtu_ar_modbus;				break;
				case RTU_LIEBERT_CHTP			: communicate_func = communicate_rtu_liebert_chtp;			break;
				case RTU_OC485					: communicate_func = communicate_rtu_oc485;					break;
				case RTU_AR_HANE09				: communicate_func = communicate_rtu_ar_hane09;				break;
				case RTU_AR_HANE09_Q			: communicate_func = communicate_rtu_ar_hane09_q;			break;
				//항온항습기 끝 air
				
				//분전반 시작 dpm
				case DPM_MPM330					: communicate_func = communicate_mpm330;					break;

				case DPM_MPM330A				:
				case DPM_MPM330A_TR				:
				case DPM_MPM330A_PMS			: communicate_func = communicate_mpm330a;					break;
				
				case DPM_KDY_200				:
				case DPM_GiMACi					:
				case DPM_GiMACIIPlus			:
				case DPM_GiPAM115FI				:
				case DPM_GiMACi_SG1040			: communicate_func = communicate_modbus;					break;

				case RTU_MPM330A				: communicate_func = communicate_rtu_mpm330a;				break;				
				case RTU_DPM_GiMACi				: communicate_func = communicate_rtu_gimaci;				break;
				case RTU_DPM_GiMACIIPlus		: communicate_func = communicate_rtu_gimaciiplus;			break;
				case RTU_DPM_GiPAM115FI			: communicate_func = communicate_rtu_gipam115fi;			break;

				case DPM_ACCURA3700				: communicate_func = communicate_accura3700;				break;
				case DPM_ACCURA2300S_3P1		: communicate_func = communicate_accura2300s_3p1;			break;
				case DPM_ACCURA2300S_3P41		: communicate_func = communicate_accura2300s_3p41;			break;
				case DPM_ACCURA2300S_3P3_1P3F18	: communicate_func = communicate_accura2300s_3p3_1p3f18;	break;
				case DPM_ACCURA3300E			: communicate_func = communicate_accura3300e;				break;
				//추후 추가 예정
				//case DPM_GiPAM2000FIM		: communicate_func = communicate_GiPAM2KFIM;			break;
				//case DPM_IMPRO				: communicate_func = communicate_impro;					break;
				//case DPM_ViMAC_IV_V			: communicate_func = communicate_ViMAC_IV_V;			break;
				//case DPM_ODT304				: communicate_func = communicate_odt304_melsec;			break;
				//case DPM_ACCURA3300			:
				//case DPM_ACCURA3550			: communicate_func = communicate_accura3300;			break;
				//case DPM_ACCURA3500			: communicate_func = communicate_accura3500;			break;
				//case DPM_DM2C_MT71			: communicate_func = communicate_DM2C_MT71;				break;
				//case DPM_ODT3XX				:
				//case DPM_ODT3XX_GLOFA_FENET : communicate_func = communicate_odt304_modbus;			break;
				//case DPM_ODE1XX				: communicate_func = communicate_ode1xx;				break;
				//case DPM_TMECS_200			: communicate_func = communicate_tmecs200;				break;
				//case DPM_ION_6200			: communicate_func = communicate_ion6200;				break;
				//case DPM_ION_7650			: communicate_func = communicate_ion7650;				break;
				//case DPM_KB_GD10_L01		: communicate_func = communicate_kb_gd10_l01;			break;
				//case DPM_GiPAM2000FIM_DI	: communicate_func = communicate_GiPAM2KFIM_DI;			break;
				//case DPM_MAX304_MELSEC		: communicate_func = communicate_max304_melsec;			break;
				//case DPM_RTP300				: communicate_func = communicate_rtp300;				break;
				//case DPM_RTM050				: communicate_func = communicate_rtm050;				break;
				//case DPM_MT4Y				: communicate_func = communicate_mt4y;					break;
				//case DPM_HDUNI_TOTAL_POW	: communicate_func = communicate_hduni_totalpower;		break;
				//case DPM_GE_F650			: communicate_func = communicate_gef6501;				break;
				//case DPM_ACCURA3300_DYRCU	: communicate_func = communicate_accura3300_dyrcu;		break;
				//case DPM_MAX304_MELSEC_DYRCU: communicate_func = communicate_max304_melsec_dyrcu;	break;
				//case DPM_MT4Y_DYRCU			: communicate_func = communicate_mt4y_dyrcu;			break;
				//case DPM_MPM330A_DYRCU		: communicate_func = communicate_mpm330_dyrcu;			break;
				//분전반 끝
				
				//STS 시작
				case RTU_SUPER_SWITCH3		: communicate_func = communicate_rtu_SuperSwitch3_STS;	break;
				case SUPER_SWITCH3			: communicate_func = communicate_SuperSwitch3_STS;		break;
				//STS 끝

				//온습도 시작
				case MES100					: communicate_func = communicate_mes100;				break;
				case ITTS2					: communicate_func = communicate_itts2;					break;
				case INNOTUBE				: communicate_func = communication_innotube;			break;

				case RTU_TH1				:
				case RTU_TH2				:
				case RTU_TH3				:
				case RTU_TH4				:
				case RTU_TH5				:
				case RTU_TH6				:
				case RTU_TH7				:
				case RTU_TH8				: communicate_func = communication_rtu_th;				break;
				case P_300C					: communicate_func = communicate_modbus;				break;
				case SP_INC					: communicate_func = communicate_modbus;				break;
				//온습도 끝
				
				//접점 시작
				case KM6053					: communicate_func = communicate_modbus;				break;
				case RM6054					: communicate_func = communicate_rm6054;				break;
				case RTU_RTU5000			: communicate_func = communicate_rtu_rtu5000;			break;
				//접점 끝
				
				//수배전반 시작
				case PAC100					: communicate_func = communicate_pac100;				break;
				case HIMAP_FI				: communicate_func = communicate_modbus;				break;
				case HIMAP_T				: communicate_func = communicate_modbus;				break;
				//수배전반 끝

				//OCGR 시작
				case GDR					: communicate_func = communicate_gdr;					break;
				//OCGR 끝

				//계전기 시작
				case CIMON					: communicate_func = communicate_cimon;					break;
				//계전기 끝



				//누수 거리형 시작
				
				//case MISA				: communicate_func = communicate_misa;					break;
				//case NDTECH_OLD		: communicate_func = communicate_ndtech_old;			break;
				case TTDM128			: communicate_func = communicate_ttdm128;				break;
				//case NDTECH_MODBUS		:
				//case NDTECH_LDCM100_110: communicate_func = communicate_ndtech_modbus;			break;
				//case TTDM128_RECV		: communicate_func = communicate_ttdm128_recv;			break;
				case A_LLM1				: communicate_func = communicate_a_llm1;				break;
				case RTU_TTDM128		: communicate_func = communication_rtu_ttdm128;			break;
				case LBSM200			: communicate_func = communicate_modbus;				break;
				//누수 거리형 끝



				//BMS 시작
				case BTECH				: communicate_func = communicate_btech;					break;
				//BMS 끝

				//ALTS 시작
				case ILJIN_ALTS			: communicate_func = communicate_iljin_alts;				break;
				//ALTS 끝

				//지열 시작
				case EXP40_TTF_DC		: communicate_func = communicate_exp40_ttf_dc;				break;
				//지열 끝

				default				:
					fileLog(CRITICAL, "[%s:%d] Undefined Model Type.. EQUIP_SEQ=[%d],model_seq=[%d]\n", __FUNCTION__, __LINE__, conn_info[equip_idx]->eseq, conn_info[equip_idx]->model_seq);
					usleep(1000000);
					is_comm_error = 0xAA;
					break;
			}
			if((fd = connectServer(pConnInfo->ip, (unsigned short)pConnInfo->port)) == -1)
            {
                is_comm_error = -1;
                break;
            }

            memset(rwbuff, 0x00, sizeof(rwbuff));
			
			is_comm_error = communicate_func(fd, rwbuff, equip_idx);

            close(fd);

		} while(0);
		
        pthread_mutex_unlock(&(gp_eth_dev_info+pConnInfo->eth_dev_index)->lock);
		
		if(is_comm_error == 0)
		{			
			pStatus->comm_err_count = 0;

            pthread_mutex_lock(pStatus->mux);
			pDI_Net->curr_val = COM_NORMAL;
			pDI_Net->curr_status = NORMAL;

			if(pDI_Net->curr_status == CRITICAL)
			{
				alarm_insert(CLEAR, equip_idx, DI, pConnInfo->di_count-1, CRITICAL);
			}
			///////////////////////
			//통신정상 알람 DB 처리 루틴
			///////////////////////
            pthread_mutex_unlock(pStatus->mux);

			set_module_status(rwbuff, equip_idx, COM_NORMAL);

		}
		else if(is_comm_error == -1)
		{
			pStatus->comm_err_count++;
		}

		timeout.tv_sec	= sec_timeout;
		timeout.tv_usec = msec_timeout;

		select(1, NULL, NULL, NULL, &timeout);
		
		if(pDI_Net->use_yn == YES)
		{
			//데이터 로그 인서트
			di_diff = difftime(now, pDI_Net->tm_save);
			if(pDI_Net->save_yn==YES && (di_diff >= pDI_Net->save_term || pDI_Net->tm_save == 0))
			{
				mysqlinsertData(pConnInfo->eseq, pDI_Net->tseq, pDI_Net->curr_val);
				pDI_Net->tm_save = time(NULL);
			}
			
			//데이터 로그 업데이트
			update_diff = difftime(now, pDI_Net->tm_update);
			if(update_diff >= pDI_Net->update_term || pDI_Net->tm_update == 0)
			{
				mysqlUpdateTag(pConnInfo->eseq, pDI_Net->tseq, pDI_Net->curr_val);
				pDI_Net->tm_update = time(NULL);
			}
		}
	}
	return (void*)1;	//void return인데 리턴 안쓰면 warning 남 확인 필요
}

void *equip_Thread_SNMP(void *arg)
{
    int equip_idx, err_stat, wbytes = 0;
	int is_comm_error = 0;
    long sec_timeout = 0L, msec_timeout= 0L;
    char rwbuff[128], temp_buff[128];
    struct timeval timeout;
    struct equip_conn_info *pConnInfo = NULL;
    struct equip_di *pDI = NULL, *pDI_Net = NULL;
    struct equip_status *pStatus = NULL;
    char trap_buff[TRAP_MAX_LEN];

    char snmpget_cmd[1024], filename[256], basicstate[256], statebuff[256];
    FILE *fp=NULL;
    char *ptr=NULL, *ptrptr=NULL;

    pthread_detach(pthread_self());

    equip_idx = *((int *)arg);
    free(arg);

    sleep(5);

	memset(rwbuff, 0x00, sizeof(rwbuff));
	memset(temp_buff, 0x00, sizeof(temp_buff));
	sec_timeout  = (conn_info[equip_idx]->timeout) / 1L;
	msec_timeout = (conn_info[equip_idx]->timeout) % 1L;

	pConnInfo = conn_info[equip_idx];

#ifdef DEBUG
    fileLog(CRITICAL, "SNMP(%d) Thread Start..\n", pConnInfo->eseq);
#endif

	pthread_once(&once_control, mutex_init_once);

	pDI = gp_status[equip_idx]->_di;
	pDI_Net = pDI+(pConnInfo->di_count-1);
	pStatus = gp_status[equip_idx];

	for(;;)
	{
		if((pStatus->comm_err_count >= pConnInfo->net_err_count)
			&& (pDI_Net->use_yn == YES)
			&& (pDI_Net->alarm_yn == YES)
			&& (pDI_Net->critical_alarm_yn == YES)
			&& (pDI_Net->curr_status == NORMAL)
			)
		{
			
            pthread_mutex_lock(pStatus->mux);
			pDI_Net->curr_val = COM_ABNORMAL;
			pDI_Net->curr_status = CRITICAL;
			alarm_insert(OCCURE, equip_idx, DI, pConnInfo->di_count-1, CRITICAL);
			///////////////////////
			//통신불량 알람 DB 처리 루틴
			///////////////////////
            pthread_mutex_unlock(pStatus->mux);

            memset(rwbuff, 0x00, sizeof(rwbuff));
            //set_module_status(rwbuff, equip_idx, COM_ABNORMAL);	//장비데이터 0으로 바꿈
#ifdef DEBUG
			fileLog(CRITICAL, "[%s:%d] eseq=[%d] Communication Fail Count: (%d/%d)\n", __FUNCTION__, __LINE__, pConnInfo->eseq, pStatus->comm_err_count, pConnInfo->net_err_count);
#endif
		}
        pthread_mutex_lock(&snmp_lock);
		do {

#ifdef DEBUG
            fileLog(CRITICAL, "<<<<<<<<<< UPS(%d) SNMP GET >>>>>>>>>>\n", pConnInfo->eseq);
#endif

            err_stat = 0;

			if(uid==0)
			{
				err_stat = isRemoteHostAlive(pConnInfo->ip); // root 권한만 사용가능
				if(err_stat != 0)
				{
					pStatus->comm_err_count++;
					break;
				}
			}

			err_stat = get_synchronous_snmp(equip_idx);

            if(err_stat != 0)
            {
                pStatus->comm_err_count++;
                break;
            }
			else
			{
                pStatus->comm_err_count = 0;
			}

			if(pStatus->comm_err_count == 0)
			{
				pthread_mutex_lock(pStatus->mux);
				pDI_Net->curr_val = COM_NORMAL;
				pDI_Net->curr_status = NORMAL;

				if(pDI_Net->curr_status == CRITICAL)
				{
					alarm_insert(CLEAR, equip_idx, DI, pConnInfo->di_count-1, CRITICAL);
				}
				pthread_mutex_unlock(pStatus->mux);

			}


		} while(0);

        pthread_mutex_unlock(&snmp_lock);
		

		sleep(10); // Sleep For Debugging
	}

	return NULL;
}

void get_conf(void)
{

    if(get_key_value(CONF_FILE, "DB_SERVER", g_db_server) == -1)	err_exit(CRITICAL, "Can't Find DB_SERVER FILE_FULLPATH=[%s]\n", CONF_FILE);
#ifdef DEBUG
	else	fileLog(CRITICAL, "DB_SERVER=[%s]\n",g_db_server);
#endif
	
    if(get_key_value(CONF_FILE, "DB_USER", g_db_user) == -1)	err_exit(CRITICAL, "Can't Find DB_USER FILE_FULLPATH=[%s]\n", CONF_FILE);
#ifdef DEBUG
	else	fileLog(CRITICAL, "DB_USER=[%s]\n",g_db_user);
#endif
	
	
    if(get_key_value(CONF_FILE, "DB_PASSWD", g_db_passwd) == -1)	err_exit(CRITICAL, "Can't Find DB_PASSWD FILE_FULLPATH=[%s]\n", CONF_FILE);
#ifdef DEBUG
	else	fileLog(CRITICAL, "DB_PASSWD=[%s]\n",g_db_passwd);
#endif

	if(get_key_value(CONF_FILE, "DB_DATABASE", g_db_database) == -1)	err_exit(CRITICAL, "Can't Find DB_DATABASE FILE_FULLPATH=[%s]\n", CONF_FILE);
#ifdef DEBUG
	else	fileLog(CRITICAL, "DB_DATABASE=[%s]\n",g_db_database);
#endif
	
	if(get_key_value(CONF_FILE, "LINUX_USER", g_linux_user) == -1)	err_exit(CRITICAL, "Can't Find DB_DATABASE FILE_FULLPATH=[%s]\n", CONF_FILE);
	else	fileLog(CRITICAL, "LINUX_USER=[%s]\n",g_linux_user);
}

int main(int argc, char** argv)
{
	char *temp = NULL;


	memset(process_name, 0x00, sizeof(process_name));
	temp = strdup(argv[0]);
	strcpy(process_name, (char *)basename(temp));
	free(temp);


    /*init_signal();*/


	print_start_log();

	get_conf();
	mysql_con_init();

	uid = getid();



	equip_count = get_equip_count();
	if(equip_count == 0)	err_exit(CRITICAL, "TB_EQUIP COUNT=[0]\n");
	else					
	
#ifdef _TEST
	fileLog(CRITICAL, "[%s:%d] !!!!!!!!!!!!!TEST MODE!!!!!!!!!!!!!\n", __FUNCTION__, __LINE__, equip_count);
#else
	fileLog(INFO, "[%s:%d] EQUIP_COUNT=[%d]\n", __FUNCTION__, __LINE__, equip_count);
#endif	
	

	atexit(user_atexit);

	process_main();



	pause(); //For Thread

	print_end_log();
	return 1;
}