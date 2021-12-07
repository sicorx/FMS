#ifndef _AIRDEF_H_
#define _AIRDEF_H_ 1

/*모델 정의*/
#define UNKNOWN					0x0000	/* 정의되지 않은 항온항습기 */
#define MST						0x0201	/* 서의(MST) 항온항습기     */
#define NEMO					0x0202	/* NEMO 항온항습기          */
#define AR						0x0203	/* AR 항온항습기            */
#define AR_OLD					0x0204	/* AR 구형 항온항습기       */
#define SVU						0x0205	/* 범양(SVU) 항온항습기     */
#define DDC_SHINSUNG			0x0206	/* DDC(컨트롤러) 신성 항온항습기 */
#define SYSTRONIC_P				0x0207	/* 시스트로닉스 컨트롤러 P 타입 프로토콜 */
#define SYSTRONIC_I				0x0208	/* 시스트로닉스 컨트롤러 I 타입 프로토콜 */
#define DDC_HANGUL				0x0209	/* DDC(컨트롤러) 한글일체형 항온항습기 */
#define NEMO_ECU				0x020A	/* NEMO 일반형 항온항습기   */
#define AR_HP8KB				0x020B	/* AR항온항습기 롬번호:ARHP8KB  */
#define AR_SPHCEX				0x020C	/* AR항온항습기 롬번호:ARSPHCEX */
#define AR_HANP08				0x020D	/* AR항온항습기 롬번호:ARHANP08 */
#define AR_HP8EPQ				0x020E	/* AR항온항습기 롬번호:ARHP8EPQ */
#define AR_HP8EPF				0x020F	/* AR항온항습기 롬번호:ARHP8EPF */
#define AR_HANE08				0x0210	/* AR항온항습기 롬번호:ARHANE08 */
#define AR_STHCE				0x0211	/* AR항온항습기 롬번호:ARSTHCE  */
#define FAC1000					0x0212	/* FAC1000 항온항습기 */
#define WEBPED					0x0213	/* 탄약(KIDA) LG CNS 미들웨어(USN data) */
#define AR_HANE09				0x0214	/* AR항온항습기 롬번호:ARHANE09 */
#define AR_MODBUS				0x0215	/* AR항온항습기 MODBUS PROTOCOL */
#define AR_SSHP8Y1				0x0216	/* AR항온항습기 롬번호:SSHPY\8Y1(ARHANE09와 거의 비슷함) */
#define DDC_HANGUL2				0x0217	/* DDC(컨트롤러) 한글일체형 항온항습기 ver 2 */
#define SEUNGIL					0x0218	/* 승일전자 컨트롤러 version2006-09-19, KT영동 */
#define SYSKOREA				0x0219	/* SYSKOREA   */
#define BY_CRC2004				0x021A	/* 범양 CRC-2004 */
#define DY_SS2000				0x021B	/* 서우항온항습기, 대연MUX COM포트에 연결된 장비 타입 */
#define LIEBERT_CHTP			0x021C	/* Liebert CHTP units */
#define AR_HANE09_DYRCU			0x021D	/* AR항온항습기 롬번호:ARHANE09 대연MUX 연결타입 */
#define AR_MODBUS_DYRCU			0x021E	/* AR항온항습기 MODBUS PROTOCOL 대연MUX 연결타입 */
#define AR_HANE09_Q				0x021F	/* AR항온항습기 롬번호:ARHANE09 명령어 a?? 연결타입 */
#define SEUNGIL_MLTM			0x0220	/* 승일전자 컨트롤러 version2009-03-31, 국토해양부 */
#define DX100					0x0221	/* 두텍 DX100, 우리은행 */
#define MeX100					0x0222	/* MeX100, KT영동 증설 */
#define DX100_SG1040			0x0223	/* 두텍 DX100, 우리은행. 시스템베이스 디바이스 서버 SG-1040연결 타입 */
#define MST_ASCII				0x0224	/* MST 컨트롤러, ASCII 타입 */
#define MST_HMM21				0x0225	/* MST 컨트롤러, 현대상선 중앙AR 항온항습기 */
#define MST_COOLER				0x0226	/* MST 컨트롤러, 현대상선 중앙AR 냉방기 */
#define AR_HANE09_EXOR			0x0227	/* AR항온항습기 신형(56Byte), CRC = EXOR */
#define AR_36BYTE				0x0228	/* AR항온항습기 36Byte 프로토콜 */
#define OC485					0x0229	/* OC485 Modbus */
#define SEUNGIL_STHC_MB_FW36	0x022A	/* 뀌두라미 항온항습기 */
#define SNMP_LIEBERT_22005		0x022B	/* 리버트 항온항습기 - 펌웨어버전 2.200.5 */
#define SNMP_LIEBERT_30100		0x022C	/* 리버트 항온항습기 - 펌웨어버전 3.010.0 */
#define SOLID					0x022D	/* 솔리드 항온항습기 */
#define STHAV_MB_R1				0x022E	/* 귀뚜라미 항온항습기 */

#define RTU_AR_MODBUS			0x022F	/* RTU AR MODBUS */
#define RTU_LIEBERT_CHTP		0x0230	/* RTU Liebert chtp */
#define RTU_OC485				0x0231	/* RTU OC485 */
#define RTU_AR_HANE09			0x0232	/* RTU AR_HANE09 */
#define RTU_AR_HANE09_Q			0x0233	/* RTU AR_HANE09_Q */
#define DDC400					0x0234	/* 시스토로닉스 */
#define DDC_UNKNOWN				0x0235	/* 시스토로닉스 LG(고객센터 사용) DDC버전 모름 */


/* 서의(MST) 제어명령 타입 */
#define MST_STX             0x1F	/* 시작 신호 */
#define MST_ETX             0x03	/* 종료 신호 */
#define MST_GRUP_NUM        0x01	/* 그룹 번호, 통상 01h로 설정(50대 이상 제어할 때만 그룹 설정) */
#define MST_SET_TEMP        0x20	/* 온도 설정 */
#define MST_SET_COLD_DEC    0x21	/* 냉방 편차 */
#define MST_SET_WARM_DEC    0x22	/* 난방 편차 */
#define MST_SET_HUMIDITY    0x23	/* 습도 설정 */
#define MST_SET_HUMI_DEC    0x24	/* 가습 편차 */
#define MST_SET_DEHUMI_DEC  0x25	/* 제습 편차 */
#define MST_PWR_FAIL_RETURN 0x26	/* 정전 복귀 */
#define MST_SUSPEND_DELAY   0x27	/* 정지 지연 */
#define MST_REQ_ALL_DATA    0x10	/* 모든데이타 요청 */
#define MST_ON_CMD          0x11	/* 가동 명령 */
#define MST_OFF_CMD         0x12	/* 정지 명령 */
#define MST_RES_SIZE        0x1C	/* 항온항습기에서 송신해주는 데이타 크기 */

/* NEMO 제어명령 타입 */
#define NEMO_STX             0x3F    /* 시작 신호 */
#define NEMO_ETX             0x24    /* 종료 신호 */
#define NEMO_RES_SIZE        0x30    /* 항온항습기에서 송신해주는 데이타 크기 */

/* AR 제어명령 타입 */
#define AR_REQ_CMD          'Q'
#define AR_ON_CMD           'O'     /* ON 명령   */
#define AR_OFF_CMD          'X'     /* OFF 명령  */
#define AR_RES_SIZE         0x26    /* 38 */

/* AR 구형 제어명령 타입 */
#define AR_OLD_REQ_CMD      '?'
#define AR_OLD_RES_SIZE     0x34    /* 52 */
#define AR_OLD_ON           '1'
#define AR_OLD_OFF          '0'

/* SVU 제어명령 타입 */
#define SVU_SET_TEMP_DB     0x81    /* 희망온도 DB  */
#define SVU_SET_TEMP_PB     0x82    /* 희망온도 PB  */
#define SVU_SET_SHUMI       0x83    /* 희망습도 CPA */
#define SVU_SET_HUMI_DB     0x84    /* 희망습도 DB  */
#define SVU_SET_HUMI_PB     0x85    /* 희망습도 DB  */
#define SVU_SET_MODE        0x86    /* 운전 MODE    */
#define SVU_SET_STATE       0x87    /* (가동/정지)  */
#define SVU_SET_STEMP       0x88    /* 희망온도     */
/* SVU 요청명령 타입 */
#define SVU_REQ_TEMP_DB     0x01    /* 희망온도 DB  */
#define SVU_REQ_TEMP_PB     0x02    /* 희망온도 PB  */
#define SVU_REQ_SHUMI       0x03    /* 희망온도 SPA */
#define SVU_REQ_HUMI_DB     0x04    /* 희망습도 DB  */
#define SVU_REQ_HUMI_PB     0x05    /* 희망습도 PB  */
#define SVU_REQ_MODE        0x06    /* 운전 모드    */
#define SVU_REQ_STATE       0x07    /*  RUN/ STOP   */
#define SVU_REQ_STEMP       0x08    /* 희망 온도 CPA(2 바이트) */
#define SVU_REQ_CONMODE     0x0A    /* 공조제어상태 */
#define SVU_REQ_RHUMI       0x0B    /* 실내 습도    */
#define SVU_REQ_RTEMP       0x0C    /* 실내온도 CPA(2바이트)   */
#define SVU_REQ_OUTBUF0     0x0E    /* 출력 상태    */
#define SVU_REQ_OUTBUF1     0x0F    /* 출력 상태    */
#define SVU_REQ_ERR00       0x10    /* ERROR 상태   */
#define SVU_REQ_ERR01       0x11    /* ERROR 상태   */
#define SVU_RES_SIZE        0x02    /* 항온항습기에서 송신해주는 데이타 크기 */

/* (DDC) 요청명령 타입 - 공통사용 */
#define DDC_STX                 0x02    /* start of message      */
#define DDC_REQ_DATA_TYPE       0xFF01  /* 데이터 요청 명령 타입        */
#define DDC_REQ_TIMER_TYPE      0xFF10  /* 타이머 설정값 요청 명령 타입 */
#define DDC_SET_TEMP            0x14    /* 데이터 설정 명령 - 온도  */
#define DDC_SET_HUMI            0x1A    /* 데이터 설정 명령 - 습도  */
#define DDC_SET_COOL_DECLINAT   0x1D    /* 데이터 설정 명령 - 냉각편차  */
#define DDC_SET_HEAT_DECLINAT   0x3B    /* 데이터 설정 명령 - 난방 편차 */
#define DDC_SET_DEHUMI_DECLINAT 0x1C    /* 데이터 설정 명령 - 제습편차  */
#define DDC_SET_HUMI_DECLINAT   0x0F    /* 데이터 설정 명령 - 가습편차  */
#define DDC_SET_REHEAT_DECLINAT 0x1E    /* 데이터 설정 명령 - 재열편차  */
#define DDC_SET_COMP1_DELAY     0x0A    /* 타이머 설정 명령 - 1호기 압축기 지연시간 */
#define DDC_SET_COMP2_DELAY     0x0D    /* 타이머 설정 명령 - 2호기 압축기 지연시간 */
#define DDC_SET_FAN_DELAY       0x2C    /* 타이머 설정 명령 - FAN Moter 지연시간 */
#define DDC_SET_ON              0x11    /* 데이터 설정 명령 - 냉방기 운전 */
#define DDC_SET_OFF             0x12    /* 데이터 설정 명령 - 냉방기 정지 */
#define DDC_REQ_MT              0x91    /* 메세지 타입(데이터 Request 명령) */
#define DDC_SET_MT              0x90    /* 메세지 타입(데이터 Set 명령)     */
#define DDC_FR                  0x77    /* Framing Byte*/
#define DDC_DATA_TYPE_SINGLE    0x01    /* 데이터 타입 - Single Byte Object */
#define DDC_DATA_TYPE_WORD      0x02    /* 데이터 타입 - Two Byte Object    */
#define DDC_DATA_TYPE_STRING    0x06    /* 데이터 타입 - 6 Character String */
#define DDC_DATA_RES_SIZE       0x50    /* 데이터 요청 명령에 대한 응답 바이트 */
#define DDC_TIMER_RES_SIZE      0x16    /* 타이머 설정값 요청에 대한 응답 바이트 */

/* DDC 신성항온항습기 */
#define DDC_SHINSUNG_DATA_RES_SIZE  52  /* 신성항온항습기 데이터 요청 명령에 대한 응답 바이트 */

/* DDC 한글일체형 항온항습기 */
#define DDC_HANGUL_DATA_RES_SIZE    80  /* 한글일체형 항온항습기 데이터 요청 명령에 대한 응답 바이트 */

/* DDC 한글일체형 항온항습기 Ver2 데이터 설정명령 */
#define DDC2_SET_TEMP            0x00    /* 데이터 설정 명령 - 온도  */
#define DDC2_SET_HUMI            0x01    /* 데이터 설정 명령 - 습도  */
#define DDC2_SET_COOL_DECLINAT   0x02    /* 데이터 설정 명령 - 냉각편차  */
#define DDC2_SET_HEAT_DECLINAT   0x03    /* 데이터 설정 명령 - 난방 편차 */
#define DDC2_SET_DEHUMI_DECLINAT 0x04    /* 데이터 설정 명령 - 제습편차  */
#define DDC2_SET_HUMI_DECLINAT   0x07    /* 데이터 설정 명령 - 가습편차  */
#define DDC2_SET_COMP1_DELAY     0x08    /* 타이머 설정 명령 - 1호기 압축기 지연시간 */
#define DDC2_SET_COMP2_DELAY     0x09    /* 타이머 설정 명령 - 2호기 압축기 지연시간 */
#define DDC2_SET_FAN_DELAY       0x0A    /* 타이머 설정 명령 - FAN Moter 지연시간 */
#define DDC2_SET_COMP_CYCLE      0x0B    /* COMP 싸이클 설정 명령*/
#define DDC2_SET_HEAT_STEP       0x0C    /* 히트스텝  설정 명령*/
#define DDC2_SET_ON_OFF          0x1B    /* 항온항습기 ON/OFF */
#define DDC2_SET_RES_SIZE        0x0F    /* 항온항습기 SET RES SIZE */

/* 시스트로닉스 (P형) 제어명령 타입 */
#define SYSTRONIC_P_REQ_CMD     '?'     /* 모든 데이터 송신 요청 명령 */
#define SYSTRONIC_P_ON_CMD      '\\'    /* ON 명령   */
#define SYSTRONIC_P_OFF_CMD     '/'     /* OFF 명령  */
#define SYSTRONIC_P_RES_SIZE    0x26    /* 38        */

/* 시스트로닉스 (I형) 제어명령 타입 */
#define SYSTRONIC_I_REQ_CMD     '?'     /* 모든 데이터 송신 요청 명령 */
#define SYSTRONIC_I_RES_SIZE    0x30    /* 48        */

/* AR항온항습기 롬번호:ARHP8KB 제어명령 타입 */
#define AR_HP8KB_REQ_CMD    AR_REQ_CMD
#define AR_HP8KB_ON_CMD     AR_ON_CMD
#define AR_HP8KB_OFF_CMD    AR_OFF_CMD
#define AR_HP8KB_RES_SIZE   AR_RES_SIZE

/* AR항온항습기 롬번호:ARSPHCEX 제어명령 타입 */
#define AR_SPHCEX_REQ_CMD   AR_OLD_REQ_CMD
#define AR_SPHCEX_RES_SIZE  AR_OLD_RES_SIZE
#define AR_SPHCEX_ON        AR_OLD_ON
#define AR_SPHCEX_OFF       AR_OLD_OFF

/* AR항온항습기 롬번호:ARHANP08 제어명령 타입 */
#define AR_HANP08_REQ_CMD   SYSTRONIC_P_REQ_CMD
#define AR_HANP08_ON_CMD    SYSTRONIC_P_ON_CMD
#define AR_HANP08_OFF_CMD   SYSTRONIC_P_OFF_CMD
#define AR_HANP08_RES_SIZE  SYSTRONIC_P_RES_SIZE

/* AR항온항습기 롬번호:ARHP8EPQ 제어명령 타입 */
#define AR_HP8EPQ_REQ_CMD   '?'
#define AR_HP8EPQ_ON_CMD    '\\'
#define AR_HP8EPQ_OFF_CMD   '/'
#define AR_HP8EPQ_RES_SIZE  0x26

/* AR항온항습기 롬번호:ARHP8EPF 제어명령 타입 */
#define AR_HP8EPF_REQ_CMD   AR_HP8EPQ_REQ_CMD
#define AR_HP8EPF_ON_CMD    AR_HP8EPQ_ON_CMD
#define AR_HP8EPF_OFF_CMD   AR_HP8EPQ_OFF_CMD
#define AR_HP8EPF_RES_SIZE  AR_HP8EPQ_RES_SIZE

/* AR항온항습기 롬번호:ARHANE08 제어명령 타입 */
#define AR_HANE08_REQ_CMD   '?'
#define AR_HANE08_ON_CMD    '\\'
#define AR_HANE08_OFF_CMD   '/'
#define AR_HANE08_RES_SIZE  0x26

/* AR항온항습기 롬번호:ARSTHCE 제어명령 타입 */
#define AR_STHCE_REQ_CMD   '?'
#define AR_STHCE_RES_SIZE   0x07

/* AR항온항습기 롬번호:ARHANE09 제어명령 타입 */
#define AR_HANE09_REQ_CMD   '?'
#define AR_HANE09_ON_CMD    '\\'
#define AR_HANE09_OFF_CMD   '/'
#define AR_HANE09_RES_SIZE  0x38 /* 56 */

/* AR항온항습기 롬번호:SSHP8Y1 제어명령 타입 */ 
#define AR_SSHP8Y1_REQ_CMD   '?'
#define AR_SSHP8Y1_ON_CMD    '\\'
#define AR_SSHP8Y1_OFF_CMD   '/'
#define AR_SSHP8Y1_RES_SIZE  0x38 /* 56 */

/* FAC1000 항온항습기 */
#define FAC1000_STX         0x02
#define FAC1000_BLK_HEAD    'T'
#define FAC1000_ETX         0x03
#define FAC1000_REQ_CMD     '?'
#define FAC1000_SET_CMD     'S'
#define FAC1000_SET_TEMP            0x2000  /* 설정온도 */
#define FAC1000_SET_HUMID           0x2001  /* 설정습도 */
#define FAC1000_SET_COMP1_DELAY     0x2002  /* 냉방기1 운전 지연시간 */
#define FAC1000_SET_COMP2_DELAY     0x2003  /* 냉방기2 운전 지연시간 */
#define FAC1000_SET_BLOWER_DELAY    0x2004  /* BLOWER 정지 지연시간 */
#define FAC1000_SET_COOL_DEC        0x2005  /* 냉방 운전 편차 */
#define FAC1000_SET_HEAT_DEC        0x2006  /* 난방 운전 편차 */
#define FAC1000_SET_HUMID_DEC       0x2007  /* 가습 운전 편차 */
#define FAC1000_SET_DEHUM_DEC       0x2008  /* 제습 운전 편차 */
#define FAC1000_ON_CMD      'O'
#define FAC1000_OFF_CMD     'F'
#define FAC1000_OPR_IDX     4       /* ON, OFF, Setting 리턴 코드의 CMD 인덱스 */
#define FAC1000_REQ_RES_SIZE    46
#define FAC1000_OPR_RES_SIZE    10  /* ON, OFF Command */

/* AR항온항습기 MODBUS타입 제어명령 */
#define AR_MODBUS_REQ_CMD       0x03
#define AR_MODBUS_CTRL_CMD      0x06    /* 제어 명령 타입*/
#define AR_MODBUS_START_ADDR    0xC8    /* 십진수 200 */
#define AR_MODBUS_WORD_CNT      0x3E    /* 십진수 62 */
#define AR_MODBUS_DO_ADDR       0x0100  /* 제어부분의 주소 256 */
#define AR_MODBUS_DO1_ADDR      0x0101  /* 제어부분의 주소 257 */
#define AR_MODBUS_DO_WORD_CNT   0x02

/* SYSKOREA 제어명령 */
#define SYSKOREA_STX         0x02
#define SYSKOREA_ETX         0x03
#define SYSKOREA_REQ_CMD     0xA9
#define SYSKOREA_REQ_RES_SIZE 39

/* 범양CRC2004 제어명령 */
#define BY_CRC2004_REQ_CMD      0x33
#define BY_CRC2004_SET_CMD      0x44
#define BY_CRC2004_REQ_RES_SIZE 30

#define MST_ASCII_RES_CNT       52
#define MST_HMM21_RES_CNT       MST_ASCII_RES_CNT

#define SEUNGIL_STHC_MB_FW36_RES_CNT    0x10E   /* 15+0xFE+1 */

#define SOLID_START_ADDR	0x08
#define SOLID_REQ_SIZE		0x26
#endif /* _AIRDEF_H_ */