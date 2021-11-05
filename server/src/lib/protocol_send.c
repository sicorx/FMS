#include "include.h"

extern struct equip_conn_info *conn_info[];
extern struct equip_status *gp_status[];

int make_send_frame(unsigned char *buff, int index, int command_code, int value)
{
    int wbyte = 0;
    int temp_value = 0;
    unsigned char crc8 = 0;
    unsigned short crc16 = 0;
    struct equip_conn_info *pConnInfo = conn_info[index];
	
    switch(pConnInfo->model_seq)
    {
		case MST :
			buff[wbyte++] = MST_STX;        /* 시작신호 */
			buff[wbyte++] = MST_GRUP_NUM;   /* 그룹번호 */
			buff[wbyte++] = (unsigned char)pConnInfo->id;    /* 장비번호 */
			buff[wbyte++] = 0x00;

			/* 개수 영역 */
			if(command_code == MST_REQ_ALL_DATA ||  /* 모든데이타 */
			   command_code == MST_ON_CMD ||        /* 가동 명령  */
			   command_code == MST_OFF_CMD)         /* 정지 명령  */
			{
				buff[wbyte++] = 0x02;
				buff[wbyte++] = (unsigned char)command_code;
			}
			else if(command_code == MST_SET_COLD_DEC    ||  /* 냉방 편차 */
					command_code == MST_SET_WARM_DEC    ||  /* 난방 편차 */
					command_code == MST_SET_HUMI_DEC    ||  /* 가습 편차 */
					command_code == MST_SET_DEHUMI_DEC  ||  /* 제습 편차 */
					command_code == MST_PWR_FAIL_RETURN ||  /* 정전 복귀 */
					command_code == MST_SUSPEND_DELAY)      /* 정지 지연 */
			{
				buff[wbyte++] = 0x03;
				buff[wbyte++] = (unsigned char)command_code;
				buff[wbyte++] = (unsigned char)(value & 0x000000FF);
			}
			else if(command_code == MST_SET_TEMP ||     /* 온도 설정 */
					command_code == MST_SET_HUMIDITY)   /* 습도 설정 */
			{
				buff[wbyte++] = 0x04;
				buff[wbyte++] = (unsigned char)command_code;
				buff[wbyte++] = (unsigned char)((value & 0x0000FF00) >> 8);
				buff[wbyte++] = (unsigned char) (value & 0x000000FF);
			}

			crc8 = exelusive_or_crc_gen(buff, wbyte);
			buff[wbyte++] = crc8;
			buff[wbyte++] = MST_ETX;
			break;

		case MST_ASCII :
		case MST_HMM21 :
		case MST_COOLER :
			buff[wbyte++] = (unsigned char) pConnInfo->id+0x60;  /* 장비번호 */
			buff[wbyte++] = (unsigned char) command_code;
			if(command_code == 'A') // 운전/정지
			{
				buff[wbyte++] = value == DO_ON ? '+' : '-';
				buff[wbyte++] = '0';
				buff[wbyte++] = '0';
				buff[wbyte++] = '0';
				crc8 = crc_gen(buff, wbyte);
				buff[wbyte++] = get_hex((crc8 & 0xF0)>>4);
				buff[wbyte++] = get_hex( crc8 & 0x0F);
			}
			else if(command_code == 'B' || // 설정온도
					command_code == 'C' || // 설정습도
					command_code == 'D' || // 냉방편차
					command_code == 'E' || // 난방편차
					command_code == 'F' || // 가습편차
					command_code == 'G')   // 제습편차
			{
				buff[wbyte++] = '0';
				buff[wbyte++] = get_hex((value%1000)/100);
				buff[wbyte++] = get_hex((value% 100)/ 10);
				buff[wbyte++] = get_hex( value%  10);
				crc8 = crc_gen(buff, wbyte);
				buff[wbyte++] = get_hex((crc8 & 0xF0) >> 4);
				buff[wbyte++] = get_hex( crc8 & 0x0F);
			}
			else if(command_code == 'H' || // 운전지연
					command_code == 'I')   // 정지지연
			{
				buff[wbyte++] = '0';
				buff[wbyte++] = get_hex((value%10000)/1000);
				buff[wbyte++] = get_hex((value% 1000)/ 100);
				buff[wbyte++] = get_hex( value%  100)/  10;
				crc8 = crc_gen(buff, wbyte);
				buff[wbyte++] = get_hex((crc8 & 0xF0) >> 4);
				buff[wbyte++] = get_hex( crc8 & 0x0F);
			}
			break;

		case NEMO :
		case NEMO_ECU :
			buff[wbyte++] = NEMO_STX;   /* 시작신호 */
			buff[wbyte++] = (unsigned char)pConnInfo->id + 0x40;  /* 장비번호 */
			buff[wbyte++] = 0x2A;
			buff[wbyte++] = NEMO_ETX;
			break;

		case AR :          /* AR 항온항습기에 사용된 컨트롤러는 시스트로닉스 것임 */
		case AR_HP8KB  :   /* version 1.3.28 same as AR */
		case AR_OLD :
		case AR_SPHCEX :   /* version 1.3.28 same as AR_OLD */
		case SYSTRONIC_P : /* 시스트로닉스 P형 컨트롤러 항온항습기 */
		case AR_HANP08 :   /* version 1.3.28 same as SYSTRONIC_P */
		case SYSTRONIC_I : /* 시스트로닉스 I형 컨트롤러 항온항습기 */
		case AR_HP8EPQ :   /* version 1.3.28 */
		case AR_HP8EPF :   /* version 1.3.28 */
		case AR_HANE08 :   /* version 1.3.28 */
		case AR_STHCE  :   /* version 1.3.28 */
		case AR_HANE09 : 
		case AR_SSHP8Y1 :  /* version 1.4.1-Debug */
		case AR_HANE09_Q :
		case AR_HANE09_EXOR :
		case AR_36BYTE:
			buff[wbyte++] = (unsigned char)pConnInfo->id + 'a' - 1;  /* 장비번호 */
			buff[wbyte++] = (unsigned char)command_code;
			buff[wbyte++] = (unsigned char)value;
			break;

		case SVU :
			if(command_code == SVU_REQ_TEMP_DB ||
			   command_code == SVU_REQ_TEMP_PB || 
			   command_code == SVU_REQ_SHUMI   || 
			   command_code == SVU_REQ_HUMI_DB || 
			   command_code == SVU_REQ_HUMI_PB || 
			   command_code == SVU_REQ_MODE    || 
			   command_code == SVU_REQ_STATE   || 
			   command_code == SVU_REQ_STEMP   || 
			   command_code == SVU_REQ_CONMODE || 
			   command_code == SVU_REQ_RHUMI   || 
			   command_code == SVU_REQ_RTEMP   || 
			   command_code == SVU_REQ_OUTBUF0 || 
			   command_code == SVU_REQ_OUTBUF1 || 
			   command_code == SVU_REQ_ERR00   || 
			   command_code == SVU_REQ_ERR01)
			{
				buff[wbyte++] = command_code;
			}
			else if(command_code == SVU_SET_TEMP_DB ||  /* 희망온도 DB */
					command_code == SVU_SET_TEMP_PB ||  /* 희망온도 PB */
					command_code == SVU_SET_SHUMI   ||  /* 희망습도 CPA */
					command_code == SVU_SET_HUMI_DB ||  /* 희망습도 DB */
					command_code == SVU_SET_HUMI_PB ||  /* 희망습도 DB */
					command_code == SVU_SET_MODE    ||  /* 운전 MODE */
					command_code == SVU_SET_STATE)      /* (가동/정지) */
			{
				buff[wbyte++] = command_code;
				buff[wbyte++] = (unsigned char)value;
				buff[wbyte++] = 0x00;
			}
			else if(command_code == SVU_SET_STEMP)      /* 희망온도 */
			{
				buff[wbyte++] = command_code;
				buff[wbyte++] = (unsigned char)((value & 0x000000FF) >> 0);
				buff[wbyte++] = (unsigned char)((value & 0x0000FF00) >> 8);
			}
			break;

		case DDC_SHINSUNG :
			buff[wbyte++] = DDC_STX;    /* 시작신호 */
			buff[wbyte++] = (pConnInfo->id & 0xFF00) >> 8;   /* Destination Address Hi */
			buff[wbyte++] =  pConnInfo->id & 0x00FF;         /* Destination Address Low */
			buff[wbyte++] = 0xFE;       /* Sourcess Address Hi */
			buff[wbyte++] = 0xDE;       /* Sourcess Address Low */
			if(command_code == DDC_REQ_DATA_TYPE)
			{
				buff[wbyte++] = DDC_REQ_MT;
				buff[wbyte++] = DDC_FR;     /* Frame Byte */
				buff[wbyte++] = DDC_DATA_TYPE_WORD; /* Data Type */
				buff[wbyte++] = 0x21;       /* Object Number */
				buff[wbyte++] = 0x00;       /* Starting Index */
				buff[wbyte++] = 0x00;       /* Starting attribute */
				buff[wbyte++] = 0x01;       /* Index count */
				buff[wbyte++] = 0x12;       /* Attribute count, 신성항온항습기는 18개의 어트리뷰트 */
				buff[wbyte++] = 0x24;       /* Data Block(Number of data bytes to be returned */
				crc8 = crc_gen(buff, wbyte);
				buff[wbyte++] = crc8;        /* 체크섬 Sum of STX ~ M7 */
			}
			else if(command_code == DDC_SET_TEMP || 
					command_code == DDC_SET_HUMI || 
					command_code == DDC_SET_COOL_DECLINAT   || 
					command_code == DDC_SET_DEHUMI_DECLINAT || 
					command_code == DDC_SET_HUMI_DECLINAT   || 
					command_code == DDC_SET_REHEAT_DECLINAT || 
					command_code == DDC_SET_COMP1_DELAY     || 
					command_code == DDC_SET_COMP2_DELAY ||
					command_code == DDC_SET_ON ||
					command_code == DDC_SET_OFF)
		   {
				buff[wbyte++] = DDC_SET_MT;
				buff[wbyte++] = DDC_FR;     /* Frame Byte */
				buff[wbyte++] = DDC_DATA_TYPE_WORD; /* Data Type */
				buff[wbyte++] = 0x10;
				if(command_code == DDC_SET_TEMP)            buff[wbyte++] = 0x00;
				if(command_code == DDC_SET_HUMI)            buff[wbyte++] = 0x01;
				if(command_code == DDC_SET_COOL_DECLINAT)   buff[wbyte++] = 0x02;
				if(command_code == DDC_SET_DEHUMI_DECLINAT) buff[wbyte++] = 0x03;
				if(command_code == DDC_SET_HUMI_DECLINAT)   buff[wbyte++] = 0x04;
				if(command_code == DDC_SET_REHEAT_DECLINAT) buff[wbyte++] = 0x05;
				if(command_code == DDC_SET_COMP1_DELAY)     buff[wbyte++] = 0x06;
				if(command_code == DDC_SET_COMP2_DELAY)     buff[wbyte++] = 0x07;
				if(command_code == DDC_SET_ON)              buff[wbyte++] = 0x10;
				if(command_code == DDC_SET_OFF)             buff[wbyte++] = 0x0A;
				buff[wbyte++] = 0x00;
				buff[wbyte++] = 0x01;
				buff[wbyte++] = 0x01;
				buff[wbyte++] = 0x02;
				buff[wbyte++] = 0x77;
				buff[wbyte++] = (value & 0x00FF);
				buff[wbyte++] = (value & 0xFF00) >> 8;
				crc8 = crc_gen(buff, wbyte);
				buff[wbyte++] = crc8;        /* 체크섬 Sum of STX ~ M9 */
			}
			break;

		case DDC_HANGUL :
			buff[wbyte++] = DDC_STX;    /* 시작신호 */
			buff[wbyte++] = (pConnInfo->id & 0xFF00) >> 8;   /* Destination Address Hi */
			buff[wbyte++] =  pConnInfo->id & 0x00FF;         /* Destination Address Low */
			buff[wbyte++] = 0xFE;       /* Sourcess Address Hi */
			buff[wbyte++] = 0xDE;       /* Sourcess Address Low */
			if(command_code == DDC_REQ_DATA_TYPE)
			{
				buff[wbyte++] = DDC_REQ_MT;
				buff[wbyte++] = DDC_FR;             /* Frame Byte */
				buff[wbyte++] = DDC_DATA_TYPE_WORD; /* Data Type */
				buff[wbyte++] = 0x21;       /* Object Number */
				buff[wbyte++] = 0x00;       /* Starting Index */
				buff[wbyte++] = 0x00;       /* Starting attribute */
				buff[wbyte++] = 0x01;       /* Index count */
				buff[wbyte++] = 0x20;       /* Attribute count, 한글일체형 항온항습기는 32개의 어트리뷰트 */
				buff[wbyte++] = 0x40;       /* Data Block(Number of data bytes to be returned */
				crc8 = crc_gen(buff, wbyte);
				buff[wbyte++] = crc8;        /* 체크섬 Sum of STX ~ M7 */
			}
			else if(command_code == DDC_SET_TEMP || 
					command_code == DDC_SET_HUMI || 
					command_code == DDC_SET_COOL_DECLINAT   || 
					command_code == DDC_SET_HEAT_DECLINAT   ||
					command_code == DDC_SET_DEHUMI_DECLINAT || 
					command_code == DDC_SET_HUMI_DECLINAT   || 
					command_code == DDC_SET_COMP1_DELAY     || 
					command_code == DDC_SET_COMP2_DELAY     ||
					command_code == DDC_SET_FAN_DELAY )
		   {
				buff[wbyte++] = DDC_SET_MT;
				buff[wbyte++] = DDC_FR;             /* Frame Byte */
				buff[wbyte++] = DDC_DATA_TYPE_WORD; /* Data Type */
				buff[wbyte++] = 0x10;
				if(command_code == DDC_SET_TEMP)            buff[wbyte++] = 0x00;
				if(command_code == DDC_SET_HUMI)            buff[wbyte++] = 0x01;
				if(command_code == DDC_SET_COOL_DECLINAT)   buff[wbyte++] = 0x02;
				if(command_code == DDC_SET_HEAT_DECLINAT)   buff[wbyte++] = 0x03;
				if(command_code == DDC_SET_DEHUMI_DECLINAT) buff[wbyte++] = 0x04;
				if(command_code == DDC_SET_HUMI_DECLINAT)   buff[wbyte++] = 0x07;
				if(command_code == DDC_SET_COMP1_DELAY)     buff[wbyte++] = 0x09;
				if(command_code == DDC_SET_COMP2_DELAY)     buff[wbyte++] = 0x09;
				if(command_code == DDC_SET_FAN_DELAY)       buff[wbyte++] = 0x0A;
				buff[wbyte++] = 0x00;
				buff[wbyte++] = 0x01;
				buff[wbyte++] = 0x01;
				buff[wbyte++] = 0x02;
				buff[wbyte++] = 0x77;
				buff[wbyte++] = (value & 0x00FF);
				buff[wbyte++] = (value & 0xFF00) >> 8;
				crc8 = crc_gen(buff, wbyte);
				buff[wbyte++] = crc8;        /* 체크섬 Sum of STX ~ M9 */
			}
			break;

		case DDC_HANGUL2 :
			buff[wbyte++] = DDC_STX;    /* 시작신호 */
			buff[wbyte++] = (pConnInfo->id & 0xFF00) >> 8;   /* Destination Address Hi */
			buff[wbyte++] =  pConnInfo->id & 0x00FF;         /* Destination Address Low */
			buff[wbyte++] = 0xFE;       /* Sourcess Address Hi */
			buff[wbyte++] = 0xDE;       /* Sourcess Address Low */
			if(command_code == DDC_REQ_DATA_TYPE)
			{
				buff[wbyte++] = DDC_REQ_MT;
				buff[wbyte++] = DDC_FR;             /* Frame Byte */
				buff[wbyte++] = DDC_DATA_TYPE_WORD; /* Data Type */
				buff[wbyte++] = 0x21;       /* Object Number */
				buff[wbyte++] = 0x00;       /* Starting Index */
				buff[wbyte++] = 0x00;       /* Starting attribute */
				buff[wbyte++] = 0x01;       /* Index count */
				buff[wbyte++] = 0x20;       /* Attribute count, 한글일체형 항온항습기는 32개의 어트리뷰트 */
				buff[wbyte++] = 0x40;       /* Data Block(Number of data bytes to be returned */
				crc8 = crc_gen(buff, wbyte);
				buff[wbyte++] = crc8;        /* 체크섬 Sum of STX ~ M7 */
			}
			else /* 세팅 명령 */
			{
				buff[wbyte++] = DDC_SET_MT;
				buff[wbyte++] = DDC_FR;             /* Frame Byte */
				buff[wbyte++] = DDC_DATA_TYPE_WORD; /* Data Type */
				buff[wbyte++] = 0x10;               /* Object Number */
				buff[wbyte++] = command_code;       /* Starting Index */
				buff[wbyte++] = 0x00;               /* Starting attribute SET은 모두 0x00 임.*/
				buff[wbyte++] = 0x01;               /* Index count */
				buff[wbyte++] = 0x01;               /* Attribute count */
				buff[wbyte++] = 0x02;               /* BYTE 수*/
				buff[wbyte++] = 0x77;               /* FB (Frame Byte)*/
				buff[wbyte++] = (value & 0x00FF);
				buff[wbyte++] = (value & 0xFF00) >> 8;
				crc8 = crc_gen(buff, wbyte);
				buff[wbyte++] = crc8;        /* 체크섬 Sum of STX ~ M9 */
			}
			break;

		case SEUNGIL :
		case SEUNGIL_MLTM :
			buff[wbyte++] = DDC_STX;    /* 시작신호 */
			buff[wbyte++] = (pConnInfo->id & 0xFF00) >> 8;   /* Destination Address Hi */
			buff[wbyte++] =  pConnInfo->id & 0x00FF;         /* Destination Address Low */
			buff[wbyte++] = 0xFE;       /* Sourcess Address Hi */
			buff[wbyte++] = 0xDE;       /* Sourcess Address Low */
			if(command_code == DDC_REQ_DATA_TYPE)
			{
				buff[wbyte++] = DDC_REQ_MT;
				buff[wbyte++] = DDC_FR;             /* Frame Byte */
				buff[wbyte++] = DDC_DATA_TYPE_WORD; /* Data Type */
				buff[wbyte++] = 0x21;       /* Object Number */
				buff[wbyte++] = value;      /* Starting Index */
				buff[wbyte++] = 0x00;       /* Starting attribute */
				buff[wbyte++] = 0x01;       /* Index count */
				buff[wbyte++] = 0x20;       /* Attribute count, 한글일체형 항온항습기는 32개의 어트리뷰트 */
				buff[wbyte++] = 0x40;       /* Data Block(Number of data bytes to be returned */
				crc8 = crc_gen(buff, wbyte);
				buff[wbyte++] = crc8;        /* 체크섬 Sum of STX ~ M7 */
			}
			else /* 세팅 명령 */
			{
				buff[wbyte++] = DDC_SET_MT;
				buff[wbyte++] = DDC_FR;             /* Frame Byte */
				buff[wbyte++] = DDC_DATA_TYPE_WORD; /* Data Type */
				buff[wbyte++] = 0x10;               /* Object Number */
				buff[wbyte++] = command_code;       /* Starting Index */
				buff[wbyte++] = 0x00;               /* Starting attribute SET은 모두 0x00 임.*/
				buff[wbyte++] = 0x01;               /* Index count */
				buff[wbyte++] = 0x01;               /* Attribute count */
				buff[wbyte++] = 0x02;               /* BYTE 수*/
				buff[wbyte++] = 0x77;               /* FB (Frame Byte)*/
				buff[wbyte++] = (value & 0x00FF);
				buff[wbyte++] = (value & 0xFF00) >> 8;
				crc8 = crc_gen(buff, wbyte);
				buff[wbyte++] = crc8;        /* 체크섬 Sum of STX ~ M9 */
			}
			break;
		case SEUNGIL_STHC_MB_FW36 :
			buff[wbyte++] = DDC_STX;    /* 시작신호 */
			buff[wbyte++] = (pConnInfo->id & 0xFF00) >> 8;   /* Destination Address Hi */
			buff[wbyte++] =  pConnInfo->id & 0x00FF;         /* Destination Address Low */
			buff[wbyte++] = 0xFE;       /* Sourcess Address Hi */
			buff[wbyte++] = 0xDE;       /* Sourcess Address Low */
			if(command_code == DDC_REQ_DATA_TYPE)
			{
				buff[wbyte++] = DDC_REQ_MT;
				buff[wbyte++] = DDC_FR;             /* Frame Byte */
				buff[wbyte++] = DDC_DATA_TYPE_WORD; /* Data Type */
				buff[wbyte++] = 0x21;       /* Object Number */
				buff[wbyte++] = value;      /* Starting Index */
				buff[wbyte++] = 0x00;       /* Starting attribute */
				buff[wbyte++] = 0x01;       /* Index count */
				buff[wbyte++] = 0x7F;       /* Attribute count, 127개의 어트리뷰트 */
				buff[wbyte++] = 0xFE;       /* Data Block(Number of data bytes to be returned */
				crc8 = crc_gen(buff, wbyte);
				buff[wbyte++] = crc8;        /* 체크섬 Sum of STX ~ M7 */
			}
			else /* 세팅 명령 */
			{
				buff[wbyte++] = DDC_SET_MT;
				buff[wbyte++] = DDC_FR;             /* Frame Byte */
				buff[wbyte++] = DDC_DATA_TYPE_WORD; /* Data Type */
				buff[wbyte++] = 0x10;               /* Object Number */
				buff[wbyte++] = 0x40;               /* Starting Index. 인덱스는 64 고정      */
				buff[wbyte++] = command_code;       /* Starting attribute SET은 모두 0x00 임 */
				buff[wbyte++] = 0x01;               /* Index count */
				buff[wbyte++] = 0x01;               /* Attribute count */
				buff[wbyte++] = 0x02;               /* BYTE 수*/
				buff[wbyte++] = 0x77;               /* FB (Frame Byte)*/
				buff[wbyte++] = (value & 0x00FF);
				buff[wbyte++] = (value & 0xFF00) >> 8;
				crc8 = crc_gen(buff, wbyte);
				buff[wbyte++] = crc8;        /* 체크섬 Sum of STX ~ M9 */
			}
			break;
		case FAC1000 :
			buff[wbyte++] = FAC1000_STX;
			buff[wbyte++] = FAC1000_BLK_HEAD;
			buff[wbyte++] = ((pConnInfo->id % 100) / 10) + 0x30;
			buff[wbyte++] =  (pConnInfo->id % 10) + 0x30;
			if(command_code == FAC1000_REQ_CMD ||
			   command_code == FAC1000_ON_CMD  ||
			   command_code == FAC1000_OFF_CMD) {
				buff[wbyte++] = (unsigned char)command_code;
				buff[wbyte++] = 0;
				buff[wbyte++] = 0;
				buff[wbyte++] = 1;
			} else if(command_code == FAC1000_SET_TEMP) {   /* 설정온도 */
				buff[wbyte++] = FAC1000_SET_CMD;
				buff[wbyte++] = 0;  /* M1 */
				buff[wbyte++] = 3;  /* M2 */
				buff[wbyte++] = (value >= 0) ? 0 : 1;   /* 0:양수, 1:음수 */
				temp_value = value % 100;
				buff[wbyte++] = ((temp_value / 10) * 0x10) + (temp_value % 10);
				temp_value = (value / 100) % 100;
				buff[wbyte++] = ((temp_value / 10) * 0x10) + (temp_value % 10);
			} else if(command_code == FAC1000_SET_HUMID        || /* 설정습도 */
					  command_code == FAC1000_SET_COMP1_DELAY  || /* 냉방기1 운전 지연시간 */
					  command_code == FAC1000_SET_COMP2_DELAY  || /* 냉방기2 운전 지연시간 */
					  command_code == FAC1000_SET_BLOWER_DELAY || /* BLOWER 정지 지연시간 */
					  command_code == FAC1000_SET_HUMID_DEC    || /* 가습 운전 편차 */
					  command_code == FAC1000_SET_DEHUM_DEC)      /* 제습 운전 편차 */
			{
				buff[wbyte++] = FAC1000_SET_CMD;
				buff[wbyte++] = (command_code == FAC1000_SET_HUMID)        ? 1 :
								(command_code == FAC1000_SET_COMP1_DELAY)  ? 2 :
								(command_code == FAC1000_SET_COMP2_DELAY)  ? 3 :
								(command_code == FAC1000_SET_BLOWER_DELAY) ? 4 :
								(command_code == FAC1000_SET_HUMID_DEC)    ? 7 : 8;
				buff[wbyte++] = 2;  /* M2 */
				temp_value = (temp_value / 10) % 100;
				buff[wbyte++] = ((temp_value / 10) * 0x10) + (temp_value % 10);
				temp_value = (value / 1000) % 100;
				buff[wbyte++] = ((temp_value / 10) * 0x10) + (temp_value % 10);
			}
			else if(command_code == FAC1000_SET_COOL_DEC || /* 냉방 운전 편차 */
					command_code == FAC1000_SET_HEAT_DEC)       /* 난방 운전 편차 */
			{
				buff[wbyte++] = FAC1000_SET_CMD;
				buff[wbyte++] = (command_code == FAC1000_SET_COOL_DEC) ? 5 : 6;
				buff[wbyte++] = 2;  /* M2 */
				temp_value = temp_value % 100;
				buff[wbyte++] = ((temp_value / 10) * 0x10) + (temp_value % 10);
				temp_value = (value / 100) % 100;
				buff[wbyte++] = ((temp_value / 10) * 0x10) + (temp_value % 10);
			}
			buff[wbyte++] = FAC1000_ETX;
			crc8 = crc_gen(buff, wbyte);
			buff[wbyte++] = crc8;
			break;

		case SYSKOREA :
			buff[wbyte++] = SYSKOREA_STX;   /* 시작신호 */
			buff[wbyte++] = 0xF0;   /*  */
			buff[wbyte++] = pConnInfo->id+0x30;   /*  */
			buff[wbyte++] = command_code;   /*  */
			if(command_code == 0xA1 || command_code == 0xA2 || command_code == 0xA3 ||
			   command_code == 0xA4 || command_code == 0xA7 || command_code == 0xA8) {
				buff[wbyte++] = (value & 0xFF00) >> 8;  /* 데이터 HIGH BYTE */
				buff[wbyte++] =  value & 0x00FF;        /* 데이터 LOW BYTE */
			}
			else {
				buff[wbyte++] = 0x00;   /* 데이터 HIGH BYTE */
				buff[wbyte++] = 0x00;   /* 데이터 LOW BYTE */
			}
			crc8 = crc_gen(buff, wbyte);
			buff[wbyte++] = crc8;   /* CRC */
			buff[wbyte++] = SYSKOREA_ETX;   /* ETX */
			break;

		case BY_CRC2004 :
		{
			struct equip_ai *pAI = gp_status[index]->_ai;
			struct equip_di *pDI = gp_status[index]->_di;

			if(command_code == BY_CRC2004_REQ_CMD) {
				buff[wbyte++] = BY_CRC2004_REQ_CMD;
				buff[wbyte++] = pConnInfo->id;
				buff[wbyte++] = 0;
				buff[wbyte++] = 0;
				buff[wbyte++] = 0;
				buff[wbyte++] = 0;
				buff[wbyte++] = 0;
				buff[wbyte++] = 0;
				buff[wbyte++] = 0;
				buff[wbyte++] = 0;
				buff[wbyte++] = 0;
				buff[wbyte++] = 0;
				buff[wbyte++] = 0;
			} else {
				buff[wbyte++] = BY_CRC2004_SET_CMD;
				buff[wbyte++] = pConnInfo->id;
				//운전모드 설정
				buff[wbyte++] = (pDI+11)->curr_val==DI_ON ? 0 : // 항온항습
								(pDI+12)->curr_val==DI_ON ? 1 : // 항온
								(pDI+13)->curr_val==DI_ON ? 2 : // 항습
								(pDI+14)->curr_val==DI_ON ? 3 : // 냉방
								(pDI+15)->curr_val==DI_ON ? 4 : // 난방
								(pDI+16)->curr_val==DI_ON ? 5 : 0; // 송풍 : Default 항온항습
				buff[wbyte++] = (((int)(pAI+2)->curr_val * 10) & 0xFF00) >> 8; //설정온도1
				buff[wbyte++] =  ((int)(pAI+2)->curr_val * 10) & 0x00FF;       //설정온도2
				buff[wbyte++] = (int)(pAI+3)->curr_val * 10; //온도DB
				buff[wbyte++] = (int)(pAI+4)->curr_val * 10; //온도PB
				buff[wbyte++] = (int)(pAI+5)->curr_val; //설정습도
				buff[wbyte++] = (int)(pAI+6)->curr_val; //습도DB
				buff[wbyte++] = (int)(pAI+7)->curr_val; //습도PB
				buff[wbyte++] = (pDI+34)->curr_val == DI_ON ? 1 : 0; //운전요구
				buff[wbyte++] = (((int)(pAI+8)->curr_val * 10) & 0x0F00) >> 8; //경보(always Zero), 전극봉가습기정격설정전류(H)
				buff[wbyte++] =  ((int)(pAI+8)->curr_val * 10) & 0x00FF; //전극봉가습기정격설정전류(L)
			}
			crc8 = crc_gen(buff+1, wbyte);
			buff[wbyte++] = crc8;
			buff[wbyte++] = 0xFF;
		}
			break;

		case DY_SS2000 :
		case AR_HANE09_DYRCU :
			wbyte = make_dyrcu_com_frame_head(buff, 0x01, 0x0A, pConnInfo->ext_addr, 3);
			buff[wbyte++] = (unsigned char)pConnInfo->id + 'a' - 1;  /* 장비번호 */
			buff[wbyte++] = (unsigned char)command_code;
			buff[wbyte++] = (unsigned char)value;
			crc16 = modbus_crc16(buff, wbyte);
			buff[wbyte++] = (crc16 & 0xFF00) >> 8; /* crc hi */
			buff[wbyte++] =  crc16 & 0x00FF;       /* crc low */
			break;

		case ITTS2 :			
		{
			buff[wbyte++] = (unsigned char)pConnInfo->id; /* 장비 번호 */
			buff[wbyte++] = 4;
			buff[wbyte++] = 0;  /* start address hi */
			buff[wbyte++] = 0;  /* start address low */
			buff[wbyte++] = (unsigned char) value;   /* word count */
			crc16 = itts2_crc16(buff, wbyte);
			buff[wbyte++] = (unsigned char) ((crc16 & 0xFF00) >> 8);  /* crc hi */
			buff[wbyte++] = (unsigned char)  (crc16 & 0x00FF);        /* crc low */
		}
			break;
		case INNOTUBE :			
			// 통신두절현상 패치 2021.01.12
			// 발생원인
			// '0xFA' 가 발생한 이후 정상적인 패킷 바이트(5바이트)를 채우지 못하면
			// 이후 온습도 센서 데이터 요청에 대한 응답이 없음
			// 
			// Dumy data [00 00 00 00] 4byte를 정상 데이터와 함께 실어 보낸다
			buff[wbyte++] = 0x00;
			buff[wbyte++] = 0x00;
			buff[wbyte++] = 0x00;
			buff[wbyte++] = 0x00;

			buff[wbyte++] = 0xFA;
			buff[wbyte++] = (unsigned char)command_code;
			buff[wbyte++] = 0x0A;
			buff[wbyte++] = buff[5] ^ buff[6];
			buff[wbyte++] = 0x03;
			break;

		default :
			fileLog(CRITICAL, "[%s:%d] Undefined Request Case eseq=[%d], model=[%s]\n", __FUNCTION__, __LINE__, pConnInfo->eseq, pConnInfo->model_seq);
			break;
	}
	return wbyte;
}

