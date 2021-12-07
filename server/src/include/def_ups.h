#ifndef _DEF_UPS_H_
#define _DEF_UPS_H_ 1

#define UPS_MAXCOMM							0x0101	//맥스컴 UPS
#define UPS_LIEBERT							0x0102	//Liebert UPS
#define UPS_SUPEREX							0x0103	//SUPEREX UPS
#define UPS_LORTEC							0x0104	//LORTEC UPS(BP UPS)
#define UPS_MEGATEC							0x0105	//인천교육청 UPS
#define UPS_SEC								0x0106	//인천공항 UPS
#define UPS_DAINONG							0x0107	//대농UPS(DMB부송출국)
#define UPS_IMV								0x0108	//IMV UPS
#define UPS_DELPHYS							0x0109	//델피스 UPS
#define UPS_UPSILON							0x010A	//UPSILON STS
#define UPS_PWARE9305						0x010B	//Porwerware 9305
#define UPS_PWARE9315						0x010C	//Porwerware 9315
#define UPS_PWARE9390						0x010D	//Porwerware 9390
#define UPS_SUPER_SWITCH3					0x010E	//SUPER_SWITCH(SS3) STS
#define UPS_MGE_GTC							0x010F	//MGE UPS (통신보드 : GTClink)
#define UPS_MEGATEC_EX						0x0110	//인천교육청 UPS MEGATEC 확장(Extended) 팩
#define UPS_APC_MW							0x0111	//APC SymmetraMW UPS
#define UPS_APC_MW_EBS						0x0112	//APC SymmetraMW EBS(SSW)
#define UPS_GLOBAL_PTX_4K					0x0113	//GLOBAL PTX 4000
#define UPS_HIPULSE_U						0x0114	//Lievert HiPULSE U UPS
#define UPS_AROS_SENTRY						0x0115	//AROS-SENTRY UPS
#define UPS_SUPER_SWITCH3_MTCP				0x0116	//SUPER_SWITCH(SS3) STS, Modbus TCP
#define UPS_Neutral_R00						0x0117	//국토해양부 Neutral R00
#define UPS_APC_SMARTROL					0x0118	//우리은행 APC UPS Smartrol 연동
#define UPS_SMM_SICE7200nHIPULSE			0x0119	//SiteScan SICE7200 & HiPulse UPS

#define SNMP_NETAGENT_MINI_1PHASE			0x0181	// Mega System Technologies, inc
#define SNMP_RDU_SIC_G2						0x0182
#define SNMP_MGE_GALAXY_5000				0x0183
#define SNMP_HIPULSE_U						0x0184
#define SNMP_EMERSON_APM					0x0185
#define SNMP_HIPULSE_V1310					0x0186
#define SNMP_TAEIL_MARCHE					0x0187
#define SNMP_MGE_GALAXY_VM					0x0188
#define SNMP_EMERSON_NXR					0x0189

/* UPS_MGE_GTC 에 사용되는 타입 정의 */
#define MGE_GTC_CURRENT_FIELD_ADDR          0x0100
#define MGE_GTC_VOLTAGE_FIELD_ADDR          0x0140
#define MGE_GTC_POWER_FIELD_ADDR            0x0180
#define MGE_GTC_FREQUENCY_FIELD_ADDR        0x01A0
#define MGE_GTC_BATTERY_FIELD_ADDR          0x01C3
#define MGE_GTC_CURRENT_FIELD_WORD_CNT      0x24
#define MGE_GTC_VOLTAGE_FIELD_WORD_CNT      0x16
#define MGE_GTC_POWER_FIELD_WORD_CNT        0x0A
#define MGE_GTC_FREQUENCY_FIELD_WORD_CNT    0x03
#define MGE_GTC_BATTERY_FIELD_WORD_CNT      0x01
#define MGE_GTC_STATUS_FIELD_ADDR           0x0040
#define MGE_GTC_STATUS_FIELD_WORD_CNT       0x0F

/* Powerware 9390 UPS에 사용되는 타입 정의 */
#define PWARE9390_METER_CMD_TYPE    0x34    /* UPS 계측 데이타 타입 */
#define PWARE9390_ALARM_CMD_TYPE    0x35    /* UPS 경보 데이타 타입 */
#define PWARE9390_STATUS_CMD_TYPE   0x33    /* UPS 상태 데이타 타입 */
#define PWARE9390_RES_H_SIZE        0x04    /* */
#define PWARE9390_SFD               0xAB    /* Start Of Frame */

/* Powerware 9305 UPS에 사용되는 타입 정의 */
#define PWARE9305_STATUS_CMD_TYPE   'S'
#define PWARE9305_MEASURE_CMD_TYPE  'M'
#define PWARE9305_STATUS_CMD        "UPSS\r"    /* UPS 상태 데이타 타입 */
#define PWARE9305_MEASURE_CMD       "UPDS\r"    /* UPS 상태/계측 데이타 타입 */
#define PWARE9305_STATUS_MIN_SIZE   68
#define PWARE9305_STATUS_MAX_SIZE   98
#define PWARE9305_MEASURE_MIN_SIZE  468
#define PWARE9305_MEASURE_MAX_SIZE  498

/* Powerware 9315 UPS에 사용되는 타입 정의 */
#define PWARE9315_METER_CMD_TYPE    0x34    /* UPS 계측 데이타 타입 */
#define PWARE9315_ALARM_CMD_TYPE    0x35    /* UPS 경보 데이타 타입 */
#define PWARE9315_STATUS_CMD_TYPE   0x33    /* UPS 상태 데이타 타입 */
#define PWARE9315_RES_H_SIZE        0x04    /* */
#define PWARE9315_SFD               0xAB    /* Start Of Frame */

/* Upsilon STS 제어명령 타입 */
#define UPSILON_READ_CMD            0x03
#define UPSILON_STATUS_ADDR         0x1000
#define UPSILON_STATUS_WORD_CNT     4
#define UPSILON_MEASURE_ADDR        0x1100
#define UPSILON_MEASURE_WORD_CNT    58

/* MAXCOMM 제어명령 타입 */
#define MAXCOMM_READ_CMD_TYPE       0x03    /* 맥스컴 계측 데이타 읽기                  */
#define MAXCOMM_ANALOG_WORD_CNT     0x1E    /* 맥스컴 아날로그 타입 데이타 Word Count   */
                                            /* Starting address 0001 ~ 0030             */
#define MAXCOMM_ANALOG_START_ADDR   0x00    /* 맥스컴 아날로그 타입 데이타 시작 Address */
#define MAXCOMM_DIGITAL_WORD_CNT    0x02    /* 맥스컴 디지털타입 데이타 Word Count      */
#define MAXCOMM_DIGITAL_START_ADDR  0x41    /* 맥스컴 아날로그 타입 데이타 시작 Address */
#define MAXCOMM_ANALOG              0x01    /* 아날로그타입 데이타                      */
#define MAXCOMM_DIGITAL             0x02    /* 디지털타입 데이타                        */
#define MAXCOMM_ANALOG_RES_SIZE     0x41    /* 아날로그타입 리턴 데이타 사이즈          */
#define MAXCOMM_DIGITAL_RES_SIZE    0x09    /* 디지털타입 리턴 데이타 사이즈            */

/* Liebert 제어명령 타입 */
#define LIEBERT_READ_CMD_TYPE       0x03    /* 리버트 데이타 읽기   */
#define LIEBERT_ALARM_ADDR          40030   /* 리버트 알람 Address */
#define LIEBERT_MEASURE_ADDR        40040   /* 리버트 계측값 Address */
#define LIEBERT_STATUS_ADDR         40007   /* 리버트 상태 Address */
#define LIEBERT_ALARM_WORD_CNT      0x06    /* 리버트 알람 Word Count, 40030 ~ 40035 */
#define LIEBERT_MEASURE_WORD_CNT    0x1D    /* 리버트 계측값 타입 Word Count, 40040 ~ 40068 */
#define LIEBERT_STATUS_WORD_CNT     0x02    /* 리버트 상태 타입 Word Count, 40007 ~ 40008 */
#define LIEBERT_ALARM               0x01    /* 알람 타입 데이타 */
#define LIEBERT_MEASURE             0x02    /* 계측 타입 데이타 */
#define LIEBERT_STATUS              0x03    /* 상태 타입 데이타 */

/* SUPEREX 제어명령 타입 */
#define SUPEREX_STX                 0x02
#define SUPEREX_EXT                 0x03
#define SUPEREX_GET_ANALOG_CMD      1
#define SUPEREX_GET_STATUS_CMD      2
#define SUPEREX_ANALOG_RES_SIZE     98
#define SUPEREX_STATUS_RES_SIZE     16

/* LORTEC 제어명령 타입 */
#define LORTEC_GET_ANALOG           0x01
#define LORTEC_GET_RELAY            0x02
#define LORTEC_GET_CMD              "P0"
#define LORTEC_RES_CMD              "P1"
#define LORTEC_ERR_CMD              "P8"
#define LORTEC_END_CMD              "P9"
#define LORTEC_ANALOG_CMD           "000001"
#define LORTEC_RELAY_CMD            "000002"

/* SEC 제어명령 타입 ( ST는 생략)*/
#define SEC_HEADER              '^'
#define SEC_POLL_CMD            'P'
#define SEC_DELIMIT             ','
#define SEC_BATTERY_GROUP       0x01
#define SEC_INPUT_GROUP         0x02
#define SEC_OUTPUT_GROUP        0x03
#define SEC_BYPASS_GROUP        0x04
#define SEC_ALARM_GROUP         0x05
#define SEC_BATTERY_REQ_SIZE    0x25    /* 32+5 */
#define SEC_INPUT_REQ_SIZE      0x46    /* 65+5 */
#define SEC_OUTPUT_REQ_SIZE     0x48    /* 67+5 */ /* Data Length 문서상 66 byte */
#define SEC_BYPASS_REQ_SIZE     0x3A    /* 53+5 */
#define SEC_ALARM_REQ_SIZE      0x22    /* 29+5 */

/* MEGATEC 제어명령 타입 */
#define MEGATEC_CR              0x0D
#define MEGATEC_GET_STATUS      0x01
#define MEGATEC_STATUS          "Q1"
#define MEGATEC_REQ_SIZE        0x2E /* 46*/

/* MEGATEC EX 제어명령 타입 */
#define MEGATEC_EX_GLOBAL_CMD   'G'
#define MEGATEC_EX_STATUS_CMD   'Q'
#define MEGATEC_EX_3PHASE_RES_SIZE        0x49    /* G3 : 3상 Data size - 72bytes + <CR> */
#define MEGATEC_EX_SINGLE_PHASE_RES_SIZE  0x2F    /* Q1 : 단상 Data size - 46bytes + <CR> */
#define MEGATEC_EX_BATT_RES_SIZE          0x29    /* G1 : 배터리관련 Data size - 39bytes + <CR> */
#define MEGATEC_EX_STATUS_RES_SIZE        0x1C    /* G2 : 기타 상태Data size - 27bytes + <CR> */
#define MEGATEC_EX_CMD_G3                 1
#define MEGATEC_EX_CMD_G2                 3
#define MEGATEC_EX_CMD_G1                 2
#define MEGATEC_EX_CMD_Q1                 4

/* DAINONG UPS 프로토콜에 사용되는 타입 정의 */
#define DAINONG_SOH         0x01
#define DAINONG_EOT         0x04
#define DAINONG_DLE         0x10
#define DAINONG_RES_SIZE    0x4B    /* 75 */

/* IMV UPS에 사용되는 타입 정의 */
#define IMV_TRACK_SRC_ADDR          0x05
#define IMV_QUERY_DST_ADDR          0x85
#define IMV_ETX                     0x03
#define IMV_EOT                     0x04
#define IMV_ENQ                     0x05
#define IMV_ACK                     0x06
#define IMV_NACK                    0x02
#define IMV_BEL                     0x07
#define IMV_ESC                     0x1B
#define IMV_SUB                     0x1A
#define IMV_AUGMENT_VALUE           0x80
#define IMV_TYP_BATT_MIN_REM        0xCF01
#define IMV_TYP_BATT_CHARGE_REM     0xCF02
#define IMV_TYP_BATT_VOL            0xCF03
#define IMV_TYP_BATT_CURR           0xCF04
#define IMV_TYP_BATT_TEMP           0xCF05
#define IMV_TYP_BATT_BUNDLE         0xCF06
#define IMV_TYP_IN_FREQ_R           0xCF07
#define IMV_TYP_IN_FREQ_S           0xCF08
#define IMV_TYP_IN_FREQ_T           0xCF09
#define IMV_TYP_IN_VOL_R            0xCF10
#define IMV_TYP_IN_VOL_S            0xCF11
#define IMV_TYP_IN_VOL_T            0xCF12
#define IMV_TYP_IN_CURR_R           0xCF13
#define IMV_TYP_IN_CURR_S           0xCF14
#define IMV_TYP_IN_CURR_T           0xCF15
#define IMV_TYP_IN_TRUE_PWR_R       0xCF16
#define IMV_TYP_IN_TRUE_PWR_S       0xCF17
#define IMV_TYP_IN_TRUE_PWR_T       0xCF18
#define IMV_TYP_IN_VOL_MIN_R        0xCF19
#define IMV_TYP_IN_VOL_MIN_S        0xCF20
#define IMV_TYP_IN_VOL_MIN_T        0xCF21
#define IMV_TYP_IN_VOL_MAX_R        0xCF22
#define IMV_TYP_IN_VOL_MAX_S        0xCF23
#define IMV_TYP_IN_VOL_MAX_T        0xCF24
#define IMV_TYP_IN_BUNDLE           0xCF25
#define IMV_TYP_OUT_FREQ            0xCF26
#define IMV_TYP_OUT_VOL_R           0xCF27
#define IMV_TYP_OUT_VOL_S           0xCF28
#define IMV_TYP_OUT_VOL_T           0xCF29
#define IMV_TYP_OUT_CURR_R          0xCF30
#define IMV_TYP_OUT_CURR_S          0xCF31
#define IMV_TYP_OUT_CURR_T          0xCF32
#define IMV_TYP_OUT_PWR_R           0xCF33
#define IMV_TYP_OUT_PWR_S           0xCF34
#define IMV_TYP_OUT_PWR_T           0xCF35
#define IMV_TYP_OUT_LOAD_R          0xCF36
#define IMV_TYP_OUT_LOAD_S          0xCF37
#define IMV_TYP_OUT_LOAD_T          0xCF38
#define IMV_TYP_OUT_BUNDLE          0xCF39
#define IMV_TYP_BYPASS_FREQ         0xCF40
#define IMV_TYP_BYPASS_VOL_R        0xCF41
#define IMV_TYP_BYPASS_VOL_S        0xCF42
#define IMV_TYP_BYPASS_VOL_T        0xCF43
#define IMV_TYP_BYPASS_CURR_R       0xCF44
#define IMV_TYP_BYPASS_CURR_S       0xCF45
#define IMV_TYP_BYPASS_CURR_T       0xCF46
#define IMV_TYP_BYPASS_PWR_R        0xCF47
#define IMV_TYP_BYPASS_PWR_S        0xCF48
#define IMV_TYP_BYPASS_PWR_T        0xCF49
#define IMV_TYP_BYPASS_BUNDLE       0xCF50
#define IMV_TYP_ALARM               0xCF51
#define IMV_TYP_UNKNOWN             0x1828

/* 델피스 UPS에 사용되는 타입 정의 */
#define DELPHYS_READ_CMD_TYPE       0x03
#define DELPHYS_IDENTI_ADDR         0x000
#define DELPHYS_STATUS_ADDR         0x020
#define DELPHYS_ALARMS_ADDR         0x040
#define DELPHYS_MEASUR_ADDR         0x060   /* to 0x08F */
#define DELPHYS_IDENTI_REQ_SIZE     12      /* Word count */
#define DELPHYS_STATUS_REQ_SIZE     4       /* Word count */
#define DELPHYS_ALARMS_REQ_SIZE     4       /* Word count */
#define DELPHYS_MEASUR_REQ_SIZE     48      /* Word count */
#define DELPHYS_IDENTI_RES_SIZE     ((DELPHYS_IDENTI_REQ_SIZE*2)+5)
#define DELPHYS_STATUS_RES_SIZE     ((DELPHYS_STATUS_REQ_SIZE*2)+5)
#define DELPHYS_ALARMS_RES_SIZE     ((DELPHYS_ALARMS_REQ_SIZE*2)+5)
#define DELPHYS_MEASUR_RES_SIZE     ((DELPHYS_MEASUR_REQ_SIZE*2)+5)

#endif /* _DEF_UPS_H_ */
