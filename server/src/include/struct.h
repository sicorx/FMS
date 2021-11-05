#ifndef _STRUCT_H_
#define _STRUCT_H_ 1


struct _st_DBinfo
{
	char pserver[16];
	char puser[20];
	char ppassword[20]; /* set me first */
	char pdatabase[20];
};


struct threadpool
{
	char	ip[15];
	int		port;
};


struct equip_conn_info 
{
	unsigned char	interface_type;	//ETHERNET || SNMP || SERIAL
	unsigned char	header;
	unsigned char	tail;
	unsigned int	eseq;			//EQUIP_SEQ
	char			ip[16];			//EQUIP_IP
	unsigned int	port;			//EQUIP_PORT
	unsigned char	id;				//EQUIP_ID

	unsigned int	ai_count;
	unsigned int	di_count;
	unsigned int	do_count;
	unsigned char	net_err_count;
	unsigned short	model_seq;

	time_t	tm_update;				// TAG 업데이트 시간
	unsigned long	timeout;
	int				eth_dev_index;
	unsigned char	ext_addr;		//확장어드레스
	char	snmp_read_community[16];//
};

struct eth_dev_info {
    char  ip[32];         /* Ethernet Type, IP */
    int   port;           /* Ethernet Type, Port Number */
    pthread_mutex_t lock; /* Ethernet Mutex */
    int   fd;             /* socket fd */
    int   index;          /* Index */
};

struct equip_status 
{
    pthread_mutex_t  *mux;
    struct equip_ai *_ai;
    struct equip_di *_di;
    struct equip_do *_do;
    int    comm_err_count;
};

struct data_oid {
    char    *Name;
    oid     Oid[MAX_OID_LEN];
    int     OidLen;
};

struct equip_ai 
{
	int     tseq;				// AI 센서 인덱스(TAG_SEQ)
	//char    type;				// AI (STS, ALM, SET)
	char	tag_nm[128];		// 태그 이름
	char	tid[64];			// 태그 modbus 번지수
	char	data_type;			// 데이터 자료형 (I, F)
	float	rate;				// 데이터 단위
	char    use_yn;				// AI 사용 여부(Y, N)
	char    alarm_yn;			// AI 알람 사용 여부(Y, N)
	char    sms_yn;				// AI sms 사용 여부(Y, N)
	char    save_yn;			// AI 데이터 저장 사용 여부(Y, N)
	unsigned int save_term;		// 데이터 저장 주기
	unsigned int update_term;	// 데이터 업데이트 주기
	int		alarm_term;			// 알람 지속 주기
	int		data_add;			// 데이터 보정
	
	int		send_flag;
	char	info_alarm_yn;
	char	info_sms_yn;
	float	info_low;
	float	info_high;

	char	warning_alarm_yn;
	char	warning_sms_yn;
	float	warning_low;
	float	warning_high;

	char	critical_alarm_yn;
	char	critical_sms_yn;
	float	critical_low;
	float	critical_high;

	char    curr_status;		// AI 현재 알람 등급
	char    last_status;		// AI 가장 최근 알람 등급
	float   curr_val;			// AI 현재 측정 값
	time_t	tm_alarm;			// 알람 시작 시간
	time_t	tm_save;			// 데이터 저장한 시간
	time_t	tm_update;			// 데이터 업데이트한 시간
	struct	th_ai *_th_ai;
    struct  data_oid dataOid;	// SNMP 사용

};

struct th_ai
{
	int		thseq;
	float	high;
	float	low;
};

struct equip_di 
{
	int     tseq;				// AI 센서 인덱스(TAG_SEQ)
	//char    type;				// AI (STS, ALM, SET)
	char	tag_nm[128];		// 태그 이름
	char	tid[64];			// 태그 modbus 번지수
	char	data_type;			// 데이터 자료형 (I, F)
	float	rate;				// 데이터 단위
	char    use_yn;				// AI 사용 여부(Y, N)
	char    alarm_yn;			// AI 알람 사용 여부(Y, N)
	char    sms_yn;				// AI sms 사용 여부(Y, N)
	char    save_yn;			// AI 데이터 저장 사용 여부(Y, N)
	unsigned int save_term;		// 데이터 저장 주기
	unsigned int update_term;	// 데이터 업데이트 주기
	int		alarm_term;			// 알람 지속 주기
	int		data_add;			// 데이터 보정
	
	int		send_flag;
	char	critical_alarm_yn;
	char	critical_sms_yn;
	int		normal_val;
	int		alarm_val;

	char    curr_status;		// DI 현재 알람 등급(A, N)
	char    last_status;		// DI 가장 최근 알람 등급
	int		curr_val;			// DI 현재 측정 값
	time_t	tm_alarm;			// 알람 시작 시간
	time_t	tm_save;			// 데이터 저장한 시간
	time_t	tm_update;			// 데이터 업데이트한 시간
	struct	th_di *_th_di;	
    struct  data_oid dataOid;	// SNMP 사용
};

struct th_di
{
	int		thseq;
	float	alarm_val;
	float	normal_val;
};
struct equip_do 
{
    int     idx;				// DO 센서 인덱스(DO01)
    char    stat;				// DO ON/OFF
    char    sensor_id[32];		// DO 센서 이름
};

#endif /* _STRUCT_H_ */
