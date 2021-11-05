#include "include.h"

extern struct equip_conn_info *conn_info[];
extern struct equip_status *gp_status[];

unsigned short get_ushort_value(unsigned char first, unsigned char second)
{
    unsigned short value = 0;

    ((unsigned char *)&value)[0] = second;
    ((unsigned char *)&value)[1] = first;

    return value;
}

short get_short_value(unsigned char first, unsigned char second)
{
    short value = 0;

    ((unsigned char *)&value)[0] = second;
    ((unsigned char *)&value)[1] = first;

    return value;
}

short get_short_value2(unsigned char first, unsigned char second)
{
    short value = 0;

    ((char *)&value)[0] = second;
    ((char *)&value)[1] = first;

    return value;
}

int get_int_value2(unsigned char first, unsigned char second)
{
    int value = 0;

    ((unsigned char *)&value)[0] = second;
    ((unsigned char *)&value)[1] = first;

    return value;
}

unsigned int get_uint_value4(unsigned char first, unsigned char second, unsigned char third, unsigned char fourth)
{
    unsigned int value = 0;

    ((unsigned char *)&value)[0] = fourth;
    ((unsigned char *)&value)[1] = third;
    ((unsigned char *)&value)[2] = second;
    ((unsigned char *)&value)[3] = first;

    return value;
}
int get_int_value4(unsigned char first, unsigned char second, unsigned char third, unsigned char fourth)
{
    int value = 0;

    ((unsigned char *)&value)[0] = fourth;
    ((unsigned char *)&value)[1] = third;
    ((unsigned char *)&value)[2] = second;
    ((unsigned char *)&value)[3] = first;

    return value;
}

float get_float_value2(unsigned char first, unsigned char second)
{
    char buf[4] = {0x00, };
    sprintf(buf, "%c.%c", first, second);
    return strtof((const char *)buf, NULL);
}

float get_float_value3(unsigned char first, unsigned char second, unsigned char third)
{
    char buf[8] = {0x00, };
    sprintf(buf, "%c%c.%c", first, second, third);
    return strtof((const char *)buf, NULL);
}

float get_float_value4(unsigned char first, unsigned char second, unsigned char third, unsigned char fourth)
{
    float value=0.0;

    ((unsigned char *)&value)[0] = first;
    ((unsigned char *)&value)[1] = second;
    ((unsigned char *)&value)[2] = third;
    ((unsigned char *)&value)[3] = fourth;

    return value;
}

float get_superswitch3_fvalue(unsigned char first, unsigned char second, unsigned char third, unsigned char fourth)
{
    float value = 0.0f;

    ((unsigned char *)&value)[0] = second;
    ((unsigned char *)&value)[1] = first;
    ((unsigned char *)&value)[2] = fourth;
    ((unsigned char *)&value)[3] = third;

    return value;
}

unsigned long get_ulong_value4(unsigned char first, unsigned char second, unsigned char third, unsigned char fourth)
{
    unsigned long value = 0;

    ((unsigned char *)&value)[0] = fourth;
    ((unsigned char *)&value)[1] = third;
    ((unsigned char *)&value)[2] = second;
    ((unsigned char *)&value)[3] = first;

    return value;
}

double get_double_value8(unsigned char first, unsigned char second, unsigned char third, unsigned char fourth, unsigned char fifth, unsigned char sixth, unsigned char seventh, unsigned char eighth)
{
    double df=0.0;

    ((unsigned char *)&df)[0] = first;
    ((unsigned char *)&df)[1] = second;
    ((unsigned char *)&df)[2] = third;
    ((unsigned char *)&df)[3] = fourth;
    ((unsigned char *)&df)[4] = fifth;
    ((unsigned char *)&df)[5] = sixth;
    ((unsigned char *)&df)[6] = seventh;
    ((unsigned char *)&df)[7] = eighth;

    return df;
}

void set_module_status(unsigned char *buff, int index, int com_state)
{
    int  i, k;
    unsigned char *p=buff;
	int dot=0;
    float temp_f;
    struct equip_ai *pAI = gp_status[index]->_ai;
    struct equip_di *pDI = gp_status[index]->_di;
    //struct equip_do *pDO = gp_status[index]->_do;
    struct equip_status *pStatus = gp_status[index];
    struct equip_conn_info *pConnInfo = conn_info[index];

	double ai_diff, di_diff, update_diff;
	time_t now;
	now = time(NULL);

    for(i = 0; i < pConnInfo->ai_count; i++, pAI++)
    {
        pthread_mutex_lock(pStatus->mux);

        switch(pConnInfo->model_seq)
        {
			//UPS 시작			
			case UPS_HIPULSE_U :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[ 0]<<8)|p[ 1])/10.; // 출력전압(R)
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[ 2]<<8)|p[ 3])/10.; // 출력전압(S)
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[ 4]<<8)|p[ 5])/10.; // 출력전압(T)
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[ 6]<<8)|p[ 7])/10.; // 출력전류(R)
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[ 8]<<8)|p[ 9])/10.; // 출력전류(S)
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[10]<<8)|p[11])/10.; // 출력전류(T)
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[12]<<8)|p[13])/10.; // 배터리전압
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[14]<<8)|p[15])/100.; // 출력주파수
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[16]<<8)|p[17])/10.; // 입력전압(RS)
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[18]<<8)|p[19])/10.; // 입력전압(ST)
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[20]<<8)|p[21])/10.; // 입력전압(TR)
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[22]<<8)|p[23])/10.; // 입력전류(R)
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[24]<<8)|p[25])/10.; // 입력전류(S)
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[26]<<8)|p[27])/10.; // 입력전류(T)
				else if(i==14) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[28]<<8)|p[29])/100.; // 입력주파수
				else if(i==15) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[30]<<8)|p[31]); // 토탈입력역률
				else if(i==16) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[32]<<8)|p[33])/10.; // 바이패스전압(R)
				else if(i==17) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[34]<<8)|p[35])/10.; // 바이패스전압(S)
				else if(i==18) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[36]<<8)|p[37])/10.; // 바이패스전압(T)
				else if(i==19) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[38]<<8)|p[39])/100.; // 바이패스주파수
				else if(i==20) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[40]<<8)|p[41])/10.; // 배터리전류
				else if(i==21) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 :  (p[42]<<8)|p[43]; // 출력역률(R)
				else if(i==22) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 :  (p[44]<<8)|p[45]; // 출력역률(S)
				else if(i==23) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 :  (p[46]<<8)|p[47]; // 출력역률(T)

				else if(i==24) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[54]<<8)|p[55])/100.; // 출력유효전력(Module Phase R)
				else if(i==25) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[56]<<8)|p[57])/100.; // 출력유효전력(Module Phase S)
				else if(i==26) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[58]<<8)|p[59])/100.; // 출력유효전력(Module Phase T)
				else if(i==27) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[60]<<8)|p[61])/100.; // 출력피상전력(Module Phase R)
				else if(i==28) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[62]<<8)|p[63])/100.; // 출력피상전력(Module Phase S)
				else if(i==29) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[64]<<8)|p[65])/100.; // 출력피상전력(Module Phase T)
				else if(i==30) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[66]<<8)|p[67])/10.; // 출력로드율(Module Phase R)
				else if(i==31) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[68]<<8)|p[69])/10.; // 출력로드율(Module Phase S)
				else if(i==32) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[70]<<8)|p[71])/10.; // 출력로드율(Module Phase T)
				else if(i==33) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[72]<<8)|p[73])/100.; // 출력무효전력(Module Phase R)
				else if(i==34) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[74]<<8)|p[75])/100.; // 출력무효전력(Module Phase S)
				else if(i==35) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[76]<<8)|p[77])/100.; // 출력무효전력(Module Phase T)
				else if(i==36) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[78]<<8)|p[79])/100.; // 출력유효전력(System Phase R)
				else if(i==37) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[80]<<8)|p[81])/100.; // 출력유효전력(System Phase S)
				else if(i==38) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[82]<<8)|p[83])/100.; // 출력유효전력(System Phase T)
				else if(i==39) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[84]<<8)|p[85])/100.; // 출력피상전력(System Phase R)
				else if(i==40) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[86]<<8)|p[87])/100.; // 출력피상전력(System Phase S)
				else if(i==41) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[88]<<8)|p[89])/100.; // 출력피상전력(System Phase T)
				else if(i==42) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[90]<<8)|p[91])/100.; // 출력무효전력(System Phase R)
				else if(i==43) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[92]<<8)|p[93])/100.; // 출력무효전력(System Phase S)
				else if(i==44) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[94]<<8)|p[95])/100.; // 출력무효전력(System Phase T)
				else if(i==45) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 :  (p[96]<<8)|p[97]; // 축전지남은시간
				else if(i==46) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[98]<<8)|p[99])/10.; // 축전지온도
				else if(i==47) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : ((p[102]<<8)|p[103])/100.; // 축전지양
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
			//항온항습기 시작
			case AR_HANE09 :
			case AR_SSHP8Y1 : /* version 1.4.1-Debug */
			case AR_HANE09_DYRCU :
			case AR_HANE09_EXOR :
			case AR_HANE09_Q :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value3(p[ 0], p[ 1], p[ 2]);	// ROOM온도
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value3(p[ 3], p[ 4], p[ 5]);	// ROOM습도
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value3(p[ 6], p[ 7], p[ 8]);	// 설정온도
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value2(p[ 9], p[10]);			// 냉방감응온도
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value2(p[11], p[12]);			// 난방감응온도
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value3(p[13], p[14], p[15]);	// 설정습도
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value3(p[16], p[17], p[18]);	// 가습감응습도
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value3(p[19], p[20], p[21]);	// 제습감응습도
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value3(p[22], p[23], p[24]);	// 고온경보설정
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value3(p[25], p[26], p[27]);	// 고습경보설정
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value2(p[28], p[29]);			// 배수주기설정
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value2(p[30], p[31]);			// 배수시간설정
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case RTU_AR_HANE09 :
			case RTU_AR_HANE09_Q :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 0], p[ 1]) / 10.;	// ROOM온도
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 2], p[ 3]) / 10.;	// ROOM습도
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 8], p[ 5]) / 10.;	// 설정온도
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 6], p[ 7]);	// 냉방감응온도
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 8], p[ 9]);	// 난방감응온도
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[10], p[11]) / 10.;	// 설정습도
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[12], p[13]);	// 가습감응습도
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[14], p[15]);	// 제습감응습도
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[16], p[17]) / 10.;	// 고온경보설정
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[18], p[19]) / 10.;	// 고습경보설정
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[20], p[21]);	// 배수주기설정
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[22], p[23]);	// 배수시간설정
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
			
			case SOLID :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 3], p[ 4]) / 10.;		// Sensor 1(실내온도)
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 5], p[ 6]) / 10.;		// Sensor 2(실외온도)
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 7], p[ 8]) / 10.;		// Sensor 3(냉수온도)
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 9], p[10]) / 10.;		// Sensor 4(동파온도)
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[11], p[12]) / 10.;		// Sensor 5(응축온도)
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[13], p[14]);			// Sensor 6(실내습도)
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[73], p[74]);			// 압축기1 누적동작시간[Hour]
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[75], p[76]);			// 압축기2 누적동작시간[Hour]
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case STHAV_MB_R1 :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 0], p[ 1]) / 10.;	// 현재 온도
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 4], p[ 5]);		// 현재 습도
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 8], p[ 9]) / 10.;	// 현재 가습 전류
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[14], p[15]) / 10.;	// 정격전류
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[70], p[71]);		// 운전시간
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[72], p[73]);		// 압축기1 동작시간
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[74], p[75]);		// 압축기2 동작시간
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[76], p[77]);		// 압축기3 동작시간
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[78], p[79]);		// 가습기 동작시간
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
				
			case AR_MODBUS :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[119],p[120]) / 10.;	// 현재온도
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[121],p[122]) / 10.;	// 현재습도
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[3],  p[4]) / 10.;		// 설정온도
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[5],  p[6]) / 10.;		// 설정습도
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[7],  p[8]);			// 정전복귀
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[9],  p[10]);			// 정지지연
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[11], p[12]);			// 콤프알람
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[15], p[16]) / 10.;		// 온도편차
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[17], p[18]) / 10.;		// 습도편차 
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[19], p[20]) / 10.;		// 전류편차 
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[27], p[28]);			// 콤프지연
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[29], p[30]);			// 히팅지연
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[31], p[32]);			// 펌프다운
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[43], p[44]) / 10.;		// 냉방편차
				else if(i==14) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[45], p[46]) / 10.;		// 난방편차
				else if(i==15) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[47], p[48]) / 10.;		// 가습편차
				else if(i==16) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[49], p[50]) / 10.;		// 제습편차
				else if(i==17) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[53], p[54]);			// 콤프1적산시간
				else if(i==18) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[55], p[56]);			// 콤프2적산시간
				else if(i==19) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[57], p[58]);			// 히터1적산시간
				else if(i==20) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[59], p[60]);			// 히터2적산시간
				else if(i==21) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[61], p[62]);			// 히터3적산시간
				else if(i==22) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[63], p[64]);			// 히터4적산시간
				else if(i==23) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[65], p[66]);			// 히터5적산시간
				else if(i==24) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[67], p[68]);			// 가습1적산시간
				else if(i==25) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[69], p[70]);			// 가습2적산시간
				else if(i==26) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[71], p[72]) / 10.;		// 고온경보
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
			case RTU_AR_MODBUS :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 0], p[ 1]) / 10;		// 현재 온도
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 2], p[ 3]) / 10;		// 현재 습도
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 4], p[ 5]) / 10;		// 설정 온도
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 6], p[ 7]) / 10;		// 설정 습도
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 8], p[ 9]) / 10;		// 정전 복귀 시간(초)
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[10], p[11]) / 10;		// 정지 지연 시간(초)
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[12], p[13]) / 10;		// 콤프알람
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[14], p[15]) / 10;		// 온도 편차
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[16], p[17]) / 10;		// 습도 편차
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[18], p[19]) / 10;		// 전류 편차
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[20], p[21]) / 10;		// 콤프 지연
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[22], p[23]) / 10;		// 히팅 지연
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[24], p[25]) / 10;		// 펌프 다운
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[26], p[27]) / 10;		// 냉방 편차
				else if(i==14) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[28], p[29]) / 10;		// 난방 편차
				else if(i==15) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[30], p[31]) / 10;		// 가습 편차
				else if(i==16) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[32], p[33]) / 10;		// 제습 편차
				else if(i==17) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[34], p[35]) / 10;		// 콤프 1 적산시간
				else if(i==18) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[36], p[37]) / 10;		// 콤프 2 적산시간
				else if(i==19) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[38], p[39]) / 10;		// 히터 1 적산시간
				else if(i==20) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[40], p[41]) / 10;		// 히터 2 적산시간
				else if(i==21) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[42], p[43]) / 10;		// 히터 3 적산시간
				else if(i==22) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[44], p[45]) / 10;		// 히터 4 적산시간
				else if(i==23) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[46], p[47]) / 10;		// 히터 5 적산시간
				else if(i==24) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[48], p[49]) / 10;		// 가습 1 적산시간
				else if(i==25) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[50], p[51]) / 10;		// 가습 2 적산시간
				else if(i==26) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[52], p[53]) / 10;		// 고온 경보 설정 온도
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
				
			case RTU_LIEBERT_CHTP :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 0], p[ 1], p[ 2], p[ 3]);		// Temperature
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 4], p[ 5], p[ 6], p[ 7]);		// humidity
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 8], p[ 9], p[10], p[11]);		// Stages
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[12], p[13], p[14], p[15]);		// Capacity
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[16], p[17], p[18], p[19]);		// Temperature_setpoint
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[20], p[21], p[22], p[23]);		// Temperature_Tolerance
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[24], p[25], p[26], p[27]);		// Humidity_Setpoint
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[28], p[29], p[30], p[31]);		// Humidity_Tolerance
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[32], p[33], p[34], p[35]);		// High_Temp_Alarm_Setpoint
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[36], p[37], p[38], p[39]);		// Low_Temp_Alarm_Setpoint
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[40], p[41], p[42], p[43]);		// High_Humd_Alarm_Setpoint
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[44], p[45], p[46], p[47]);		// Low_Humd_Alarm_Setpoint
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[48], p[49], p[50], p[51]);		// Fan_Motor_Run_Hour
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[52], p[53], p[54], p[55]);		// Humidifier_Run_Hour
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
				
			case OC485 :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 0], p[ 1])/10.;	// 설정온도
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 2], p[ 3])/10.;	// 온도편차구간
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 4], p[ 5])/10.;	// 설정편차온도
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 8], p[ 9]);		// 설정습도
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[10], p[11]);		// 습도편차구간
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[14], p[15]);		// 설정편차습도
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[16], p[17])/10.;	//	고온설정
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[18], p[19])/10.;	// 저온설정
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[24], p[25]);		// 고습설정
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[26], p[27]);		// 저습설정
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[48], p[49])/10.;	// 리턴온도
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[68], p[69]);		// 리턴습도
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[78], p[79]);		// 팬기동시간
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[84], p[85]);		// 가습기동시간
				else if(i==14) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[86], p[87]);		// 제습기동시간
				else if(i==15) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[90], p[91]);		// Electrical_Heaters#1_Run_hours
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case RTU_OC485 :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[  0], p[  1], p[  2], p[  3]);		// 설정온도
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[  4], p[  5], p[  6], p[  7]);		// 온도편차구간
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[  8], p[  9], p[ 10], p[ 11]);		// 설정편차온도
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 12], p[ 13], p[ 14], p[ 15]);		// 설정습도
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 16], p[ 17], p[ 18], p[ 19]);		// 습도편차구간
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 20], p[ 21], p[ 22], p[ 23]);		// 설정편차습도
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 24], p[ 25], p[ 26], p[ 27]);		// 고온설정
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 28], p[ 29], p[ 30], p[ 31]);		// 저온설정
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 40], p[ 41], p[ 42], p[ 43]);		// 고습설정
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 44], p[ 45], p[ 46], p[ 47]);		// 저습설정
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 80], p[ 81], p[ 82], p[ 83]);		// 리턴 온도
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[120], p[121], p[122], p[123]);		// 리턴 습도
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[140], p[141], p[142], p[143]);		// 팬 기동 시간
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[152], p[153], p[154], p[155]);		// 가습기 기동 시간
				else if(i==14) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[156], p[157], p[158], p[159]);		// 제습기 기동 시간
				else if(i==15) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[164], p[165], p[166], p[167]);		// 히터 기동 시간
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			//분전반 시작
			case DPM_MPM330 :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[3],  p[4]);				//선간전압(RS)
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[5],  p[6]);				//선간전압(ST)
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[7],  p[8]);				//선간전압(TR)
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[9],  p[10]);				//상전압(R)
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[11], p[12]);				//상전압(S)
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[13], p[14]);				//상전압(T)
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[15], p[16]);				//상전류(R)
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[17], p[18]);				//상전류(S)
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[19], p[20]);				//상전류(T)
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[21], p[22]) / 100.;		//주파수
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[23], p[24]) / 100.;		//평균역율
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[25], p[26]);				//Total유효전력
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[27], p[28]);				//Total무효전력
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[29], p[30]);				//Total피상전력
				else if(i==14) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[41], p[42]);				//CT비
				else if(i==15) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[43], p[44]);				//PT비
				else if(i==16) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[63], p[64]) / 100.;		//R상역율
				else if(i==17) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[65], p[66]) / 100.;		//S상역율
				else if(i==18) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[67], p[68]) / 100.;		//T상역율
				else if(i==19) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[69], p[70]);				//R상유효전력
				else if(i==20) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[71], p[72]);				//S상유효전력
				else if(i==21) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[73], p[74]);				//T상유효전력
				else if(i==22) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[75], p[76]);				//R상무효전력
				else if(i==23) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[77], p[78]);				//S상무효전력
				else if(i==24) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[79], p[80]);				//T상무효전력
				else if(i==25) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_uint_value4(p[31], p[32], p[33], p[34]);	//누적전력량
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case DPM_MPM330A :
			case DPM_MPM330A_TR :
			case DPM_MPM330A_DYRCU :
			case DPM_MPM330A_PMS :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_uint_value4(p[  0], p[  1], p[  2], p[  3]) / 100.; //선간전압(RS)
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_uint_value4(p[  4], p[  5], p[  6], p[  7]) / 100.; //선간전압(ST)
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_uint_value4(p[  8], p[  9], p[ 10], p[ 11]) / 100.; //선간전압(TR)
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_uint_value4(p[ 12], p[ 13], p[ 14], p[ 15]) / 100.; //상전압(R)
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_uint_value4(p[ 16], p[ 17], p[ 18], p[ 19]) / 100.; //상전압(S)
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_uint_value4(p[ 20], p[ 21], p[ 22], p[ 23]) / 100.; //상전압(T)
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_uint_value4(p[ 24], p[ 25], p[ 26], p[ 27]) / 1000.; //상전류(R)
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_uint_value4(p[ 28], p[ 29], p[ 30], p[ 31]) / 1000.; //상전류(S)
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_uint_value4(p[ 32], p[ 33], p[ 34], p[ 35]) / 1000.; //상전류(T)
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_uint_value4(p[ 36], p[ 37], p[ 38], p[ 39]) / 1000.; //Total유효전력
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_uint_value4(p[ 40], p[ 41], p[ 42], p[ 43]) / 1000.; //Total무효전력
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_uint_value4(p[ 44], p[ 45], p[ 46], p[ 47]) / 1000.; //Total피상전력
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_uint_value4(p[ 48], p[ 49], p[ 50], p[ 51]) / 10.; //주파수
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_int_value4 (p[ 52], p[ 53], p[ 54], p[ 55]) / 100.; //평균역률
				else if(i==14) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_uint_value4(p[ 56], p[ 57], p[ 58], p[ 59]); // 유효전력량
				else if(i==15) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_uint_value4(p[ 60], p[ 61], p[ 62], p[ 63]); // 무효전력량
				else if(i==16) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_uint_value4(p[ 64], p[ 65], p[ 66], p[ 67]) / 1000.; // 유효전력PEAK
				else if(i==17) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_int_value4 (p[ 68], p[ 69], p[ 70], p[ 71]) / 1000.; // R상전류PEAK
				else if(i==18) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_int_value4 (p[ 72], p[ 73], p[ 74], p[ 75]) / 1000.; // S상전류PEAK
				else if(i==19) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_int_value4 (p[ 76], p[ 77], p[ 78], p[ 79]) / 1000.; // T상전류PEAK
				else if(i==20) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_int_value4 (p[ 80], p[ 81], p[ 82], p[ 83]) / 1000.; // R상유효전력
				else if(i==21) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_int_value4 (p[ 84], p[ 85], p[ 86], p[ 87]) / 1000.; // S상유효전력
				else if(i==22) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_int_value4 (p[ 88], p[ 89], p[ 90], p[ 91]) / 1000.; // T상유효전력
				else if(i==23) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_int_value4 (p[ 92], p[ 93], p[ 94], p[ 95]) / 1000.; // R상무효전력
				else if(i==24) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_int_value4 (p[ 96], p[ 97], p[ 98], p[ 99]) / 1000.; // S상무효전력
				else if(i==25) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_int_value4 (p[100], p[101], p[102], p[103]) / 1000.; // T상무효전력
				else if(i==26) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_int_value4 (p[104], p[105], p[106], p[107]) / 1000.; // R상피상전력
				else if(i==27) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_int_value4 (p[108], p[109], p[110], p[111]) / 1000.; // S상피상전력
				else if(i==28) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_int_value4 (p[112], p[113], p[114], p[115]) / 1000.; // T상피상전력
				else if(i==29) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value(p[116], p[117]) / 100.; // R상역율
				else if(i==30) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value(p[118], p[119]) / 100.; // S상역율
				else if(i==31) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value(p[120], p[121]) / 100.; // T상역율
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case RTU_MPM330A :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[  0], p[  1], p[  2], p[  3]); //선간전압(RS)
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[  4], p[  5], p[  6], p[  7]); //선간전압(ST)
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[  8], p[  9], p[ 10], p[ 11]); //선간전압(TR)
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[ 12], p[ 13], p[ 14], p[ 15]); //상전압(R)
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[ 16], p[ 17], p[ 18], p[ 19]); //상전압(S)
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[ 20], p[ 21], p[ 22], p[ 23]); //상전압(T)
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[ 24], p[ 25], p[ 26], p[ 27]); //상전류(R)
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[ 28], p[ 29], p[ 30], p[ 31]); //상전류(S)
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[ 32], p[ 33], p[ 34], p[ 35]); //상전류(T)
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[ 36], p[ 37], p[ 38], p[ 39]); //Total유효전력
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[ 40], p[ 41], p[ 42], p[ 43]); //Total무효전력
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[ 44], p[ 45], p[ 46], p[ 47]); //Total피상전력
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[ 48], p[ 49], p[ 50], p[ 51]); //주파수
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[ 52], p[ 53], p[ 54], p[ 55]); //평균역률
				else if(i==14) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[ 56], p[ 57], p[ 58], p[ 59]); // 유효전력량
				else if(i==15) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[ 60], p[ 61], p[ 62], p[ 63]); // 무효전력량
				else if(i==16) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[ 64], p[ 65], p[ 66], p[ 67]); // 유효전력PEAK
				else if(i==17) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[ 68], p[ 69], p[ 70], p[ 71]); // R상전류PEAK
				else if(i==18) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[ 72], p[ 73], p[ 74], p[ 75]); // S상전류PEAK
				else if(i==19) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[ 76], p[ 77], p[ 78], p[ 79]); // T상전류PEAK
				else if(i==20) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[ 80], p[ 81], p[ 82], p[ 83]); // R상유효전력
				else if(i==21) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[ 84], p[ 85], p[ 86], p[ 87]); // S상유효전력
				else if(i==22) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[ 88], p[ 89], p[ 90], p[ 91]); // T상유효전력
				else if(i==23) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[ 92], p[ 93], p[ 94], p[ 95]); // R상무효전력
				else if(i==24) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[ 96], p[ 97], p[ 98], p[ 99]); // S상무효전력
				else if(i==25) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[100], p[101], p[102], p[103]); // T상무효전력
				else if(i==26) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[104], p[105], p[106], p[107]); // R상피상전력
				else if(i==27) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[108], p[109], p[110], p[111]); // S상피상전력
				else if(i==28) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_float_value4 (p[112], p[113], p[114], p[115]); // T상피상전력
				else if(i==29) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value(p[116], p[117]); // R상역율
				else if(i==30) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value(p[118], p[119]); // S상역율
				else if(i==31) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value(p[120], p[121]); // T상역율
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case DPM_IMPRO :
				((char *)&temp_f)[0] = p[(i*4)+3];
				((char *)&temp_f)[1] = p[(i*4)+2];
				((char *)&temp_f)[2] = p[(i*4)+1];
				((char *)&temp_f)[3] = p[(i*4)+0];
				pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : temp_f;
				break;

			case DPM_ViMAC_IV_V :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ulong_value4 (p[3 ], p[4 ], p[5 ], p[6 ]) / 1000;
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ulong_value4 (p[7 ], p[8 ], p[9 ], p[10]) / 1000;
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ulong_value4 (p[11], p[12], p[13], p[14]) / 1000;
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ulong_value4 (p[15], p[16], p[17], p[18]) / 10;
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ulong_value4 (p[19], p[20], p[21], p[22]) / 10;
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ulong_value4 (p[23], p[24], p[25], p[26]) / 10;
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ulong_value4 (p[27], p[28], p[29], p[30]) / 10;
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ulong_value4 (p[31], p[32], p[33], p[34]) / 10;
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ulong_value4 (p[35], p[36], p[37], p[38]) / 10;
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ulong_value4 (p[39], p[40], p[41], p[42]);
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ulong_value4 (p[43], p[44], p[45], p[46]);
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ulong_value4 (p[47], p[48], p[49], p[50]);
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ulong_value4 (p[51], p[52], p[53], p[54]);
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ulong_value4 (p[55], p[56], p[57], p[58]);
				else if(i==14) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value (p[59], p[60]) / 100;
				else if(i==15) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[61], p[62]) / 100;
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case DPM_ODT304 :
			case DPM_MAX304_MELSEC :
			case DPM_MAX304_MELSEC_DYRCU :
			{
				char ODT304[16];
				memset(ODT304, 0x00, sizeof(ODT304));
				if       (i== 0) {
					sprintf(ODT304, "0x%c%c%c%c", p[0 ], p[1 ], p[2 ], p[3 ]);
					pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)strtol(ODT304, NULL, 16) ;  // 전압(R)
				} else if(i== 1) {
					sprintf(ODT304, "0x%c%c%c%c", p[4 ], p[5 ], p[6 ], p[7 ]);
					pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)strtol(ODT304, NULL, 16) ;  // 전압(S)
				} else if(i== 2) {
					sprintf(ODT304, "0x%c%c%c%c", p[8 ], p[9], p[10], p[11]);
					pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)strtol(ODT304, NULL, 16) ;  // 전압(T)
				} else if(i== 3) {
					sprintf(ODT304, "0x%c%c%c%c", p[12], p[13], p[14], p[15]);
					pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)strtol(ODT304, NULL, 16) ;  // 전류(R)
				} else if(i== 4) {
					sprintf(ODT304, "0x%c%c%c%c", p[16], p[17], p[18], p[19]);
					pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)strtol(ODT304, NULL, 16) ;  // 전류(S)
				} else if(i== 5) {
					sprintf(ODT304, "0x%c%c%c%c", p[20], p[21], p[22], p[23]);
					pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)strtol(ODT304, NULL, 16) ;  // 전류(T)
				} else if(i== 6) {
					sprintf(ODT304, "0x%c%c%c%c", p[24], p[25], p[26], p[27]);
					pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)strtol(ODT304, NULL, 16) ;  // 전력
				} else if(i== 7) {
					sprintf(ODT304, "0x%c%c%c%c", p[28], p[29], p[30], p[31]);
					pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)strtol(ODT304, NULL, 16) ;  // 역률
				} else if(i== 8) {
					sprintf(ODT304, "0x%c%c%c%c", p[32], p[33], p[34], p[35]);
					pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)strtol(ODT304, NULL, 16) ;  // 무효전력
				} else if(i== 9) {
					sprintf(ODT304, "0x%c%c%c%c", p[36], p[37], p[38], p[39]);
					pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)strtol(ODT304, NULL, 16) ;  // 주파수
				} else if(i== 10) {
					sprintf(ODT304, "0x%c%c%c%c%c%c%c%c", p[40], p[41], p[42], p[43], p[44], p[45], p[46], p[47]);
					pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)strtol(ODT304, NULL, 16) ;  // 유효전력량
				} else if(i== 11) {
					sprintf(ODT304, "0x%c%c%c%c%c%c%c%c", p[48], p[49], p[50], p[51], p[52], p[53], p[54], p[55]);
					pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)strtol(ODT304, NULL, 16) ;  // 무효전력량
				} 
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
			}
				break;

			case DPM_ACCURA3300 :
			case DPM_ACCURA3500 :
			case DPM_ACCURA3550 :
			case DPM_ACCURA3300_DYRCU :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 0], p[ 1]) * get_ushort_value(p[16], p[17]) * VSA_SCALE; // 전압(R)
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 2], p[ 3]) * get_ushort_value(p[16], p[17]) * VSA_SCALE; // 전압(S)
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 4], p[ 5]) * get_ushort_value(p[16], p[17]) * VSA_SCALE; // 전압(T)
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 6], p[ 7]) * get_ushort_value(p[16], p[17]) * VSA_SCALE; // 전압평균
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 8], p[ 9]) * get_ushort_value(p[16], p[17]) * VSA_SCALE; // 선간전압(RS)
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[10], p[11]) * get_ushort_value(p[16], p[17]) * VSA_SCALE; // 선간전압(ST)
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[12], p[13]) * get_ushort_value(p[16], p[17]) * VSA_SCALE; // 선간전압(TR)
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[14], p[15]) * get_ushort_value(p[16], p[17]) * VSA_SCALE; // 선간전압평균
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[18], p[19]) * get_ushort_value(p[34], p[35]) * VSB_SCALE; // 전류(R)
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[20], p[21]) * get_ushort_value(p[34], p[35]) * VSB_SCALE; // 전류(S)
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[22], p[23]) * get_ushort_value(p[34], p[35]) * VSB_SCALE; // 전류(T)
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[24], p[25]) * get_ushort_value(p[34], p[35]) * VSB_SCALE; // 전류평균
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[26], p[27]) * get_ushort_value(p[34], p[35]) * VSB_SCALE; // 전류(R)
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[28], p[29]) * get_ushort_value(p[34], p[35]) * VSB_SCALE; // 전류(S)
				else if(i==14) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[30], p[31]) * get_ushort_value(p[34], p[35]) * VSB_SCALE; // 전류(T)
				else if(i==15) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[32], p[33]) * get_ushort_value(p[34], p[35]) * VSB_SCALE; // 전류평균
				else if(i==16) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[36], p[37]) * get_ushort_value(p[42], p[43]) * VSB_SCALE; // 유효전력(R)
				else if(i==17) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[38], p[39]) * get_ushort_value(p[42], p[43]) * VSB_SCALE; // 유효전력(S)
				else if(i==18) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[40], p[41]) * get_ushort_value(p[42], p[43]) * VSB_SCALE; // 유효전력(T)
				else if(i==19) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[44], p[45]) * get_ushort_value(p[46], p[47]) * VSB_SCALE; // 토탈유효전력
				else if(i==20) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[48], p[49]) * get_ushort_value(p[54], p[55]) * VSB_SCALE; // 무효전력(R)
				else if(i==21) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[50], p[51]) * get_ushort_value(p[54], p[55]) * VSB_SCALE; // 무효전력(S)
				else if(i==22) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[52], p[53]) * get_ushort_value(p[54], p[55]) * VSB_SCALE; // 무효전력(T)
				else if(i==23) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[56], p[56]) * get_ushort_value(p[58], p[59]) * VSB_SCALE; // 토탈무효전력
				else if(i==24) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[60], p[61]) * get_ushort_value(p[66], p[67]) * VSB_SCALE; // 피상전력(R)
				else if(i==25) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[62], p[63]) * get_ushort_value(p[66], p[67]) * VSB_SCALE; // 피상전력(S)
				else if(i==26) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[64], p[65]) * get_ushort_value(p[66], p[67]) * VSB_SCALE; // 피상전력(T)
				else if(i==27) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[68], p[69]) * get_ushort_value(p[70], p[71]) * VSB_SCALE; // 토탈피상전력
				else if(i==28) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[72], p[73]) * 0.1; // 역율(R)
				else if(i==29) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[74], p[75]) * 0.1; // 역율(S)
				else if(i==30) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[76], p[77]) * 0.1; // 역율(T)
				else if(i==31) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[78], p[79]) * 0.1; // 토탈역율
				else if(i==32) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[80], p[81]) * 0.01; // 주파수
				else if(i==33) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[82], p[83], p[84], p[85]); // 유효전력량
				else if(i==34) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[86], p[87], p[88], p[89]); // 무효전력량
				else if(i==35) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[90], p[91], p[92], p[93]); // 피상전력량
				else if(i==36) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[94], p[95]) * 0.01; // 전압왜율(R)
				else if(i==37) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[96], p[97]) * 0.01; // 전압왜율(S)
				else if(i==38) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[98], p[99]) * 0.01; // 전압왜율(T)
				else if(i==39) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[100], p[101]) * 0.01; // 전류왜율(R)
				else if(i==40) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[102], p[103]) * 0.01; // 전류왜율(S)
				else if(i==41) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[104], p[105]) * 0.01; // 전류왜율(T)
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
				break;
															
			case DPM_DM2C_MT71 :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4(p[44], p[45], p[46], p[47]) * 0.001;		// R상전류
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4(p[48], p[49], p[50], p[51]) * 0.001;		// S상전류
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4(p[52], p[53], p[54], p[55]) * 0.001;		// T상전류
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4(p[56], p[57], p[58], p[59]) * 0.001;		// R상전압
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4(p[60], p[61], p[62], p[63]) * 0.001;		// S상전압
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4(p[64], p[65], p[66], p[67]) * 0.001;		// T상전압
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[68], p[69], p[70], p[71]) * 0.001;		// 음수역전력
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[72], p[73], p[74], p[75]) * 0.001;		// 음수진상1(VAR)
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[76], p[77], p[78], p[79]) * 0.001;		// 음수진상2(PF)
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4(p[80], p[81], p[82], p[83]) * 0.001;		// 주파수
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4(p[84], p[85], p[86], p[87]) * 0.001;		// 전력
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4(p[88], p[89], p[90], p[91]) * 0.001;		// 무효전력
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4(p[96], p[97], p[98], p[99]) * 0.001;		// 역전력량
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4(p[100], p[101], p[102], p[103]) * 0.001;	// 역무효전력
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case DPM_ODT3XX :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[ 0],  p[ 1]); // 선간전압(RS)
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[ 2],  p[ 3]); // 선간전압(ST)
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[ 4],  p[ 5]); // 선간전압(TR)
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[ 6],  p[ 7]); // 상전류(R)
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[ 8],  p[ 9]); // 상전류(S)
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[10],  p[11]); // 상전류(T)
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[12],  p[13]); // 전력
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[14],  p[15]); // 역율
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[16],  p[17]); // 무효전력
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[18],  p[19]) / 10.; // 주파수
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_uint_value4(p[20], p[21], p[22], p[23]); // 적산전력
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_uint_value4(p[24], p[25], p[26], p[27]); // 뮤효전력량
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case DPM_ODT3XX_GLOFA_FENET :
				if(i >= 0 && i <= 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value(p[i*2+1], p[i*2]);
				else if(i== 9)       pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value(p[i*2+1], p[i*2])/10; /*주파수*/
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case DPM_GiMACi :
			case DPM_GiMACi_SG1040 :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 3], p[ 2], p[ 1], p[ 0]); // 평균전압
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 7], p[ 6], p[ 5], p[ 4]); // 평균전류
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[11], p[10], p[ 9], p[ 8]); // R상전류
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[15], p[14], p[13], p[12]); // S상전류
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[19], p[18], p[17], p[16]); // T상전류
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[23], p[22], p[21], p[20]); // R상전압
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[27], p[26], p[25], p[24]); // S상전압
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[31], p[30], p[29], p[28]); // T상전압
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[35], p[34], p[33], p[32]); // RS상전압
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[39], p[38], p[37], p[36]); // ST상전압
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[43], p[42], p[41], p[40]); // TR상전압
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[47], p[46], p[45], p[44]); // 역률
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[51], p[50], p[49], p[48]) / 1000.; // 유효전력
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[55], p[54], p[53], p[52]) / 1000.; // 무효전력
				else if(i==14) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[59], p[58], p[57], p[56]) / 1000.; // 피상전력
				else if(i==15) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[63], p[62], p[61], p[60]); // 주파수
				else if(i==16) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[67], p[66], p[65], p[64]) / 1000000.; // 유효전력량
				else if(i==17) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[71], p[70], p[69], p[68]) / 1000.; // 무효전력량
				else if(i==18) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[75], p[74], p[73], p[72]) / 1000.; // 피상전력량
				else if(i==19) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[79], p[78], p[77], p[76]); // Ia부하율
				else if(i==20) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[83], p[82], p[81], p[80]); // Ib부하율
				else if(i==21) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[87], p[86], p[85], p[84]); // Ic부하율
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case RTU_DPM_GiMACi :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 0], p[ 1]) / 10.; // 평균전압
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 2], p[ 3]) / 10.; // 평균전류
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 8], p[ 5]) / 10.; // R상전류
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 6], p[ 7]) / 10.; // S상전류
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 8], p[ 9]) / 10.; // T상전류
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[10], p[11]) / 10.; // R상전압
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[12], p[13]) / 10.; // S상전압
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[14], p[15]) / 10.; // T상전압
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[16], p[17]) / 10.; // RS상전압
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[18], p[19]) / 10.; // ST상전압
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[20], p[21]) / 10.; // TR상전압
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[22], p[23]) / 10.; // 역률
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[24], p[25]) / 10.; // 유효전력
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[26], p[27]) / 10.; // 무효전력
				else if(i==14) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[28], p[29]) / 10.; // 피상전력
				else if(i==15) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[30], p[31]) / 10.; // 주파수
				else if(i==16) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[32], p[33]) / 10.; // 유효전력량
				else if(i==17) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[34], p[35]) / 10.; // 무효전력량
				else if(i==18) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[36], p[37]) / 10.; // 피상전력량
				else if(i==19) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[38], p[39]) / 10.; // Ia부하율
				else if(i==20) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[40], p[41]) / 10.; // Ib부하율
				else if(i==21) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[42], p[43]) / 10.; // Ic부하율
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case DPM_GiPAM115FI :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 3], p[ 2], p[ 1], p[ 0]); // R상전압
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 7], p[ 6], p[ 5], p[ 4]); // S상전압
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[11], p[10], p[ 9], p[ 8]); // T상전압
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[15], p[14], p[13], p[12]); // RS상전압
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[19], p[18], p[17], p[16]); // ST상전압
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[23], p[22], p[21], p[20]); // TS상전압
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[27], p[26], p[25], p[24]); // 영상전압
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[47], p[46], p[45], p[44]); // 최대영상전압
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[31], p[30], p[29], p[28]); // R상전류
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[35], p[34], p[33], p[32]); // S상전류
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[39], p[38], p[37], p[36]); // T상전류
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[43], p[42], p[41], p[40]); // 주파수
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[59], p[58], p[57], p[56]); // 역률
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[51], p[50], p[49], p[48]) / 1000.; // 유효전력
				else if(i==14) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[63], p[62], p[61], p[60]) / 1000.; // 유효전력량
				else if(i==15) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[55], p[54], p[53], p[52]) / 1000.; // 무효전력
				else if(i==16) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[67], p[66], p[65], p[64]) / 1000.; // 무효전력량
				else if(i==17) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[71], p[70], p[69], p[68]); // 위상차(R)
				else if(i==18) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[75], p[74], p[73], p[72]); // 위상차(S)
				else if(i==19) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[79], p[78], p[77], p[76]); // 위상차(T)
				else if(i==20) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[83], p[82], p[81], p[80]); // 영상위상차
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
				
			case RTU_DPM_GiPAM115FI :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 0], p[ 1]) / 10.; // R상전압
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 2], p[ 3]) / 10.; // S상전압
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 8], p[ 5]) / 10.; // T상전압
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 6], p[ 7]) / 10.; // RS상전압
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 8], p[ 9]) / 10.; // ST상전압
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[10], p[11]) / 10.; // TS상전압
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[12], p[13]) / 10.; // 영상전압
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[14], p[15]) / 10.; // 최대영상전압
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[16], p[17]) / 10.; // R상전류
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[18], p[19]) / 10.; // S상전류
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[20], p[21]) / 10.; // T상전류
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[22], p[23]) / 10.; // 주파수
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[24], p[25]) / 10.; // 역률
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[26], p[27]) / 10.; // 유효전력
				else if(i==14) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[28], p[29]) / 10.; // 유효전력량
				else if(i==15) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[30], p[31]) / 10.; // 무효전력
				else if(i==16) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[32], p[33]) / 10.; // 무효전력량
				else if(i==17) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[34], p[35]) / 10.; // 위상차(R)
				else if(i==18) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[36], p[37]) / 10.; // 위상차(S)
				else if(i==19) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[38], p[39]) / 10.; // 위상차(T)
				else if(i==20) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[40], p[41]) / 10.; // 영상위상차
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case DPM_GiMACIIPlus :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 27], p[ 26], p[ 25], p[ 24]); // RS상전압
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 31], p[ 30], p[ 29], p[ 28]); // ST상전압
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 35], p[ 34], p[ 33], p[ 32]); // TS상전압
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 15], p[ 14], p[ 13], p[ 12]); // R상전압
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 19], p[ 18], p[ 17], p[ 16]); // S상전압
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 23], p[ 22], p[ 21], p[ 20]); // T상전압
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 71], p[ 70], p[ 69], p[ 68]); // 영상전압
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 83], p[ 82], p[ 81], p[ 80]); // 정상전압
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 87], p[ 86], p[ 85], p[ 84]); // 역상전압
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[  3], p[  2], p[  1], p[  0]); // R상전류
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[  7], p[  6], p[  5], p[  4]); // S상전류
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 11], p[ 10], p[  9], p[  8]); // T상전류
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 95], p[ 94], p[ 93], p[ 92]); // 정상전류
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 99], p[ 98], p[ 97], p[ 96]); // 역상전류
				else if(i==14) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 43], p[ 42], p[ 41], p[ 40]) / 1000.; // 유효전력
				else if(i==15) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 47], p[ 46], p[ 45], p[ 44]) / 1000.; // 무효전력
				else if(i==16) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 51], p[ 50], p[ 49], p[ 48]) / 1000.; // 피상전력
				else if(i==17) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 59], p[ 58], p[ 57], p[ 56]) / 1000.; // 유효전력량
				else if(i==18) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 63], p[ 62], p[ 61], p[ 60]) / 1000.; // 무효전력량
				else if(i==19) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 67], p[ 66], p[ 65], p[ 64]) / 1000.; // 역유효전력량
				else if(i==20) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 55], p[ 54], p[ 53], p[ 52]); // 주파수
				else if(i==21) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 39], p[ 38], p[ 37], p[ 36]); // 역률
				else if(i==22) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[147], p[146], p[145], p[144]); // R상전류역률
				else if(i==23) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[151], p[150], p[149], p[148]); // S상전류역률
				else if(i==24) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[155], p[154], p[153], p[152]); // T상전류역률
				else if(i==25) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[123], p[122], p[121], p[120]); // RS상전압위상
				else if(i==26) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[127], p[126], p[125], p[124]); // ST상전압위상
				else if(i==27) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[131], p[130], p[129], p[128]); // TR상전압위상
				else if(i==28) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[111], p[110], p[109], p[108]); // R상전압위상
				else if(i==29) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[115], p[114], p[113], p[112]); // S상전압위상
				else if(i==30) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[119], p[118], p[117], p[116]); // T상전압위상
				else if(i==31) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[135], p[134], p[133], p[132]); // R상전류위상
				else if(i==32) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[139], p[138], p[137], p[136]); // S상전류위상
				else if(i==33) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[143], p[142], p[141], p[140]); // T상전류위상
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
				
			case RTU_DPM_GiMACIIPlus :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 0], p[ 1]) / 10.; // RS상전압
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 2], p[ 3]) / 10.; // ST상전압
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 8], p[ 5]) / 10.; // TS상전압
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 6], p[ 7]) / 10.; // R상전압
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 8], p[ 9]) / 10.; // S상전압
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[10], p[11]) / 10.; // T상전압
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[12], p[13]) / 10.; // 영상전압
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[14], p[15]) / 10.; // 정상전압
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[16], p[17]) / 10.; // 역상전압
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[18], p[19]) / 10.; // R상전류
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[20], p[21]) / 10.; // S상전류
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[22], p[23]) / 10.; // T상전류
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[24], p[25]) / 10.; // 정상전류
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[26], p[27]) / 10.; // 역상전류
				else if(i==14) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[28], p[29]) / 10.; // 유효전력
				else if(i==15) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[30], p[31]) / 10.; // 무효전력
				else if(i==16) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[32], p[33]) / 10.; // 피상전력
				else if(i==17) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[34], p[35]) / 10.; // 유효전력량
				else if(i==18) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[36], p[37]) / 10.; // 무효전력량
				else if(i==19) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[38], p[39]) / 10.; // 역유효전력량
				else if(i==20) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[40], p[41]) / 10.; // 주파수
				else if(i==21) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[42], p[43]) / 10.; // 역률
				else if(i==22) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[44], p[45]) / 10.; // R상전류역률
				else if(i==23) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[46], p[47]) / 10.; // S상전류역률
				else if(i==24) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[48], p[49]) / 10.; // T상전류역률
				else if(i==25) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[50], p[51]) / 10.; // RS상전압위상
				else if(i==26) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[52], p[53]) / 10.; // ST상전압위상
				else if(i==27) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[54], p[55]) / 10.; // TR상전압위상
				else if(i==28) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[56], p[57]) / 10.; // R상전압위상
				else if(i==29) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[58], p[59]) / 10.; // S상전압위상
				else if(i==30) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[60], p[61]) / 10.; // T상전압위상
				else if(i==31) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[62], p[63]) / 10.; // R상전류위상
				else if(i==32) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[64], p[65]) / 10.; // S상전류위상
				else if(i==33) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[66], p[67]) / 10.; // T상전류위상
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case DPM_ODE1XX :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[ 0],  p[ 1]);
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[ 2],  p[ 3]);
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[ 4],  p[ 5]);
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[ 6],  p[ 7]);
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[ 8],  p[ 9]);
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[10],  p[11]);
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[12],  p[13]);
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[14],  p[15]);
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[16],  p[17]);
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[18],  p[19]);
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[20],  p[21]);
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[22],  p[23]);
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[24],  p[25]);
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[26],  p[27]);
				else if(i==14) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[28],  p[29]);
				else if(i==15) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[30],  p[31]);
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case DPM_TMECS_200 :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[0], p[1]) / 10.; // 전압
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[2], p[3]) / 10.; // 전류
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[4], p[5]) / 10.; // 역률
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[6], p[7]) / 10.; // 주파수
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_ushort_value(p[8], p[9]) / 10.; // 유효전력
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_uint_value4(p[14], p[15], p[16], p[17]) / 10.; // 유효전력량
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case DPM_GiPAM2000FIM :
			case DPM_GiPAM2000FIM_DI :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[  3], p[  2], p[  1], p[  0]) / 1000.; // R상전압
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[  7], p[  6], p[  5], p[  4]) / 1000.; // S상전압
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 11], p[ 10], p[  9], p[  8]) / 1000.; // T상전압
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 15], p[ 14], p[ 13], p[ 12]) / 1000.; // RS상전압
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 19], p[ 18], p[ 17], p[ 16]) / 1000.; // ST상전압
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 23], p[ 22], p[ 21], p[ 20]) / 1000.; // TS상전압
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 27], p[ 26], p[ 25], p[ 24]); // 영상전압
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 31], p[ 30], p[ 29], p[ 28]); // R상전류
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 35], p[ 34], p[ 33], p[ 32]); // S상전류
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 39], p[ 38], p[ 37], p[ 36]); // T상전류
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 43], p[ 42], p[ 41], p[ 40]); // 영상전류
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 47], p[ 46], p[ 45], p[ 44]); // B모선전압
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 51], p[ 50], p[ 49], p[ 48]); // 전압역상분
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 55], p[ 54], p[ 53], p[ 52]); // 전류역상분
				else if(i==14) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 59], p[ 58], p[ 57], p[ 56]); // 주파수
				else if(i==15) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 63], p[ 62], p[ 61], p[ 60]) / 1000.; // 피상전력
				else if(i==16) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 67], p[ 66], p[ 65], p[ 64]) / 1000.; // 유효전력
				else if(i==17) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 71], p[ 70], p[ 69], p[ 68]) / 1000.; // 무효전력
				else if(i==18) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 75], p[ 74], p[ 73], p[ 72]) / 1000.; // 역방향유효전력량
				else if(i==19) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 79], p[ 78], p[ 77], p[ 76]) / 1000.; // 역방향무효전력
				else if(i==20) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 83], p[ 82], p[ 81], p[ 80]); // 역률
				else if(i==21) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 87], p[ 86], p[ 85], p[ 84]) / 1000.; // 유효전력량
				else if(i==22) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 91], p[ 90], p[ 89], p[ 88]) / 1000.; // 무효전력량
				else if(i==23) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 95], p[ 94], p[ 93], p[ 92]) / 1000.; // 역방향유효전력량
				else if(i==24) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 99], p[ 98], p[ 97], p[ 96]); // 전압R상THD
				else if(i==25) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[103], p[102], p[101], p[100]); // 전압S상THD
				else if(i==26) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[107], p[106], p[105], p[104]); // 전압T상THD
				else if(i==27) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[115], p[114], p[113], p[112]); // 전류R상THD
				else if(i==28) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[119], p[118], p[117], p[116]); // 전류S상THD
				else if(i==29) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[123], p[122], p[121], p[120]); // 전류T상THD
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case DPM_ION_6200 :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[  0], p[  1]) / 10.; // R상전압
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[  2], p[  3]) / 10.; // S상전압
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[  4], p[  5]) / 10.; // T상전압
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[  6], p[  7]) / 10.; // 상전압평균
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[  8], p[  9]) / 10.; // RS상전압
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 10], p[ 11]) / 10.; // ST상전압
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 12], p[ 13]) / 10.; // TR상전압
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 14], p[ 15]) / 10.; // 선간전압평균
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 16], p[ 17]) / 10.; // R상전류
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 18], p[ 19]) / 10.; // S상전류
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 20], p[ 21]) / 10.; // T상전류
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 22], p[ 23]) / 10.; // 전류평균
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value (p[ 30], p[ 31]) / 100.; // 주파수
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value (p[ 32], p[ 33]) / 100.; // 토탈역률
				else if(i==14) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value (p[ 34], p[ 35]) / 100.; // 역률(R)
				else if(i==15) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value (p[ 36], p[ 37]) / 100.; // 역률(S)
				else if(i==16) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value (p[ 38], p[ 39]) / 100.; // 역률(T)
				else if(i==17) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value (p[ 40], p[ 41]); // 토탈유효전력
				else if(i==18) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value (p[ 42], p[ 43]); // 토탈무효전력
				else if(i==19) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value (p[ 44], p[ 45]); // 토탈피상전력
				else if(i==20) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4  (p[ 78], p[ 79], p[ 76], p[ 77]); // 수전유효전력량
				else if(i==21) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4  (p[ 82], p[ 83], p[ 80], p[ 81]); // 송전유효전력량
				else if(i==22) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4  (p[ 86], p[ 87], p[ 84], p[ 85]); // 수전무효전력량
				else if(i==23) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4  (p[ 90], p[ 91], p[ 88], p[ 89]); // 송전무효전력량
				else if(i==24) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4  (p[ 94], p[ 95], p[ 92], p[ 93]); // 송수전피상전력량
				else if(i==25) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value (p[ 46], p[ 47]); // 유효전력(R)
				else if(i==26) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value (p[ 48], p[ 49]); // 유효전력(S)
				else if(i==27) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value (p[ 50], p[ 51]); // 유효전력(T)
				else if(i==28) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value (p[ 52], p[ 53]); // 무효전력(R)
				else if(i==29) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value (p[ 54], p[ 55]); // 무효전력(S)
				else if(i==30) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value (p[ 56], p[ 57]); // 무효전력(T)
				else if(i==31) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value (p[ 58], p[ 59]); // 피상전력(R)
				else if(i==32) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value (p[ 60], p[ 61]); // 피상전력(S)
				else if(i==33) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value (p[ 62], p[ 63]); // 피상전력(T)
				else if(i==34) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value (p[ 64], p[ 65]); // 수요유효전력
				else if(i==35) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value (p[ 68], p[ 69]); // 수요무효전력
				else if(i==36) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value (p[ 70], p[ 71]); // 수요피상전력
				else if(i==37) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 96], p[ 97]) / 10.; // V1-THD
				else if(i==38) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 98], p[ 99]) / 10.; // V2-THD
				else if(i==39) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[100], p[101]) / 10.; // V3-THD
				else if(i==40) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[102], p[103]) / 10.; // I1-THD
				else if(i==41) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[104], p[105]) / 10.; // I2-THD
				else if(i==42) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[106], p[107]) / 10.; // I3-THD
				else if(i==43) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[108], p[109]) / 10.; // 수요전류(R)
				else if(i==44) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[110], p[111]) / 10.; // 수요전류(S)
				else if(i==45) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[112], p[113]) / 10.; // 수요전류(T)
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case DPM_ION_7650 :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 0], p[ 1])/10.; // R상전류
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 2], p[ 3])/10.; // S상전류
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 4], p[ 5])/10.; // T상전류
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 6], p[ 7])/10.; // I4
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 8], p[ 9])/10.; // I5
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[10], p[11])/10.; // 전류avg
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[12], p[13])/10.; // 전류avg_mn
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[14], p[15])/10.; // 전류avg_mx
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[16], p[17])/10.; // 전류avg_mean
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[18], p[19])/10.; // 주파수
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[20], p[21])/10.; // 주파수_mn
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[22], p[23])/10.; // 주파수_mx
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[24], p[25])/10.; // 주파수_mean
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[26], p[27])/10.; // 전압unbal
				else if(i==14) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[28], p[29])/10.; // 전류unbal
				else if(i==15) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4(p[30], p[31], p[32], p[33])/1000.; // 입력전압(R)
				else if(i==16) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4(p[34], p[35], p[36], p[37])/1000.; // 입력전압(S)
				else if(i==17) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4(p[38], p[39], p[40], p[41])/1000.; // 입력전압(T)
				else if(i==18) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4(p[42], p[43], p[44], p[45])/1000.; // 입력전압avg
				else if(i==19) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4(p[46], p[47], p[48], p[49])/1000.; // 입력전압avg_mx
				else if(i==20) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4(p[50], p[51], p[52], p[53])/1000.; // 입력전압(RS)
				else if(i==21) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4(p[54], p[55], p[56], p[57])/1000.; // 입력전압(ST)
				else if(i==22) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4(p[58], p[59], p[60], p[61])/1000.; // 입력전압(TR)
				else if(i==23) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4(p[62], p[63], p[64], p[65])/1000.; // 선간전압avg
				else if(i==24) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4(p[66], p[67], p[68], p[69])/1000.; // 선간전압avg_mx
				else if(i==25) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4(p[70], p[71], p[72], p[73])/1000.; // 선간전압avg_mx_mean
				else if(i==26) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 74], p[ 75], p[ 76], p[ 77]); // 유효전력(R)
				else if(i==27) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 78], p[ 79], p[ 80], p[ 81]); // 유효전력(S)
				else if(i==28) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 82], p[ 83], p[ 84], p[ 85]); // 유효전력(T)
				else if(i==29) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 86], p[ 87], p[ 88], p[ 89]); // 유효전력tot
				else if(i==30) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 90], p[ 91], p[ 92], p[ 93]); // 유효전력tot_max
				else if(i==31) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 94], p[ 95], p[ 96], p[ 97]); // 무효전력(R)
				else if(i==32) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 98], p[ 99], p[100], p[101]); // 무효전력(S)
				else if(i==33) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[102], p[103], p[104], p[105]); // 무효전력(T)
				else if(i==34) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[106], p[107], p[108], p[109]); // 무효전력tot
				else if(i==35) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[110], p[111], p[112], p[113]); // 무효전력tot_max
				else if(i==36) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[114], p[115], p[116], p[117]); // 피상전력(R)
				else if(i==37) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[118], p[119], p[120], p[121]); // 피상전력(S)
				else if(i==38) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[122], p[123], p[124], p[125]); // 피상전력(T)
				else if(i==39) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[126], p[127], p[128], p[129]); // 피상전력tot
				else if(i==30) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[130], p[131], p[132], p[133]); // 피상전력tot_max
				else if(i==31) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[134], p[135], p[136], p[137]); // 유효전력량(del)
				else if(i==42) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[138], p[139], p[140], p[141]); // 유효전력량(rec)
				else if(i==43) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[142], p[143], p[144], p[145]); // 무효전력량(del)
				else if(i==44) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[146], p[147], p[148], p[149]); // 무효전력량(rec)
				else if(i==45) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[150], p[151], p[152], p[153]); // 무효전력량(del+rec)
				else if(i==46) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[154], p[155])/100.; // 역률(R)
				else if(i==47) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[156], p[157])/100.; // 역률(S)
				else if(i==48) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[158], p[159])/100.; // 역률(T)
				else if(i==49) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[160], p[161])/100.; // 역률tot
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case DPM_KB_GD10_L01 :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 0], p[ 1])/100.; // Line1누설전류
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 2], p[ 3])/100.; // Line2누설전류
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 4], p[ 5])/100.; // Line3누설전류
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 6], p[ 7])/100.; // Line4누설전류
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 8], p[ 9])/100.; // Line5누설전류
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[10], p[11])/100.; // Line6누설전류
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[12], p[13])/100.; // Line7누설전류
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[14], p[15])/100.; // Line8누설전류
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[16], p[17])/100.; // Line9누설전류
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[18], p[19])/100.; // Line10누설전류
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case DPM_GE_F650 :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[  0], p[  1], p[  2], p[  3]); // R상전류
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[  4], p[  5], p[  6], p[  7]); // S상전류
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[  8], p[  9], p[ 10], p[ 11]); // T상전류
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 12], p[ 13], p[ 14], p[ 15])/1000.; // RS선간전압
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 16], p[ 17], p[ 18], p[ 19])/1000.; // ST선간전압
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 20], p[ 21], p[ 22], p[ 23])/1000.; // TR선간전압
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 24], p[ 25], p[ 26], p[ 27])/1000.; // R상전압
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 28], p[ 29], p[ 30], p[ 31])/1000.; // S상전압
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 32], p[ 33], p[ 34], p[ 35])/1000.; // T상전압
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 36], p[ 37], p[ 38], p[ 39]); // R상유효전력
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 40], p[ 41], p[ 42], p[ 43]); // R상무효전력
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 44], p[ 45], p[ 46], p[ 47]); // R상피상전력
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 48], p[ 49], p[ 50], p[ 51]); // S상유효전력
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 52], p[ 53], p[ 54], p[ 55]); // S상무효전력
				else if(i==14) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 56], p[ 57], p[ 58], p[ 59]); // S상피상전력
				else if(i==15) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 60], p[ 61], p[ 62], p[ 63]); // T상유효전력
				else if(i==16) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 64], p[ 65], p[ 66], p[ 67]); // T상무효전력
				else if(i==17) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 68], p[ 69], p[ 70], p[ 71]); // T상피상전력
				else if(i==18) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 72], p[ 73], p[ 74], p[ 75]); // 3상유효전력
				else if(i==19) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 76], p[ 77], p[ 78], p[ 79]); // 3상무효전력
				else if(i==20) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 80], p[ 81], p[ 82], p[ 83]); // 3상피상전력
				else if(i==21) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 84], p[ 85], p[ 86], p[ 87])/10.; // R상역률
				else if(i==22) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 88], p[ 89], p[ 90], p[ 91])/10.; // S상역률
				else if(i==23) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 92], p[ 93], p[ 94], p[ 95])/10.; // T상역률
				else if(i==24) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[ 96], p[ 97], p[ 98], p[ 99])/10.; // 3상역률
				else if(i==25) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[100], p[101], p[102], p[103])/1000.; // 주파수
				else if(i==26) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[104], p[105], p[106], p[107]); // 유효전력량
				else if(i==27) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[108], p[109], p[110], p[111]); // 무효전력량
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case DPM_RTP300 :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[  3], p[  2], p[  1], p[  0]); // 전압(R)
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[  7], p[  6], p[  5], p[  4]); // 전압(S)
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 11], p[ 10], p[  9], p[  8]); // 전압(T)
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 15], p[ 14], p[ 13], p[ 12]); // 선간전압(RS)
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 19], p[ 18], p[ 17], p[ 16]); // 선간전압(ST)
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 23], p[ 22], p[ 21], p[ 20]); // 선간전압(TR)
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 27], p[ 26], p[ 25], p[ 24]); // 전류(R)
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 31], p[ 30], p[ 29], p[ 28]); // 전류(S)
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 35], p[ 34], p[ 33], p[ 32]); // 전류(T)
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 39], p[ 38], p[ 37], p[ 36]); // 유효전력(R)
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 43], p[ 42], p[ 41], p[ 40]); // 유효전력(S)
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 47], p[ 46], p[ 45], p[ 44]); // 유효전력(T)
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 51], p[ 50], p[ 49], p[ 48]); // 토탈유효전력
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 55], p[ 54], p[ 53], p[ 52]); // 무효전력(R)
				else if(i==14) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 59], p[ 58], p[ 57], p[ 56]); // 무효전력(S)
				else if(i==15) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 63], p[ 62], p[ 61], p[ 60]); // 무효전력(T)
				else if(i==16) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 67], p[ 66], p[ 65], p[ 64]); // 토탈무효전력
				else if(i==17) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 71], p[ 70], p[ 69], p[ 68]); // 역율(R)
				else if(i==18) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 75], p[ 74], p[ 73], p[ 72]); // 역율(S)
				else if(i==19) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 79], p[ 78], p[ 77], p[ 76]); // 역율(T)
				else if(i==20) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 83], p[ 82], p[ 81], p[ 80]); // 토탈역율
				else if(i==21) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 87], p[ 86], p[ 85], p[ 84]); // 주파수
				else if(i==22) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 91], p[ 90], p[ 89], p[ 88]); // 유효전력량
				else if(i==23) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 95], p[ 94], p[ 93], p[ 92]); // 무효전력량
				else if(i==24) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 96], p[ 97]); // 전압왜율(R)
				else if(i==25) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 98], p[ 99]); // 전압왜율(S)
				else if(i==26) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[100], p[101]); // 전압왜율(T)
				else if(i==27) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[102], p[103]); // 전류왜율(R)
				else if(i==28) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[104], p[105]); // 전류왜율(S)
				else if(i==29) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[106], p[107]); // 전류왜율(T)
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case DPM_RTM050 :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 0], p[ 1]) * (((p[12]<<8)|p[13])==1?0.1:((p[12]<<8)|p[13])==2?1:((p[12]<<8)|p[13])==4?10.0:((p[13]<<8)|p[12])==8?100.0:1000.0); // 전압(R)
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 2], p[ 3]) * (((p[12]<<8)|p[13])==1?0.1:((p[12]<<8)|p[13])==2?1:((p[12]<<8)|p[13])==4?10.0:((p[13]<<8)|p[12])==8?100.0:1000.0); // 전압(S)
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 4], p[ 5]) * (((p[12]<<8)|p[13])==1?0.1:((p[12]<<8)|p[13])==2?1:((p[12]<<8)|p[13])==4?10.0:((p[13]<<8)|p[12])==8?100.0:1000.0); // 전압(T)
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 6], p[ 7]) * (((p[12]<<8)|p[13])==1?0.1:((p[12]<<8)|p[13])==2?1:((p[12]<<8)|p[13])==4?10.0:((p[13]<<8)|p[12])==8?100.0:1000.0); // 선간전압(RS)
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[ 8], p[ 9]) * (((p[12]<<8)|p[13])==1?0.1:((p[12]<<8)|p[13])==2?1:((p[12]<<8)|p[13])==4?10.0:((p[13]<<8)|p[12])==8?100.0:1000.0); // 선간전압(ST)
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[10], p[11]) * (((p[12]<<8)|p[13])==1?0.1:((p[12]<<8)|p[13])==2?1:((p[12]<<8)|p[13])==4?10.0:((p[13]<<8)|p[12])==8?100.0:1000.0); // 선간전압(TR)
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[14], p[15]) * (((p[20]<<8)|p[21])==1?0.001:((p[20]<<8)|p[21])==2?0.01:((p[20]<<8)|p[21])==4?0.1:1.0); // 전류(R)
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[16], p[17]) * (((p[20]<<8)|p[21])==1?0.001:((p[20]<<8)|p[21])==2?0.01:((p[20]<<8)|p[21])==4?0.1:1.0); // 전류(S)
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[18], p[19]) * (((p[20]<<8)|p[21])==1?0.001:((p[20]<<8)|p[21])==2?0.01:((p[20]<<8)|p[21])==4?0.1:1.0); // 전류(T)
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value (p[22], p[23]) * (((p[24]<<8)|p[25])==1?0.001:((p[24]<<8)|p[25])==2?0.01:((p[24]<<8)|p[25])==4?0.1:((p[25]<<8)|p[24])==8?1.0:10.0); // 토탈유효전력
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value (p[26], p[27]) * (((p[28]<<8)|p[29])==1?0.001:((p[28]<<8)|p[29])==2?0.01:((p[28]<<8)|p[29])==4?0.1:((p[29]<<8)|p[28])==8?1.0:10.0); // 토탈무효전력
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value (p[30], p[31]) * 0.1; // 토탈역율
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[32], p[33]) * 0.1; // 주파수
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_ushort_value(p[34], p[35]) * 0.1; // 온도
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case DPM_MT4Y :
			case DPM_MT4Y_DYRCU :
				/* Dot 위치 정보 Dot Setting */
				dot = get_short_value(p[2], p[3])==0x01 ?   10 :
					  get_short_value(p[2], p[3])==0x02 ?  100 :
					  get_short_value(p[2], p[3])==0x03 ? 1000 : 1;
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)(get_short_value(p[0], p[1]) / dot); /* 측정값(전압) Measure Value */
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)(get_short_value(p[4], p[5]) / dot); /* 최대전압 High Peak Value */
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)(get_short_value(p[6], p[7]) / dot); /* 최소전압 Low Peak Value */
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case DPM_KDY_200 :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 0], p[ 1]) / 10.;	// 선간전압 12(V)
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 2], p[ 3]) / 10.;	// 선간전압 23(V)
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 4], p[ 5]) / 10.;	// 선간전압 31(V)
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 6], p[ 7]) / 10.;	// 상전압 1(V)
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 8], p[ 9]) / 10.;	// 상전압 2(V)
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[10], p[11]) / 10.;	// 상전압 3(V)
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[12], p[13]) / 10.;	// 전류 1(A)
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[14], p[15]) / 10.;	// 전류 2(A)
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[16], p[17]) / 10.;	// 전류 3(A)
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[18], p[19]) / 10.;	// 유효전력 1(W)
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[20], p[21]) / 10.;	// 유효전력 2(W)
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[22], p[23]) / 10.;	// 유효전력 3(W)
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[24], p[25]) / 10.;	// 유효전력 Total(W)
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[26], p[27]) / 10.;	// 무효전력 1(var)
				else if(i==14) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[28], p[29]) / 10.;	// 무효전력 2(var)
				else if(i==15) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[30], p[31]) / 10.;	// 무효전력 3(var)
				else if(i==16) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[32], p[33]) / 10.;	// 무효전력 Total(var)
				else if(i==17) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[34], p[35]) / 10.;	// 피상전력 1(VA)
				else if(i==18) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[36], p[37]) / 10.;	// 피상전력 2(VA)
				else if(i==19) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[38], p[39]) / 10.;	// 피상전력 3(VA)
				else if(i==20) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[40], p[41]) / 10.;	// 피상전력 Total(VA)
				else if(i==21) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[42], p[43]) / 10.;	// 역률 1(PF)
				else if(i==22) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[44], p[45]) / 10.;	// 역률 2(PF)
				else if(i==23) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[46], p[47]) / 10.;	// 역률 3(PF)
				else if(i==24) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[48], p[49]) / 10.;	// 평균 (PF)
				else if(i==25) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[50], p[51]) / 100.;	// 주파수 (Hz)
				else if(i==26) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[52], p[53], p[54], p[55]);	// 전체 유효전력량 (+Wh) K->L 정방향
				else if(i==27) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[56], p[57], p[58], p[59]);	// 전체 유효전력량 (-Wh) L->K 역방향
				else if(i==28) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[60], p[61], p[62], p[63]);	// 전체 무효전력량 (+var)
				else if(i==29) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value4(p[64], p[65], p[66], p[67]);	// 전체 무효전력량 (-var)
				else if(i==30) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[68], p[69]) / 10.;	// 선간전압 최대값 12(V)
				else if(i==31) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[70], p[71]) / 10.;	// 선간전압 최대값 23(V)
				else if(i==32) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[72], p[73]) / 10.;	// 선간전압 최대값 31(V)
				else if(i==33) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[74], p[75]) / 10.;	// 전압 최대값 1(V)
				else if(i==34) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[76], p[77]) / 10.;	// 전압 최대값 2(V)
				else if(i==35) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[78], p[79]) / 10.;	// 전압 최대값 3(V)
				else if(i==36) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[80], p[81]) / 10.;	// 전류 최대값 1(A)
				else if(i==37) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[82], p[83]) / 10.;	// 전류 최대값 2(A)
				else if(i==38) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[84], p[85]) / 10.;	// 전류 최대값 3(A)
				else if(i==39) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[86], p[87]);		// 전력 최대값(W)
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
			//분전반 끝
			
			//STS 시작
			case RTU_SUPER_SWITCH3 :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[  0], p[  1], p[  2], p[  3]); //S1-전원LL전압(RS)
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[  4], p[  5], p[  6], p[  7]); //S1-전원전류(R)
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[  8], p[  9], p[ 10], p[ 11]); //S1-전원LL전압(ST)
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 12], p[ 13], p[ 14], p[ 15]); //S1-전원전류(S)
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 16], p[ 17], p[ 18], p[ 19]); //S1-전원LL전압(TR)
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 20], p[ 21], p[ 22], p[ 23]); //S1-전원전류(T)
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 24], p[ 25], p[ 26], p[ 27]); //S1-전원주파수
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 28], p[ 29], p[ 30], p[ 31]); //S1-출력전압(RS)
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 32], p[ 33], p[ 34], p[ 35]); //S1-출력전압(ST)
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 36], p[ 37], p[ 38], p[ 39]); //S1-출력전압(TR)
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 40], p[ 41], p[ 42], p[ 43]); //S1-전원총피상전력(R)
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 44], p[ 45], p[ 46], p[ 47]); //S1-전원총피상전력(S)
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 48], p[ 49], p[ 50], p[ 51]); //S1-전원총피상전력(T)
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 52], p[ 53], p[ 54], p[ 55]); //S1-IPeakphase(R)
				else if(i==14) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 56], p[ 57], p[ 58], p[ 59]); //S1-IPeakphase(S)
				else if(i==15) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 60], p[ 61], p[ 62], p[ 63]); //S1-IPeakphase(T)
				else if(i==16) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 64], p[ 65], p[ 66], p[ 67]); //S1-전원전력(R)
				else if(i==17) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 68], p[ 69], p[ 70], p[ 71]); //S1-전원전력(S)
				else if(i==18) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 72], p[ 73], p[ 74], p[ 75]); //S1-전원전력(T)
				else if(i==19) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 76], p[ 77], p[ 78], p[ 79]); //S1-역율(R)
				else if(i==20) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 80], p[ 81], p[ 82], p[ 83]); //S1-역율(S)
				else if(i==21) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 84], p[ 85], p[ 86], p[ 87]); //S1-역율(T)
				else if(i==22) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 88], p[ 89], p[ 90], p[ 91]); //S1-전원LN전압(R)
				else if(i==23) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 92], p[ 93], p[ 94], p[ 95]); //S1-전원LN전압(S)
				else if(i==24) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[ 96], p[ 97], p[ 98], p[ 99]); //S1-전원LN전압(T)
				else if(i==25) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[100], p[101], p[102], p[103]); //S1-전압THD(R)
				else if(i==26) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[104], p[105], p[106], p[107]); //S1-전압THD(S)
				else if(i==27) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[108], p[109], p[110], p[111]); //S1-전압THD(T)
				else if(i==28) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[112], p[113], p[114], p[115]); //S1-전류THD(R)
				else if(i==29) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[116], p[117], p[118], p[119]); //S1-전류THD(S)
				else if(i==30) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[120], p[121], p[122], p[123]); //S1-전류THD(T)

				else if(i==31) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[124], p[125], p[126], p[127]); //S2-전원LL전압(RS)
				else if(i==32) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[128], p[129], p[130], p[131]); //S2-전원전류(R)
				else if(i==33) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[132], p[133], p[134], p[135]); //S2-전원LL전압(ST)
				else if(i==34) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[136], p[137], p[138], p[139]); //S2-전원전류(S)
				else if(i==35) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[140], p[141], p[142], p[143]); //S2-전원LL전압(TR)
				else if(i==36) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[144], p[145], p[146], p[147]); //S2-전원전류(T)
				else if(i==37) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[148], p[149], p[150], p[151]); //S2-전원주파수
				else if(i==38) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[152], p[153], p[154], p[155]); //S2-출력전압(RS)
				else if(i==39) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[156], p[157], p[158], p[159]); //S2-출력전압(ST)
				else if(i==40) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[160], p[161], p[162], p[163]); //S2-출력전압(TR)
				else if(i==41) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[164], p[165], p[166], p[167]); //S2-전원총피상전력(R)
				else if(i==42) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[168], p[169], p[170], p[171]); //S2-전원총피상전력(S)
				else if(i==43) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[172], p[173], p[174], p[175]); //S2-전원총피상전력(T)
				else if(i==44) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[176], p[177], p[178], p[179]); //S2-IPeakphase(R)
				else if(i==45) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[180], p[181], p[182], p[183]); //S2-IPeakphase(S)
				else if(i==46) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[184], p[185], p[186], p[187]); //S2-IPeakphase(T)
				else if(i==47) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[188], p[189], p[190], p[191]); //S2-전원전력(R)
				else if(i==48) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[192], p[193], p[194], p[195]); //S2-전원전력(S)
				else if(i==49) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[196], p[197], p[198], p[199]); //S2-전원전력(T)
				else if(i==50) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[200], p[201], p[202], p[203]); //S2-역율(R)
				else if(i==51) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[204], p[205], p[206], p[207]); //S2-역율(S)
				else if(i==52) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[208], p[209], p[210], p[211]); //S2-역율(T)
				else if(i==53) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[212], p[213], p[214], p[215]); //S2-전원LN전압(R)
				else if(i==54) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[216], p[217], p[218], p[219]); //S2-전원LN전압(S)
				else if(i==55) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[220], p[221], p[222], p[223]); //S2-전원LN전압(T)
				else if(i==56) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[224], p[225], p[226], p[227]); //S2-전압THD(R)
				else if(i==57) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[228], p[229], p[230], p[231]); //S2-전압THD(S)
				else if(i==58) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[232], p[233], p[234], p[235]); //S2-전압THD(T)
				else if(i==59) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[236], p[237], p[238], p[239]); //S2-전류THD(R)
				else if(i==60) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[240], p[241], p[242], p[243]); //S2-전류THD(S)
				else if(i==61) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(p[244], p[245], p[246], p[247]); //S2-전류THD(T)
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case SUPER_SWITCH3 :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[ 0], p[ 1]); //S1-전원LL전압(RS)
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[ 2], p[ 3]); //S1-전원전류(R)
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[ 4], p[ 5]); //S1-전원LL전압(ST)
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[ 6], p[ 7]); //S1-전원전류(S)
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[ 8], p[ 9]); //S1-전원LL전압(TR)
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[10], p[11]); //S1-전원전류(T)
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[12], p[13], p[14], p[15]); //S1-전원주파수
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[16], p[17]); //S1-출력전압(RS)
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[18], p[19]); //S1-출력전압(ST)
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[20], p[21]); //S1-출력전압(TR)
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[22], p[23], p[24], p[25]); //S1-전원총피상전력(R)
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[26], p[27], p[28], p[29]); //S1-전원총피상전력(S)
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[30], p[31], p[32], p[33]); //S1-전원총피상전력(T)
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[34], p[35]); //S1-IPeakphase(R)
				else if(i==14) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[36], p[37]); //S1-IPeakphase(S)
				else if(i==15) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[38], p[39]); //S1-IPeakphase(T)
				else if(i==16) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[40], p[41], p[42], p[43]); //S1-전원전력(R)
				else if(i==17) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[44], p[45], p[46], p[47]); //S1-전원전력(S)
				else if(i==18) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[48], p[49], p[50], p[51]); //S1-전원전력(T)
				else if(i==19) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[52], p[53], p[54], p[55]); //S1-역율(R)
				else if(i==20) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[56], p[57], p[58], p[59]); //S1-역율(S)
				else if(i==21) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[60], p[61], p[62], p[63]); //S1-역율(T)
				else if(i==22) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[64], p[65]); //S1-전원LN전압(R)
				else if(i==23) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[66], p[67]); //S1-전원LN전압(S)
				else if(i==24) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[68], p[69]); //S1-전원LN전압(T)
				else if(i==25) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[70], p[71], p[72], p[73]); //S1-전압THD(R)
				else if(i==26) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[74], p[75], p[76], p[77]); //S1-전압THD(S)
				else if(i==27) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[78], p[79], p[80], p[81]); //S1-전압THD(T)
				else if(i==28) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[82], p[83], p[84], p[85]); //S1-전류THD(R)
				else if(i==29) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[86], p[87], p[88], p[89]); //S1-전류THD(S)
				else if(i==30) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[90], p[91], p[92], p[93]); //S1-전류THD(T)
				else if(i==31) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[94+ 0], p[94+ 1]); //S2-전원LL전압(RS)
				else if(i==32) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[94+ 2], p[94+ 3]); //S2-전원전류(R)
				else if(i==33) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[94+ 4], p[94+ 5]); //S2-전원LL전압(ST)
				else if(i==34) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[94+ 6], p[94+ 7]); //S2-전원전류(S)
				else if(i==35) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[94+ 8], p[94+ 9]); //S2-전원LL전압(TR)
				else if(i==36) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[94+10], p[94+11]); //S2-전원전류(T)
				else if(i==37) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[94+12], p[94+13], p[94+14], p[94+15]); //S2-전원주파수
				else if(i==38) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[94+16], p[94+17]); //S2-출력전압(RS)
				else if(i==39) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[94+18], p[94+19]); //S2-출력전압(ST)
				else if(i==40) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[94+20], p[94+21]); //S2-출력전압(TR)
				else if(i==41) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[94+22], p[94+23], p[94+24], p[94+25]); //S2-전원총피상전력(R)
				else if(i==42) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[94+26], p[94+27], p[94+28], p[94+29]); //S2-전원총피상전력(S)
				else if(i==43) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[94+30], p[94+31], p[94+32], p[94+33]); //S2-전원총피상전력(T)
				else if(i==44) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[94+34], p[94+35]); //S2-IPeakphase(R)
				else if(i==45) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[94+36], p[94+37]); //S2-IPeakphase(S)
				else if(i==46) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[94+38], p[94+39]); //S2-IPeakphase(T)
				else if(i==47) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[94+40], p[94+41], p[94+42], p[94+43]); //S2-전원전력(R)
				else if(i==48) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[94+44], p[94+45], p[94+46], p[94+47]); //S2-전원전력(S)
				else if(i==49) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[94+48], p[94+49], p[94+50], p[94+51]); //S2-전원전력(T)
				else if(i==50) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[94+52], p[94+53], p[94+54], p[94+55]); //S2-역율(R)
				else if(i==51) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[94+56], p[94+57], p[94+58], p[94+59]); //S2-역율(S)
				else if(i==52) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[94+60], p[94+61], p[94+62], p[94+63]); //S2-역율(T)
				else if(i==53) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[94+64], p[94+65]); //S2-전원LN전압(R)
				else if(i==54) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[94+66], p[94+67]); //S2-전원LN전압(S)
				else if(i==55) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)get_short_value (p[94+68], p[94+69]); //S2-전원LN전압(T)
				else if(i==56) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[94+70], p[94+71], p[94+72], p[94+73]); //S2-전압THD(R)
				else if(i==57) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[94+74], p[94+75], p[94+76], p[94+77]); //S2-전압THD(S)
				else if(i==58) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[94+78], p[94+79], p[94+80], p[94+81]); //S2-전압THD(T)
				else if(i==59) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[94+82], p[94+83], p[94+84], p[94+85]); //S2-전류THD(R)
				else if(i==60) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[94+86], p[94+87], p[94+88], p[94+89]); //S2-전류THD(S)
				else if(i==61) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_superswitch3_fvalue(p[94+90], p[94+91], p[94+92], p[94+93]); //S2-전류THD(T)
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
			//STS 끝

			//온습도 시작
			case MES100 :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 3], p[ 4]) / 10.; // 온도
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 5], p[ 6]) / 10.; // 습도
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
			case ITTS2 :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 3], p[ 4]) / 100.; // 온도
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 5], p[ 6]) / 100.; // 습도
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case INNOTUBE :
				if     (i== 0) 
				{
					pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : (float)(p[3]&0x7F) + ((float)p[4]/10.);			// 온도
					pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : p[3] & 0x80 ? -pAI->curr_val : pAI->curr_val;	// 온도
				}
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : p[ 2]; // 습도
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case RTU_TH1 :
			case RTU_TH2 :
			case RTU_TH3 :
			case RTU_TH4 :
			case RTU_TH5 :
			case RTU_TH6 :
			case RTU_TH7 :
			case RTU_TH8 :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 0], p[ 1]) / 10.;		// 온도
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 2], p[ 3]) / 10.;		// 습도
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
			//온습도 끝

			//계전기 시작
			case CIMON :
				if     (i== 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 0], p[ 1]);					// 상전압 R상
				else if(i== 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 2], p[ 3]);					// 상전압 S상
				else if(i== 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 4], p[ 5]);					// 상전압 T상
				else if(i== 3) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 6], p[ 7]);					// 선간전압 RS
				else if(i== 4) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[ 8], p[ 9]);					// 선간전압 ST
				else if(i== 5) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[10], p[11]);					// 선간전압 TR
				else if(i== 6) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[14], p[15]);					// 전류 R상
				else if(i== 7) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[16], p[17]);					// 전류 S상
				else if(i== 8) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[18], p[19]);					// 전류 T상
				else if(i== 9) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4(p[28], p[29], p[30], p[31]);		// 주파수
				else if(i==10) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[32], p[33]);					// 유효전력
				else if(i==11) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[34], p[35]);					// 무효전력
				else if(i==12) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value2(p[40], p[41]);					// 역률
				else if(i==13) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_int_value2(p[42], p[43]);					// 유효 전력량
				else if(i==14) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_uint_value4(p[44], p[45], p[46], p[47]);		// 무효 전력량
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
			//누수 거리형 시작
			case TTDM128 :
				if(i == 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[0],p[1]) & 0x0001 ? (float)get_short_value(p[2], p[3]) / 12.796 : 0.0;	//누수거리
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case A_LLM1 :
				if(i == 0)
					pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_short_value(p[0],p[1]) & 0x0001 ? (float)get_short_value(p[2], p[3]) / 10.0 : 0.0;	//누수거리
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
				
			case RTU_TTDM128 :
				if(i == 0)
				pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : p[0] & 0x01 ? (float)get_float_value4(p[1], p[2], p[3], p[4]) : 0.0;	//누수거리
				
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			//BMS 시작
			case BTECH :
				k = (i/3) * 4;
				     if((i % 3) == 0) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(buff[k+3], buff[k+2], buff[k+1], buff[k]);				// 전압
				else if((i % 3) == 1) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(buff[k+1027], buff[k+1026], buff[k+1025], buff[k+1024]);	// 온도
				else if((i % 3) == 2) pAI->curr_val = com_state==COM_ABNORMAL ? 0.0 : get_float_value4(buff[k+2051], buff[k+2050], buff[k+2049], buff[k+2048]);	// 저항
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
			default :
				pthread_mutex_unlock(pStatus->mux);
				continue;
		}
#ifdef DEBUG
        fileLog(WARNING, "[%s:%d] AI eseq=[%d] tseq=[%d] curr_val=[%f] \n", __FUNCTION__, __LINE__, pConnInfo->eseq, pAI->tseq, pAI->curr_val);
#endif
		
		if(pAI->use_yn == YES && com_state == COM_NORMAL)
		{
			if(pAI->alarm_yn == YES)
			{
				if(pAI->tm_alarm==0) pAI->tm_alarm = time(NULL);

				if((pAI->curr_val > pAI->critical_high || pAI->curr_val < pAI->critical_low) && pAI->critical_alarm_yn == YES)
				{
					pAI->curr_status = CRITICAL;
				}
				else if((pAI->curr_val > pAI->warning_high || pAI->curr_val < pAI->warning_low) && pAI->warning_alarm_yn == YES)
				{
					pAI->curr_status = WARNING;
				}
				else if((pAI->curr_val > pAI->info_high || pAI->curr_val < pAI->info_low) && pAI->info_alarm_yn == YES)
				{
					pAI->curr_status = INFO;
				}
				else
				{
					pAI->curr_status = NORMAL;
					pAI->tm_alarm = time(NULL);
				}
				
				// 알람 일때
				if(pAI->send_flag==0 && pAI->curr_status != NORMAL )
				{
					ai_diff = difftime(now, pAI->tm_alarm);
					if(ai_diff >= pAI->alarm_term)
					{
						pAI->send_flag = 1;
						alarm_insert(OCCURE, index, AI, i, pAI->curr_status);
						pAI->last_status = pAI->curr_status;
						//LOG_ALARM INSERT
					}
				}
				// 더 상위 알람
				else if(pAI->send_flag==1 && pAI->last_status < pAI->curr_status)
				{
					alarm_insert(CLEAR, index, AI, i, pAI->last_status);
					alarm_insert(OCCURE, index, AI, i, pAI->curr_status);
					pAI->last_status = pAI->curr_status;
				}
				// 알람이 복구 될때
				else if(pAI->send_flag == 1 && pAI->curr_status == NORMAL)
				{
					pAI->send_flag = 0;
					alarm_insert(CLEAR, index, AI, i, pAI->last_status);
					//LOG_ALARM RECOVER
				}
			}
		}


		ai_diff = difftime(now, pAI->tm_save);
		if(pAI->use_yn == YES)
		{
			if(pAI->save_yn==YES && (ai_diff >= pAI->save_term || pAI->tm_save == 0) )
			{
				mysqlinsertData(pConnInfo->eseq, pAI->tseq, pAI->curr_val);
				pAI->tm_save = time(NULL);
			}

			//데이터 로그 업데이트
			//update_diff = difftime(now, pConnInfo->tm_update);
			update_diff = difftime(now, pAI->tm_update);
			if(update_diff >= pAI->update_term || pAI->tm_update == 0)
			{
				mysqlUpdateTag(pConnInfo->eseq, pAI->tseq, pAI->curr_val);
				pAI->tm_update = time(NULL);
			}
		}
		pthread_mutex_unlock(pStatus->mux);
	}

	//DI 시작
    for(i = 0; i < pConnInfo->di_count - 1; i++, pDI++)
    {
        pthread_mutex_lock(pStatus->mux);
		
        switch(pConnInfo->model_seq)
        {
			//분전반 시작			
			case RTU_MPM330A :
				if(i==0) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[122] & 0x01) ? DI_ON : DI_OFF; // RTU와 장비 통신불량
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
			//UPS 시작
			case UPS_HIPULSE_U :
				if     (i== 0) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[104]<<8|p[105]) & 0x0001 ? DI_ON : DI_OFF; // UPS Alarm
				else if(i== 1) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[104]<<8|p[105]) & 0x0010 ? DI_ON : DI_OFF; // 정류부입력전압이상
				else if(i== 2) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[104]<<8|p[105]) & 0x0020 ? DI_ON : DI_OFF; // 정류부고장
				else if(i== 3) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[104]<<8|p[105]) & 0x0040 ? DI_ON : DI_OFF; // 인버터고장
				else if(i== 4) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[104]<<8|p[105]) & 0x0080 ? DI_ON : DI_OFF; // Byp. volt out spec
				else if(i== 5) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[104]<<8|p[105]) & 0x0100 ? DI_ON : DI_OFF; // 축전지저전압
				else if(i== 6) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[104]<<8|p[105]) & 0x0200 ? DI_ON : DI_OFF; // 축전지고전압
				else if(i== 7) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[104]<<8|p[105]) & 0x0300 ? DI_ON : DI_OFF; // 축전지방전
				else if(i== 8) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[104]<<8|p[105]) & 0x0800 ? DI_ON : DI_OFF; // 정류부입력주파수이상
				else if(i== 9) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[104]<<8|p[105]) & 0x1000 ? DI_ON : DI_OFF; // 입력퓨즈고장
				else if(i==10) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[104]<<8|p[105]) & 0x2000 ? DI_ON : DI_OFF; // Main phase Reversed
				else if(i==11) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[104]<<8|p[105]) & 0x4000 ? DI_ON : DI_OFF; // REC input phase missing
				else if(i==12) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[104]<<8|p[105]) & 0x8000 ? DI_ON : DI_OFF; // 컨트롤파워1고장

				else if(i==13) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[106]<<8|p[107]) & 0x0001 ? DI_ON : DI_OFF; // 컨트롤파워2고장
				else if(i==14) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[106]<<8|p[107]) & 0x0002 ? DI_ON : DI_OFF; // Rectifire current limit
				else if(i==15) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[106]<<8|p[107]) & 0x0008 ? DI_ON : DI_OFF; // Rectifire overtemp
				else if(i==16) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[106]<<8|p[107]) & 0x0010 ? DI_ON : DI_OFF; // Input Filter Fault
				else if(i==17) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[106]<<8|p[107]) & 0x0040 ? DI_ON : DI_OFF; // Filter contactor fault
				else if(i==18) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[106]<<8|p[107]) & 0x0080 ? DI_ON : DI_OFF; // REC driver cirtuit fault
				else if(i==19) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[106]<<8|p[107]) & 0x0200 ? DI_ON : DI_OFF; // Inverter Fault
				else if(i==20) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[106]<<8|p[107]) & 0x0400 ? DI_ON : DI_OFF; // Fan fault
				else if(i==21) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[106]<<8|p[107]) & 0x0800 ? DI_ON : DI_OFF; // 인버터 STS fail
				else if(i==22) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[106]<<8|p[107]) & 0x1000 ? DI_ON : DI_OFF; // 바이패스 STS fail
				else if(i==23) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[106]<<8|p[107]) & 0x2000 ? DI_ON : DI_OFF; // Operation invalid
				else if(i==24) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[106]<<8|p[107]) & 0x4000 ? DI_ON : DI_OFF; // Unit overload
				else if(i==25) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[106]<<8|p[107]) & 0x8000 ? DI_ON : DI_OFF; // System overload

				else if(i==26) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[108]<<8|p[109]) & 0x0002 ? DI_ON : DI_OFF; // Byp. Abnomal shutdown
				else if(i==27) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[108]<<8|p[109]) & 0x0004 ? DI_ON : DI_OFF; // 출력과전압
				else if(i==28) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[108]<<8|p[109]) & 0x0008 ? DI_ON : DI_OFF; // 인버터과전류
				else if(i==29) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[108]<<8|p[109]) & 0x0010 ? DI_ON : DI_OFF; // Byp. phase reversed
				else if(i==30) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[108]<<8|p[109]) & 0x0020 ? DI_ON : DI_OFF; // Load impact transfer
				else if(i==31) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[108]<<8|p[109]) & 0x0040 ? DI_ON : DI_OFF; // transfer timeout
				else if(i==32) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[108]<<8|p[109]) & 0x0080 ? DI_ON : DI_OFF; // Load sharing fault
				else if(i==33) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[108]<<8|p[109]) & 0x0100 ? DI_ON : DI_OFF; // DC Bus abnormal
				else if(i==34) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[108]<<8|p[109]) & 0x0200 ? DI_ON : DI_OFF; // System transfer
				else if(i==35) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[108]<<8|p[109]) & 0x0400 ? DI_ON : DI_OFF; // Parallel board fault
				else if(i==36) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[108]<<8|p[109]) & 0x0800 ? DI_ON : DI_OFF; // Parallel connect fault
				else if(i==37) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[108]<<8|p[109]) & 0x2000 ? DI_ON : DI_OFF; // 바이패스과전류

				else if(i==38) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[110]<<8|p[111]) & 0x0001 ? DI_ON : DI_OFF; // Byp. induct overtemp
				else if(i==39) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[110]<<8|p[111]) & 0x0002 ? DI_ON : DI_OFF; // Static Sw. overtemp
				else if(i==40) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[110]<<8|p[111]) & 0x0004 ? DI_ON : DI_OFF; // Byp. feedback fault
				else if(i==41) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[110]<<8|p[111]) & 0x0008 ? DI_ON : DI_OFF; // INV. driver circuit fault
				else if(i==42) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[110]<<8|p[111]) & 0x0010 ? DI_ON : DI_OFF; // Inverter comm. fault
				else if(i==43) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[110]<<8|p[111]) & 0x0020 ? DI_ON : DI_OFF; // System Battery low
				else if(i==44) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[110]<<8|p[111]) & 0x0080 ? DI_ON : DI_OFF; // Ambient overtemp
				else if(i==45) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[110]<<8|p[111]) & 0x0400 ? DI_ON : DI_OFF; // Battery overtemp
				else if(i==46) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[110]<<8|p[111]) & 0x0800 ? DI_ON : DI_OFF; // Battery ground fault                
				else if(i==47) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[110]<<8|p[111]) & 0x1000 ? DI_ON : DI_OFF; // Battery fuse fault

				else if(i==48) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[112]<<8|p[113]) & 0x0001 ? DI_ON : DI_OFF; // Bus capacitor over volt
				else if(i==49) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[112]<<8|p[113]) & 0x0002 ? DI_ON : DI_OFF; // DC bus over volt
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
			//항온항습기 시작
			case AR_HANE09 :
			case AR_HANE09_DYRCU :
			case AR_HANE09_EXOR :
			case AR_HANE09_Q :
				if     (i== 0) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[32]=='1') ? DI_ON : DI_OFF; // FAN
				else if(i== 1) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[33]=='1') ? DI_ON : DI_OFF; // 냉방1(CS1)
				else if(i== 2) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[34]=='1') ? DI_ON : DI_OFF; // 냉방2(CS2)
				else if(i== 3) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[35]=='1') ? DI_ON : DI_OFF; // 난방히터1
				else if(i== 4) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[36]=='1') ? DI_ON : DI_OFF; // 난방히터2
				else if(i== 5) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[37]=='1') ? DI_ON : DI_OFF; // 난방히터3
				else if(i== 6) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[38]=='1') ? DI_ON : DI_OFF; // 난방히터4
				else if(i== 7) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[39]=='1') ? DI_ON : DI_OFF; // 난방히터5
				else if(i== 8) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[40]=='1') ? DI_ON : DI_OFF; // 가습
				else if(i== 9) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[41]=='1') ? DI_ON : DI_OFF; // 급수
				else if(i==10) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[42]=='1') ? DI_ON : DI_OFF; // 배수
				else if(i==11) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[44]=='1') ? DI_ON : DI_OFF; // 고온경보
				else if(i==12) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[45]=='1') ? DI_ON : DI_OFF; // COMP1경보
				else if(i==13) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[46]=='1') ? DI_ON : DI_OFF; // COMP2경보
				else if(i==14) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[47]=='1') ? DI_ON : DI_OFF; // 난방히터경보
				else if(i==15) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[48]=='1') ? DI_ON : DI_OFF; // 가습히터경보
				else if(i==16) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[49]=='1') ? DI_ON : DI_OFF; // FAN경보
				else if(i==17) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[50]=='1') ? DI_ON : DI_OFF; // 누수경보
				else if(i==18) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[51]=='1') ? DI_ON : DI_OFF; // 온도센서경보
				else if(i==19) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[52]=='1') ? DI_ON : DI_OFF; // 습도센서경보
				else if(i==20) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[54]=='0') ? DI_ON : DI_OFF; // 단선
				else if(i==21) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[54]=='2') ? DI_ON : (p[54]=='1') ? DI_OFF : pDI->curr_val; // 가동/정지
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
			case RTU_AR_HANE09 :
			case RTU_AR_HANE09_Q :
				if     (i== 0) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[24] & 0x01) ? DI_ON : DI_OFF; // FAN
				else if(i== 1) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[24] & 0x02) ? DI_ON : DI_OFF; // 냉방1(CS1)
				else if(i== 2) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[24] & 0x04) ? DI_ON : DI_OFF; // 냉방2(CS2)
				else if(i== 3) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[24] & 0x08) ? DI_ON : DI_OFF; // 난방히터1
				else if(i== 4) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[24] & 0x10) ? DI_ON : DI_OFF; // 난방히터2
				else if(i== 5) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[24] & 0x20) ? DI_ON : DI_OFF; // 난방히터3
				else if(i== 6) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[24] & 0x40) ? DI_ON : DI_OFF; // 난방히터4
				else if(i== 7) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[24] & 0x80) ? DI_ON : DI_OFF; // 난방히터5
				else if(i== 8) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[25] & 0x01) ? DI_ON : DI_OFF; // 가습
				else if(i== 9) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[25] & 0x02) ? DI_ON : DI_OFF; // 급수
				else if(i==10) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[25] & 0x04) ? DI_ON : DI_OFF; // 배수
				else if(i==11) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[25] & 0x08) ? DI_ON : DI_OFF; // 고온경보
				else if(i==12) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[25] & 0x10) ? DI_ON : DI_OFF; // COMP1경보
				else if(i==13) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[25] & 0x20) ? DI_ON : DI_OFF; // COMP2경보
				else if(i==14) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[25] & 0x40) ? DI_ON : DI_OFF; // 난방히터경보
				else if(i==15) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[25] & 0x80) ? DI_ON : DI_OFF; // 가습히터경보
				else if(i==16) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[26] & 0x01) ? DI_ON : DI_OFF; // FAN경보
				else if(i==17) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[26] & 0x02) ? DI_ON : DI_OFF; // 누수경보
				else if(i==18) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[26] & 0x04) ? DI_ON : DI_OFF; // 온도센서경보
				else if(i==19) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[26] & 0x08) ? DI_ON : DI_OFF; // 습도센서경보
				else if(i==20) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[26] & 0x10) ? DI_ON : DI_OFF; // 단선
				else if(i==21) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[26] & 0x20) ? DI_OFF : DI_ON; // 0:가동/1:정지
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
				
			case SOLID :
				if     (i== 0) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[16] & 0x01) ? DI_ON : DI_OFF; // 펌프1이상
				else if(i== 1) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[16] & 0x02) ? DI_ON : DI_OFF; // 펌프2이상
				else if(i== 2) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[16] & 0x04) ? DI_ON : DI_OFF; // 압축기1이상
				else if(i== 3) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[16] & 0x08) ? DI_ON : DI_OFF; // 압축기2이상
				else if(i== 4) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[16] & 0x10) ? DI_ON : DI_OFF; // 실내기팬 이상
				else if(i== 5) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[16] & 0x20) ? DI_ON : DI_OFF; // 실외기팬 이상
				else if(i== 6) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[16] & 0x40) ? DI_ON : DI_OFF; // 필터 이상
				else if(i== 7) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[16] & 0x80) ? DI_ON : DI_OFF; // 가습기 이상
				else if(i== 8) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[15] & 0x01) ? DI_ON : DI_OFF; // 유체흐름1이상
				else if(i== 9) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[15] & 0x02) ? DI_ON : DI_OFF; // 유체흐름2이상
				else if(i==10) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[15] & 0x04) ? DI_ON : DI_OFF; // 누수 이상
				else if(i==11) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[15] & 0x08) ? DI_ON : DI_OFF; // 고온 경보
				else if(i==12) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[15] & 0x10) ? DI_ON : DI_OFF; // 저온 경보
				else if(i==13) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[15] & 0x20) ? DI_ON : DI_OFF; // 센서 이상
				else if(i==14) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[15] & 0x40) ? DI_ON : DI_OFF; // 하론 이상
				else if(i==15) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[18] & 0x01) ? DI_ON : DI_OFF; // 실내 온도 센서 이상
				else if(i==16) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[18] & 0x02) ? DI_ON : DI_OFF; // 실외 온도 센서 이상
				else if(i==17) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[18] & 0x04) ? DI_ON : DI_OFF; // 냉수 온도 센서 이상
				else if(i==18) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[18] & 0x08) ? DI_ON : DI_OFF; // 동파 온도 센서 이상
				else if(i==19) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[18] & 0x10) ? DI_ON : DI_OFF; // 응축 온도 센서 이상
				else if(i==20) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[18] & 0x20) ? DI_ON : DI_OFF; // 습도 센서 이상
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case STHAV_MB_R1 :
				if     (i== 0) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[17] & 0x01) ? DI_ON : DI_OFF; // 운전상태(팬)
				else if(i== 1) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[19] & 0x01) ? DI_ON : DI_OFF; // 운전상태(냉방)
				else if(i== 2) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[21] & 0x01) ? DI_ON : DI_OFF; // 운전상태(난방)
				else if(i== 3) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[23] & 0x01) ? DI_ON : DI_OFF; // 운전상태(가습)
				else if(i== 4) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[25] & 0x01) ? DI_ON : DI_OFF; // 운전상태(제습)
				else if(i== 5) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[27] & 0x01) ? DI_ON : DI_OFF; // 압축기1 이상
				else if(i== 6) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[29] & 0x01) ? DI_ON : DI_OFF; // 압축기2 이상
				else if(i== 7) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[31] & 0x01) ? DI_ON : DI_OFF; // 압축기3 이상
				else if(i== 8) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[33] & 0x01) ? DI_ON : DI_OFF; // 저압1 이상
				else if(i== 9) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[35] & 0x01) ? DI_ON : DI_OFF; // 저압2 이상
				else if(i==10) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[37] & 0x01) ? DI_ON : DI_OFF; // 저압3 이상
				else if(i==11) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[39] & 0x01) ? DI_ON : DI_OFF; // 습도과열기 이상
				else if(i==12) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[41] & 0x01) ? DI_ON : DI_OFF; // 난방/재열히터 이상
				else if(i==13) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[43] & 0x01) ? DI_ON : DI_OFF; // 공기물흐름 이상
				else if(i==14) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[45] & 0x01) ? DI_ON : DI_OFF; // 메인팬 이상
				else if(i==15) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[47] & 0x01) ? DI_ON : DI_OFF; // 누수감지
				else if(i==16) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[49] & 0x01) ? DI_ON : DI_OFF; // 고온 이상
				else if(i==17) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[51] & 0x01) ? DI_ON : DI_OFF; // 저온 이상
				else if(i==18) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[53] & 0x01) ? DI_ON : DI_OFF; // 고습 이상
				else if(i==19) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[55] & 0x01) ? DI_ON : DI_OFF; // 저습 이상
				else if(i==20) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[57] & 0x01) ? DI_ON : DI_OFF; // 하론 이상
				else if(i==21) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[59] & 0x01) ? DI_ON : DI_OFF; // 필터교체
				else if(i==22) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[61] & 0x01) ? DI_ON : DI_OFF; // 토탈 이상
				else if(i==23) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[63] & 0x01) ? DI_ON : DI_OFF; // 가습상태(급수장치)
				else if(i==24) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[65] & 0x01) ? DI_ON : DI_OFF; // 가습상태(배수장치)
				else if(i==25) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[67] & 0x01) ? DI_ON : DI_OFF; // 가습상태(가습실린더)
				else if(i==26) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[69] & 0x01) ? DI_ON : DI_OFF; // 가습상태(전류감지)
				else if(i==27) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[81] & 0x01) ? DI_ON : DI_OFF; // 운전상태(기동/정지)
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
				
			case AR_MODBUS :
				if     (i== 0) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[ 75], p[ 76]) & 0x02 ? DI_ON : DI_OFF; /* 팬상태 */
				else if(i== 1) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[ 75], p[ 76]) & 0x04 ? DI_ON : DI_OFF; /* 냉방상태 */
				else if(i== 2) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[ 75], p[ 76]) & 0x08 ? DI_ON : DI_OFF; /* 난방상태 */
				else if(i== 3) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[ 75], p[ 76]) & 0x10 ? DI_ON : DI_OFF; /* 가습상태 */
				else if(i== 4) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[ 75], p[ 76]) & 0x20 ? DI_ON : DI_OFF; /* 제습상태 */
				else if(i== 5) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[ 77], p[ 78]) & 0x0001 ? DI_ON : DI_OFF; /* 송풍기이상 */
				else if(i== 6) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[ 77], p[ 78]) & 0x0002 ? DI_ON : DI_OFF; /* 콤프1이상 */
				else if(i== 7) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[ 77], p[ 78]) & 0x0004 ? DI_ON : DI_OFF; /* 콤프2이상 */
				else if(i== 8) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[ 77], p[ 78]) & 0x0008 ? DI_ON : DI_OFF; /* 온도센서이상 */
				else if(i== 9) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[ 77], p[ 78]) & 0x0010 ? DI_ON : DI_OFF; /* 습도센서이상 */
				else if(i==10) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[ 77], p[ 78]) & 0x0020 ? DI_ON : DI_OFF; /* 가습기이상 */
				else if(i==11) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[ 77], p[ 78]) & 0x0040 ? DI_ON : DI_OFF; /* 누수이상 */
				else if(i==12) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[ 77], p[ 78]) & 0x0080 ? DI_ON : DI_OFF; /* 냉수이상 */
				else if(i==13) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[ 77], p[ 78]) & 0x0100 ? DI_ON : DI_OFF; /* 필터이상 */
				else if(i==14) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[ 77], p[ 78]) & 0x0200 ? DI_ON : DI_OFF; /* 고온이상 */
				else if(i==15) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[ 79], p[ 80]) & 0x0001 ? DI_ON : DI_OFF; /* 난방기이상 */
				else if(i==16) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[111], p[112]) & 0x02 ? DI_ON : DI_OFF; /* 솔레노이드1 */
				else if(i==17) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[111], p[112]) & 0x08 ? DI_ON : DI_OFF; /* 솔레노이드2 */
				else if(i==18) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[111], p[112]) & 0x20 ? DI_ON : DI_OFF; /* 히터1 */
				else if(i==19) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[111], p[112]) & 0x40 ? DI_ON : DI_OFF; /* 히터2 */
				else if(i==20) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[111], p[112]) & 0x80 ? DI_ON : DI_OFF; /* 히터3 */
				else if(i==21) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[113], p[114]) & 0x01 ? DI_ON : DI_OFF; /* 히터4 */
				else if(i==22) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[113], p[114]) & 0x02 ? DI_ON : DI_OFF; /* 히터5 */
				else if(i==23) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[113], p[114]) & 0x04 ? DI_ON : DI_OFF; /* 가습1 */
				else if(i==24) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[113], p[114]) & 0x08 ? DI_ON : DI_OFF; /* 가습2 */
				else if(i==25) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[ 75], p[ 76]) & 0x01 ? DI_ON : DI_OFF; /* 가동/정지 */
				else if(i==26) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[111], p[112]) & 0x01 ? DI_ON : DI_OFF; /* 송풍기 */
				else if(i==27) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[111], p[112]) & 0x04 ? DI_ON : DI_OFF; /* 콤프1 */
				else if(i==28) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[111], p[112]) & 0x10 ? DI_ON : DI_OFF; /* 콤프2 */
				else if(i==29) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[113], p[114]) & 0x10 ? DI_ON : DI_OFF; /* 급수 */
				else if(i==30) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[113], p[114]) & 0x20 ? DI_ON : DI_OFF; /* 배수 */
				else if(i==31) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[113], p[114]) & 0x40 ? DI_ON : DI_OFF; /* 알람 */
				else if(i==32) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[115], p[116]) & 0x20 ? DI_ON : DI_OFF; /* 누수 */
				else if(i==33) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[115], p[116]) & 0x40 ? DI_ON : DI_OFF; /* 수위 */
				else if(i==34) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[115], p[116]) & 0x80 ? DI_ON : DI_OFF; /* Water_Flow */
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
			case RTU_AR_MODBUS :
				if     (i== 0) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[54] & 0x01) ? DI_ON : DI_OFF; // 팬 상태      
				else if(i== 1) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[54] & 0x02) ? DI_ON : DI_OFF; // 냉방상태     
				else if(i== 2) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[54] & 0x04) ? DI_ON : DI_OFF; // 난방상태     
				else if(i== 3) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[54] & 0x08) ? DI_ON : DI_OFF; // 가습상태     
				else if(i== 4) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[54] & 0x10) ? DI_ON : DI_OFF; // 제습상태     
				else if(i== 5) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[54] & 0x20) ? DI_ON : DI_OFF; // 송풍기 경보   
				else if(i== 6) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[54] & 0x40) ? DI_ON : DI_OFF; // 콤프 1 경보  
				else if(i== 7) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[54] & 0x80) ? DI_ON : DI_OFF; // 콤프 2 경보
				
				else if(i== 8) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[55] & 0x01) ? DI_ON : DI_OFF; // 온도 센서 경보 
				else if(i== 9) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[55] & 0x02) ? DI_ON : DI_OFF; // 습도 센서 경보 
				else if(i==10) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[55] & 0x04) ? DI_ON : DI_OFF; // 가습기 경보   
				else if(i==11) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[55] & 0x08) ? DI_ON : DI_OFF; // 누수 경보    
				else if(i==12) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[55] & 0x10) ? DI_ON : DI_OFF; // 냉수 경보    
				else if(i==13) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[55] & 0x20) ? DI_ON : DI_OFF; // 필터 경보    
				else if(i==14) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[55] & 0x40) ? DI_ON : DI_OFF; // 고온 경보    
				else if(i==15) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[55] & 0x80) ? DI_ON : DI_OFF; // 난방기 경보   

				else if(i==16) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[56] & 0x01) ? DI_ON : DI_OFF; // 솔레노이드1 상태
				else if(i==17) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[56] & 0x02) ? DI_ON : DI_OFF; // 솔레노이드2 상태
				else if(i==18) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[56] & 0x04) ? DI_ON : DI_OFF; // 히터 1      
				else if(i==19) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[56] & 0x08) ? DI_ON : DI_OFF; // 히터 2      
				else if(i==20) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[56] & 0x10) ? DI_ON : DI_OFF; // 히터 3      
				else if(i==21) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[56] & 0x20) ? DI_ON : DI_OFF; // 히터 4      
				else if(i==22) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[56] & 0x40) ? DI_ON : DI_OFF; // 히터 5      
				else if(i==23) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[56] & 0x80) ? DI_ON : DI_OFF; // 가습 1      

				else if(i==24) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[57] & 0x01) ? DI_ON : DI_OFF; // 가습 2      
				else if(i==25) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[57] & 0x02) ? DI_ON : DI_OFF; // 가동/정지    
				else if(i==26) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[57] & 0x04) ? DI_ON : DI_OFF; // 송풍기		
				else if(i==27) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[57] & 0x08) ? DI_ON : DI_OFF; // 콤프1       		
				else if(i==28) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[57] & 0x10) ? DI_ON : DI_OFF; // 콤프2       
				else if(i==29) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[57] & 0x20) ? DI_ON : DI_OFF; // 급수        
				else if(i==30) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[57] & 0x40) ? DI_ON : DI_OFF; // 배수        
				else if(i==31) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[57] & 0x80) ? DI_ON : DI_OFF; // 알람        

				else if(i==32) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[58] & 0x01) ? DI_ON : DI_OFF; // 누수        
				else if(i==33) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[58] & 0x02) ? DI_ON : DI_OFF; // 수위        
				else if(i==34) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[58] & 0x04) ? DI_ON : DI_OFF; // Water Flow
				else if(i==35) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[58] & 0x08) ? DI_ON : DI_OFF; // RTU와 장비 통신불량
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case RTU_LIEBERT_CHTP :
				if     (i== 0) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[56] & 0x01) ? DI_ON : DI_OFF; // Cooling
				else if(i== 1) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[56] & 0x02) ? DI_ON : DI_OFF; // Heating    
				else if(i== 2) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[56] & 0x04) ? DI_ON : DI_OFF; // Humidification    
				else if(i== 3) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[56] & 0x08) ? DI_ON : DI_OFF; // De_Humidification
				else if(i== 4) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[56] & 0x10) ? DI_ON : DI_OFF; // Unit_Status
				else if(i== 5) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[56] & 0x20) ? DI_ON : DI_OFF; // Communication
				else if(i== 6) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[56] & 0x40) ? DI_ON : DI_OFF; // Local_Off
				else if(i== 7) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[56] & 0x80) ? DI_ON : DI_OFF; // Remote_Off
				
				else if(i== 8) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[57] & 0x01) ? DI_ON : DI_OFF; // Loss_of_Airflow 
				else if(i== 9) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[57] & 0x02) ? DI_ON : DI_OFF; // Change_Filters
				else if(i==10) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[57] & 0x04) ? DI_ON : DI_OFF; // High_Temperature
				else if(i==11) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[57] & 0x08) ? DI_ON : DI_OFF; // Low_Temperature
				else if(i==12) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[57] & 0x10) ? DI_ON : DI_OFF; // High_Humidity
				else if(i==13) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[57] & 0x20) ? DI_ON : DI_OFF; // Low_Humidity
				else if(i==14) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[57] & 0x40) ? DI_ON : DI_OFF; // Humidifier_Problem
				else if(i==15) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[57] & 0x80) ? DI_ON : DI_OFF; // No Water in Humidifier Fan

				else if(i==16) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[58] & 0x01) ? DI_ON : DI_OFF; // Main Fan OverLoad
				else if(i==17) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[58] & 0x02) ? DI_ON : DI_OFF; // Manual_Override
				else if(i==18) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[58] & 0x04) ? DI_ON : DI_OFF; // Smoke_Detected
				else if(i==19) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[58] & 0x08) ? DI_ON : DI_OFF; // Loss_of_water
				else if(i==20) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[58] & 0x10) ? DI_ON : DI_OFF; // Loss_of_Power
				else if(i==21) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[58] & 0x20) ? DI_ON : DI_OFF; // off_by_Remote_Shutdown
				else if(i==22) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[58] & 0x40) ? DI_ON : DI_OFF; // RTU와 장비 통신불량
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
				
			case OC485 :
				if     (i== 0) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[98] & 0x01) ? DI_ON : DI_OFF; // Fan
				else if(i== 1) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[98] & 0x02) ? DI_ON : DI_OFF; // Cool
				else if(i== 2) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[98] & 0x10) ? DI_ON : DI_OFF; // Electrical_Heater
				else if(i== 3) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[98] & 0x20) ? DI_ON : DI_OFF; // Humidification
				else if(i== 4) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[98] & 0x40) ? DI_ON : DI_OFF; // Dehumidification
				else if(i== 5) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[99] & 0x02) ? DI_ON : DI_OFF; // MAIN_FAN_OVERLOAD
				else if(i== 6) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[99] & 0x04) ? DI_ON : DI_OFF; // LOSS_of_AIRFLOW
				else if(i== 7) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[99] & 0x08) ? DI_ON : DI_OFF; // LOSS_of_FLOW				
				else if(i== 8) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[100] & 0x40) ? DI_ON : DI_OFF; // SMOKE_DETECTED
				else if(i== 9) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[100] & 0x80) ? DI_ON : DI_OFF; // WATER_UNDER_FLOOR
				else if(i==10) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[101] & 0x01) ? DI_ON : DI_OFF; // HUMIDIFIER_PROBLEM
				else if(i==11) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[101] & 0x04) ? DI_ON : DI_OFF; // STANDBY_UNIT_ON
				else if(i==12) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[102] & 0x08) ? DI_ON : DI_OFF; // HIGH_TEMPERATURE
				else if(i==13) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[102] & 0x10) ? DI_ON : DI_OFF; // LOSS_OF_POWER
				else if(i==14) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[103] & 0x04) ? DI_ON : DI_OFF; // HIGH_ROOM_TEMP
				else if(i==15) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[103] & 0x08) ? DI_ON : DI_OFF; // LOW_ROOM_TEMP
				else if(i==16) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[103] & 0x10) ? DI_ON : DI_OFF; // HIGH_ROOM_HUM
				else if(i==17) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[103] & 0x20) ? DI_ON : DI_OFF; // LOW_ROOM_HUM
				else if(i==18) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[104] & 0x08) ? DI_ON : DI_OFF; // GLOGGED_FILTERS
				else if(i==19) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[106] & 0x04) ? DI_ON : DI_OFF; // NETWORK FAILURE
				else if(i==20) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[106] & 0x08) ? DI_ON : DI_OFF; // DSCROLL 1 SENSOR FAIL
				else if(i==21) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[106] & 0x10) ? DI_ON : DI_OFF; // DSCROLL 2 SENSOR FAIL
				else if(i==22) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[32], p[33]) ? DI_ON : DI_OFF; // Summary_Alarm
				else if(i==23) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[46], p[47]) ? DI_ON : DI_OFF; // FreeCooling_Status
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case RTU_OC485 :
				if     (i== 0) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[168] & 0x01) ? DI_ON : DI_OFF; // 팬기동(기동상태)
				else if(i== 1) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[168] & 0x02) ? DI_ON : DI_OFF; // 냉방기동
				else if(i== 2) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[168] & 0x10) ? DI_ON : DI_OFF; // 전기히터기동
				else if(i== 3) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[168] & 0x20) ? DI_ON : DI_OFF; // 가습 기동
				else if(i== 4) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[168] & 0x40) ? DI_ON : DI_OFF; // 제습 기동
				else if(i== 5) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[168] & 0x80) ? DI_ON : DI_OFF; // 송풍기과전류	
				
				else if(i== 6) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[169] & 0x01) ? DI_ON : DI_OFF; // 공기흐름없음
				else if(i== 7) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[169] & 0x02) ? DI_ON : DI_OFF; // 유량없음	
				
				else if(i== 8) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[170] & 0x10) ? DI_ON : DI_OFF; // 연기감지
				else if(i== 9) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[170] & 0x20) ? DI_ON : DI_OFF; // 누수감지
				else if(i==10) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[170] & 0x40) ? DI_ON : DI_OFF; // 가습기 문제	
				
				else if(i==11) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[171] & 0x01) ? DI_ON : DI_OFF; // STANDBY UNIT ON
				else if(i==12) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[171] & 0x80) ? DI_ON : DI_OFF; // 고온	
				
				else if(i==13) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[172] & 0x01) ? DI_ON : DI_OFF; // 전원공급차단
				else if(i==14) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[172] & 0x04) ? DI_ON : DI_OFF; // 고온알람
				else if(i==15) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[172] & 0x08) ? DI_ON : DI_OFF; // 저온알람
				else if(i==16) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[172] & 0x10) ? DI_ON : DI_OFF; // 고습알람
				else if(i==17) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[172] & 0x20) ? DI_ON : DI_OFF; // 저습알람		
				
				else if(i==18) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[173] & 0x08) ? DI_ON : DI_OFF; // 필터 막힘
				
				else if(i==19) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[175] & 0x02) ? DI_ON : DI_OFF; // 네트워크 연결 실패
				else if(i==20) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[175] & 0x04) ? DI_ON : DI_OFF; // DSCROLL1_SENSOR_FAIL
				else if(i==21) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[175] & 0x08) ? DI_ON : DI_OFF; // DSCROLL2_SENSOR_FAIL
				else if(i==22) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[175] & 0x10) ? DI_ON : DI_OFF; // SUMMARY_ALARM
				else if(i==23) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[175] & 0x20) ? DI_ON : DI_OFF; // FreeCooling_status
				else if(i==24) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[175] & 0x40) ? DI_ON : DI_OFF; // RTU-장비 통신불량

				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
			//항온항습기 끝

			//STS 시작
			case RTU_SUPER_SWITCH3 :
				if     (i== 0) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[248] & 0x01) ? DI_ON : DI_OFF; // 캐비넷온도이상
				else if(i== 1) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[248] & 0x02) ? DI_ON : DI_OFF; // 히트싱크온도이상
				else if(i== 2) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[248] & 0x04) ? DI_ON : DI_OFF; // 0=open, 1=closed : 입력서킷브레이크상태
				else if(i== 3) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[248] & 0x08) ? DI_ON : DI_OFF; // 입력서킷브레이크이상
				else if(i== 4) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[248] & 0x10) ? DI_ON : DI_OFF; // LoadA서킷브레이크상태
				else if(i== 5) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[248] & 0x20) ? DI_ON : DI_OFF; // LoadA서킷브레이크이상
				else if(i== 6) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[248] & 0x40) ? DI_ON : DI_OFF; // LoadB서킷브레이크상태
				else if(i== 7) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[248] & 0x80) ? DI_ON : DI_OFF; // LoadB서킷브레이크이상
				else if(i== 8) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[249] & 0x01) ? DI_ON : DI_OFF; // 바이패스서킷브레이크상태
				else if(i== 9) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[249] & 0x02) ? DI_ON : DI_OFF; // 바이패스서킷브레이크이상
				else if(i==10) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[249] & 0x04) ? DI_ON : DI_OFF; // 0=red, 1=green : 출력상태
				else if(i==11) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[249] & 0x08) ? DI_ON : DI_OFF; // 0=out of sync, 1=in sync : Source_Sync
				else if(i==12) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[249] & 0x10) ? DI_ON : DI_OFF; // Door_State
				else if(i==13) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[249] & 0x20) ? DI_ON : DI_OFF; // Summary_Alarm
				else if(i==14) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[249] & 0x40) ? DI_ON : DI_OFF; // Unacknowledged_Events
				else if(i==15) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[249] & 0x80) ? DI_ON : DI_OFF; // Fan3_Fail
				else if(i==16) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[250] & 0x01) ? DI_ON : DI_OFF; // Fan4_Fail
				else if(i==17) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[250] & 0x02) ? DI_ON : DI_OFF; // SCR_ON
				else if(i==18) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[250] & 0x04) ? DI_ON : DI_OFF; // Source_Available

				else if(i==19) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[250] & 0x08) ? DI_ON : DI_OFF; // 캐비넷온도이상
				else if(i==20) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[250] & 0x10) ? DI_ON : DI_OFF; // 히트싱크온도이상
				else if(i==21) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[250] & 0x20) ? DI_ON : DI_OFF; // 0=open, 1=closed : 입력서킷브레이크상태
				else if(i==22) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[250] & 0x40) ? DI_ON : DI_OFF; // 입력서킷브레이크이상
				else if(i==23) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[250] & 0x80) ? DI_ON : DI_OFF; // LoadA서킷브레이크상태
				else if(i==24) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[251] & 0x01) ? DI_ON : DI_OFF; // LoadA서킷브레이크이상
				else if(i==25) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[251] & 0x02) ? DI_ON : DI_OFF; // LoadB서킷브레이크상태
				else if(i==26) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[251] & 0x04) ? DI_ON : DI_OFF; // LoadB서킷브레이크이상
				else if(i==27) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[251] & 0x08) ? DI_ON : DI_OFF; // 바이패스서킷브레이크상태
				else if(i==28) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[251] & 0x10) ? DI_ON : DI_OFF; // 바이패스서킷브레이크이상
				else if(i==29) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[251] & 0x20) ? DI_ON : DI_OFF; // 0=red, 1=green : 출력상태
				else if(i==30) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[251] & 0x40) ? DI_ON : DI_OFF; // 0=out of sync, 1=in sync : Source_Sync
				else if(i==31) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[251] & 0x80) ? DI_ON : DI_OFF; // Door_State
				else if(i==32) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[252] & 0x01) ? DI_ON : DI_OFF; // Summary_Alarm
				else if(i==33) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[252] & 0x02) ? DI_ON : DI_OFF; // Unacknowledged_Events
				else if(i==34) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[252] & 0x04) ? DI_ON : DI_OFF; // Fan3_Fail
				else if(i==35) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[252] & 0x08) ? DI_ON : DI_OFF; // Fan4_Fail
				else if(i==36) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[252] & 0x10) ? DI_ON : DI_OFF; // SCR_ON
				else if(i==37) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[252] & 0x20) ? DI_ON : DI_OFF; // Source_Available
				else if(i==38) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[252] & 0x20) ? DI_ON : DI_OFF; // RTU-장비 통신불량
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case SUPER_SWITCH3 :
				if     (i== 0) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[188], p[189]) == 1 ? DI_ON : DI_OFF; // 캐비넷온도이상
				else if(i== 1) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[190], p[191]) == 1 ? DI_ON : DI_OFF; // 히트싱크온도이상
				else if(i== 2) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[192], p[193]) == 0 ? DI_ON : get_short_value(p[192], p[193]) == 1 ? DI_OFF: DI_OFF;   //0=open, 1=closed : 입력서킷브레이크상태
				else if(i== 3) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[192], p[193]) == 2 ? DI_ON : DI_OFF;   // 입력서킷브레이크이상
				else if(i== 4) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[194], p[195]) == 0 ? DI_ON : get_short_value(p[194], p[195]) == 1 ? DI_OFF : DI_OFF;  // LoadA서킷브레이크상태
				else if(i== 5) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[194], p[195]) == 2 ? DI_ON : DI_OFF;   // LoadA서킷브레이크이상
				else if(i== 6) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[196], p[197]) == 0 ? DI_ON : get_short_value(p[196], p[197]) == 1 ? DI_OFF : DI_OFF;  // LoadB서킷브레이크상태
				else if(i== 7) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[196], p[197]) == 2 ? DI_ON : DI_OFF;   // LoadB서킷브레이크이상
				else if(i== 8) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[198], p[199]) == 0 ? DI_ON : get_short_value(p[198], p[199]) == 1 ? DI_OFF : DI_OFF;  // 바이패스서킷브레이크상태
				else if(i== 9) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[198], p[199]) == 2 ? DI_ON : DI_OFF;   // 바이패스서킷브레이크이상
				else if(i==10) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[200], p[201]) == 1 ? DI_ON : DI_OFF;   // 0=red, 1=green : 출력상태
				else if(i==11) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[202], p[203]) == 1 ? DI_ON : DI_OFF;   // 0=out of sync, 1=in sync : Source_Sync
				else if(i==12) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[204], p[205]) ? DI_ON : DI_OFF;        // Door_State
				else if(i==13) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[206], p[207]) & 0x01 ? DI_ON : DI_OFF; // Summary_Alarm
				else if(i==14) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[206], p[207]) & 0x04 ? DI_ON : DI_OFF; // Unacknowledged_Events
				else if(i==15) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[208], p[209]) & 0x01 ? DI_ON : DI_OFF; // Fan3_Fail
				else if(i==16) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[208], p[209]) & 0x02 ? DI_ON : DI_OFF; // Fan4_Fail
				else if(i==17) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[210], p[211]) !=0x00 ? DI_ON : DI_OFF; // SCR_ON
				else if(i==18) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[212], p[213]) ==0x02 ? DI_ON : DI_OFF; // Source_Available

				else if(i==19) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[188+26], p[189+26]) == 1 ? DI_ON : DI_OFF; // 캐비넷온도이상
				else if(i==20) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[190+26], p[191+26]) == 1 ? DI_ON : DI_OFF; // 히트싱크온도이상
				else if(i==21) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[192+26], p[193+26]) == 0 ? DI_ON : get_short_value(p[192+26], p[193+26]) == 1 ? DI_OFF: DI_OFF;   // 0=open, 1=closed : 입력서킷브레이크상태
				else if(i==22) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[192+26], p[193+26]) == 2 ? DI_ON : DI_OFF;   // 입력서킷브레이크이상
				else if(i==23) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[194+26], p[195+26]) == 0 ? DI_ON : get_short_value(p[194+26], p[195+26]) == 1 ? DI_OFF : DI_OFF;  // LoadA서킷브레이크상태
				else if(i==24) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[194+26], p[195+26]) == 2 ? DI_ON : DI_OFF;   // LoadA서킷브레이크이상
				else if(i==25) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[196+26], p[197+26]) == 0 ? DI_ON : get_short_value(p[196+26], p[197+26]) == 1 ? DI_OFF : DI_OFF;  // LoadB서킷브레이크상태
				else if(i==26) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[196+26], p[197+26]) == 2 ? DI_ON : DI_OFF;   // LoadB서킷브레이크이상
				else if(i==27) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[198+26], p[199+26]) == 0 ? DI_ON : get_short_value(p[198+26], p[199+26]) == 1 ? DI_OFF : DI_OFF;  // 바이패스서킷브레이크상태
				else if(i==28) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[198+26], p[199+26]) == 2 ? DI_ON : DI_OFF;   // 바이패스서킷브레이크이상
				else if(i==29) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[200+26], p[201+26]) == 1 ? DI_ON : DI_OFF;   // 0=red, 1=green : 출력상태
				else if(i==30) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[202+26], p[203+26]) == 1 ? DI_ON : DI_OFF;   // 0=out of sync, 1=in sync : Source_Sync
				else if(i==31) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[204+26], p[205+26]) ? DI_ON : DI_OFF;        // Door_State
				else if(i==32) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[206+26], p[207+26]) & 0x01 ? DI_ON : DI_OFF; // Summary_Alarm
				else if(i==33) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[206+26], p[207+26]) & 0x04 ? DI_ON : DI_OFF; // Unacknowledged_Events
				else if(i==34) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[208+26], p[209+26]) & 0x01 ? DI_ON : DI_OFF; // Fan3_Fail
				else if(i==35) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[208+26], p[209+26]) & 0x02 ? DI_ON : DI_OFF; // Fan4_Fail
				else if(i==36) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[210+26], p[211+26]) !=0x00 ? DI_ON : DI_OFF; // SCR_ON
				else if(i==37) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : get_short_value(p[212+26], p[213+26]) ==0x02 ? DI_ON : DI_OFF; // Source_Available
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
			//STS 끝

			//접점 시작
			case KM6053 :
				if	   (i== 0) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 4] & 0x01) ? DI_OFF : DI_ON; // DI00
				else if(i== 1) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 4] & 0x02) ? DI_OFF : DI_ON; // DI01
				else if(i== 2) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 4] & 0x04) ? DI_OFF : DI_ON; // DI02
				else if(i== 3) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 4] & 0x08) ? DI_OFF : DI_ON; // DI03
				else if(i== 4) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 4] & 0x10) ? DI_OFF : DI_ON; // DI04
				else if(i== 5) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 4] & 0x20) ? DI_OFF : DI_ON; // DI05
				else if(i== 6) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 4] & 0x40) ? DI_OFF : DI_ON; // DI06
				else if(i== 7) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 4] & 0x80) ? DI_OFF : DI_ON; // DI07
				else if(i== 8) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 3] & 0x01) ? DI_OFF : DI_ON; // DI08
				else if(i== 9) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 3] & 0x02) ? DI_OFF : DI_ON; // DI09
				else if(i==10) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 3] & 0x04) ? DI_OFF : DI_ON; // DI10
				else if(i==11) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 3] & 0x08) ? DI_OFF : DI_ON; // DI11
				else if(i==12) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 3] & 0x10) ? DI_OFF : DI_ON; // DI12
				else if(i==13) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 3] & 0x20) ? DI_OFF : DI_ON; // DI13
				else if(i==14) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 3] & 0x40) ? DI_OFF : DI_ON; // DI14
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
			case RM6054 :
				if	   (i== 0) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 4] & 0x01) ? DI_OFF : DI_ON; // DI00
				else if(i== 1) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 4] & 0x02) ? DI_OFF : DI_ON; // DI01
				else if(i== 2) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 4] & 0x04) ? DI_OFF : DI_ON; // DI02
				else if(i== 3) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 4] & 0x08) ? DI_OFF : DI_ON; // DI03
				else if(i== 4) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 4] & 0x10) ? DI_OFF : DI_ON; // DI04
				else if(i== 5) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 4] & 0x20) ? DI_OFF : DI_ON; // DI05
				else if(i== 6) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 4] & 0x40) ? DI_OFF : DI_ON; // DI06
				else if(i== 7) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 4] & 0x80) ? DI_OFF : DI_ON; // DI07
				else if(i== 8) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 3] & 0x01) ? DI_OFF : DI_ON; // DI08
				else if(i== 9) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 3] & 0x02) ? DI_OFF : DI_ON; // DI09
				else if(i==10) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 3] & 0x04) ? DI_OFF : DI_ON; // DI10
				else if(i==11) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 3] & 0x08) ? DI_OFF : DI_ON; // DI11
				else if(i==12) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 3] & 0x10) ? DI_OFF : DI_ON; // DI12
				else if(i==13) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 3] & 0x20) ? DI_OFF : DI_ON; // DI13
				else if(i==14) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 3] & 0x40) ? DI_OFF : DI_ON; // DI14
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
				
			case RTU_RTU5000 :
				if	   (i== 0) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 1] & 0x01) ? DI_ON : DI_OFF; // DI00
				else if(i== 1) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 1] & 0x02) ? DI_ON : DI_OFF; // DI01
				else if(i== 2) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 1] & 0x04) ? DI_ON : DI_OFF; // DI02
				else if(i== 3) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 1] & 0x08) ? DI_ON : DI_OFF; // DI03
				else if(i== 4) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 1] & 0x10) ? DI_ON : DI_OFF; // DI04
				else if(i== 5) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 1] & 0x20) ? DI_ON : DI_OFF; // DI05
				else if(i== 6) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 1] & 0x40) ? DI_ON : DI_OFF; // DI06
				else if(i== 7) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 1] & 0x80) ? DI_ON : DI_OFF; // DI07
				else if(i== 8) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 0] & 0x01) ? DI_ON : DI_OFF; // DI08
				else if(i== 9) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 0] & 0x02) ? DI_ON : DI_OFF; // DI09
				else if(i==10) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 0] & 0x04) ? DI_ON : DI_OFF; // DI10
				else if(i==11) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 0] & 0x08) ? DI_ON : DI_OFF; // DI11
				else if(i==12) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 0] & 0x10) ? DI_ON : DI_OFF; // DI12
				else if(i==13) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 0] & 0x20) ? DI_ON : DI_OFF; // DI13
				else if(i==14) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 0] & 0x40) ? DI_ON : DI_OFF; // DI14
				else if(i==15) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[ 0] & 0x80) ? DI_ON : DI_OFF; // DI15
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;
			//분전반 시작
			
			case RTU_DPM_GiMACi :
				if(i==0) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[44] & 0x01) ? DI_ON : DI_OFF; // RTU-장비 통신불량
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case RTU_DPM_GiMACIIPlus :
				if(i==0) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[68] & 0x01) ? DI_ON : DI_OFF; // RTU-장비 통신불량
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			case RTU_DPM_GiPAM115FI :
				if(i==0) pDI->curr_val = com_state==COM_ABNORMAL ? pDI->curr_val : (p[42] & 0x01) ? DI_ON : DI_OFF; // RTU-장비 통신불량
				else { pthread_mutex_unlock(pStatus->mux); continue; } break;

			default :
				pthread_mutex_unlock(pStatus->mux);
				continue;
			
		}
#ifdef DEBUG
        fileLog(WARNING, "[%s:%d] DI eseq=[%d] tseq=[%d] curr_val=[%d]\n", __FUNCTION__, __LINE__, pConnInfo->eseq, pDI->tseq, pDI->curr_val);
#endif
		

		if(pDI->use_yn == YES && com_state == COM_NORMAL)
		{
			if(pDI->alarm_yn == YES)
			{
				if(pDI->tm_alarm==0) pDI->tm_alarm = time(NULL);

				if(pDI->curr_val == pDI->alarm_val && pDI->critical_alarm_yn == YES)
					pDI->curr_status = CRITICAL;
				else
				{
					pDI->curr_status = NORMAL;
					pDI->tm_alarm = time(NULL);
				}
				
				// 알람 일때
				if(pDI->send_flag==0 && pDI->curr_status != NORMAL )
				{
					di_diff = difftime(now, pDI->tm_alarm);
					if(di_diff >= pDI->alarm_term)
					{
						pDI->send_flag = 1;
						alarm_insert(OCCURE, index, DI, i, pDI->curr_status);
						pDI->last_status = pDI->curr_status;
					}
				}
				// 알람이 복구 될때
				else if(pDI->send_flag == 1 && pDI->curr_status == NORMAL)
				{
					pDI->send_flag = 0;
					alarm_insert(CLEAR, index, DI, i, pDI->last_status);
				}
			}
		}

		if(pDI->use_yn == YES)
		{
			//데이터 로그 인서트
			di_diff = difftime(now, pDI->tm_save);
			if(pDI->save_yn==YES && (di_diff >= pDI->save_term || pDI->tm_save == 0))
			{
				mysqlinsertData(pConnInfo->eseq, pDI->tseq, pDI->curr_val);
				pDI->tm_save = time(NULL);
			}
			
			//데이터 로그 업데이트
			update_diff = difftime(now, pDI->tm_update);
			if(update_diff >= pDI->update_term || pDI->tm_update == 0)
			{
				mysqlUpdateTag(pConnInfo->eseq, pDI->tseq, pDI->curr_val);
				pDI->tm_update = time(NULL);
			}
		}


		pthread_mutex_unlock(pStatus->mux);
	}


}