#ifndef _LEAKDEF_H_
#define _LEAKDEF_H_ 1

/* 누수감지모듈 모델 */
#define NDTECH				0x0901    /* ND Tech 누수감지지   */
#define MISA				0x0902    /* 미사ENG 누수감지지   */
#define SENTRY24			0x0903    /* Sentry24 연동 모듈   */
#define NDTECH_OLD			0x0904    /* ND Tech 구형 누수감지지 */
#define TTDM128				0x0905    /* TraceTek TTDM-128 */
#define NDTECH_MODBUS		0x0906    /* ND Tech Modbus타입 누수감지지 */
#define TTDM128_RECV		0x0907    /* TraceTek TTDM-128 */
#define NDTECH_LDCM100_110	0x0908    /* ND Tech Modbus타입 누수감지지, LDCM-100, 110 통합용 버전 */
#define A_LLM1				0x0909    /* Anying */
#define RTU_TTDM128			0x090A
#define LBSM200				0x090B

/* NDTech 누수감지모듈(LDCM-110) 제어 코드 표 */
#define LDCM_NUM_CMD        0x00    /* NULL          */
#define LDCM_STX_CMD        0x02    /* Start of text */
#define LDCM_ETX_CMD        0x03    /* End of text   */
#define LDCM_RES_SIZE       0x10    /* GM6_SB_CMD에 대한 응답 프레임 길이 */

/* 누수감지모듈(MISA) 제어 코드 표 */
#define MISA_ZONE1_ADDR     0x07A0
#define MISA_ZONE2_ADDR     0x0FA0
#define MISA_ZONE3_ADDR     0x17A0
#define MISA_REQ_CMD_TYPE   0x03
#define MISA_MAX_RES_CNT    0x10
#define MISA_RES_SIZE       5+(2*MISA_MAX_RES_CNT)
#define MISA_PER_CABLE_METER    0x0F

/* NDTech(구형) 누수감지모듈(LDCM-110) 제어 코드 표 */
#define NDTECH_OLD_ENQ      0x05    /* ENQ */
#define NDTECH_OLD_EOT      0x04    /* EOT */
#define NDTECH_OLD_ACK      0x06
#define NDTECH_OLD_NAK      0x15
#define NDTECH_OLD_ACK_SIZE 18
#define NDTECH_OLD_NAK_SIZE 6
#define NDTECH_OLD_REQ      'R'     /* Data Request */
#define NDTECH_OLD_SET      'C'     /* Controal Command */

/* TraceTek TTDM-128  제어 코드 표 */
#define TTDM128_REQ_CMD         0x04
#define TTDM128_START_REG_ADDR  100

/* TraceTek TTDM-128  제어 코드 표 */
#define NDTECH_MODBUS_READ_CMD  0x03
#define NDTECH_MODBUS_READ_REG  0x00
#define NDTECH_MODBUS_WORD_CNT  6

/* Anying A-LLM1  제어 코드 표 */
#define A_LLM1_REQ_CMD			0x04
#define A_LLM1_START_REG_ADDR	0
#define A_LLM1_MODBUS_WORD_CNT	2

#endif /* _LEAKDEF_H_ */
