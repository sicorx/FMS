#include "include.h"

extern struct equip_conn_info *conn_info[];
extern struct equip_status *gp_status[];

int make_dyrcu_com_frame_head(unsigned char *buff, int cpu_addr, int func, int ext_addr, int data_size)
{
    int wbyte=0;

    buff[wbyte++] = cpu_addr;
    buff[wbyte++] = func;
    buff[wbyte++] = (((ext_addr-1)*0x64) & 0xFF00) >> 8;
    buff[wbyte++] =  ((ext_addr-1)*0x64) & 0x00FF;
    buff[wbyte++] = (data_size & 0xFF00) >> 8;
    buff[wbyte++] =  data_size & 0x00FF;

    return wbyte;
}

int communicate_mst(int fd, unsigned char *buff, int index)
{
    int wbytes = 0;
    struct equip_conn_info *pConnInfo = conn_info[index];
    usleep(100000);

    wbytes = make_send_frame(buff, index, MST_REQ_ALL_DATA, 0);
    if(sendto_module(fd, buff, wbytes) != 0)		{ fileLog(WARNING, "SERVER -> MST eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }
    if(readfrom_module(fd, buff, index, 0) != 0)	{ fileLog(WARNING, "SERVER <- MST eseq=[%d] Packet Recv Fail\n", pConnInfo->eseq); return -1; }

    return 0;
}


int communicate_nemo(int fd, unsigned char *buff, int index)
{
    int wbytes = 0;
    struct equip_conn_info *pConnInfo = conn_info[index];
    usleep(100000);

    wbytes = make_send_frame(buff, index, 0, 0);
    if(sendto_module(fd, buff, wbytes) != 0)		{ fileLog(WARNING, "SERVER -> NEMO eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }
    if(readfrom_module(fd, buff, index, 0) != 0)	{ fileLog(WARNING, "SERVER <- NEMO eseq=[%d] Packet Recv Fail\n", pConnInfo->eseq); return -1; }

    return 0;
}

int communicate_ar(int fd, unsigned char *buff, int index) /* AR 항온항습기 시작(a Q 0 타입) */
{
    int wbytes = 0;
    struct equip_conn_info *pConnInfo = conn_info[index];
    usleep(100000);

    wbytes = make_send_frame(buff, index, AR_REQ_CMD, '0');
    if(sendto_module(fd, buff, wbytes) != 0)		{ fileLog(WARNING, "SERVER -> AR eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }
    if(readfrom_module(fd, buff, index, 0) != 0)	{ fileLog(WARNING, "SERVER <- AR eseq=[%d] Packet Recv Fail\n", pConnInfo->eseq); return -1; }

    return 0;
}

int communicate_ar_dyrcu(int fd, unsigned char *buff, int index)/* AR 항온항습기 시작 (a ? 0 타입) */
{
    int wbytes = 0;
    struct equip_conn_info *pConnInfo = conn_info[index];
    unsigned char rxtmp[128];
    usleep(100000);

    wbytes = make_send_frame(rxtmp, index, SYSTRONIC_P_REQ_CMD, 0);
    if(sendto_module(fd, rxtmp, wbytes) != 0)						{ fileLog(WARNING, "SERVER -> AR_DYRCU eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

    memset(rxtmp, 0x00, sizeof(rxtmp));
    if(readfrom_module(fd, rxtmp, index, SYSTRONIC_P_REQ_CMD) != 0) { fileLog(WARNING, "SERVER <- AR_DYRCU eseq=[%d] Packet Recv Fail\n", pConnInfo->eseq); return -1; }

    memcpy(buff, rxtmp+6, 56);

    return 0;
}


int communicate_ar_old(int fd, unsigned char *buff, int index)/* AR 항온항습기 시작 (a ? 0 타입) */
{
    int wbytes = 0;
    struct equip_conn_info *pConnInfo = conn_info[index];
    usleep(100000);

    wbytes = make_send_frame(buff, index, SYSTRONIC_P_REQ_CMD, '0');
    if(sendto_module(fd, buff, wbytes) != 0)		{ fileLog(WARNING, "SERVER -> AR_OLD eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }
    if(readfrom_module(fd, buff, index, 0) != 0)	{ fileLog(WARNING, "SERVER <- AR_OLD eseq=[%d] Packet Recv Fail\n", pConnInfo->eseq); return -1; }

    return 0;
}

int communicate_ar_question(int fd, unsigned char *buff, int index)/* AR 항온항습기 시작 (a ? ? 타입) */
{
    int wbytes = 0;
    struct equip_conn_info *pConnInfo = conn_info[index];
    usleep(100000);

    wbytes = make_send_frame(buff, index, SYSTRONIC_P_REQ_CMD, '?');
    if(sendto_module(fd, buff, wbytes) != 0)		{ fileLog(WARNING, "SERVER -> AR_QUESTION eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }
    if(readfrom_module(fd, buff, index, 0) != 0)	{ fileLog(WARNING, "SERVER <- AR_QUESTION eseq=[%d] Packet Recv Fail\n", pConnInfo->eseq); return -1; }

    return 0;
}

int communicate_ar_modbus(int fd, unsigned char *buff, int index)
{
    int wbytes = 0;
    struct equip_conn_info *pConnInfo = conn_info[index];
    usleep(100000);

    wbytes = make_modbus_frame(index, buff, pConnInfo->id, AR_MODBUS_REQ_CMD, AR_MODBUS_START_ADDR, AR_MODBUS_WORD_CNT, NULL);
    if(sendto_module(fd, buff, wbytes) != 0)									{ fileLog(WARNING, "SERVER -> AR_MODBUS eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }
    if(read_modbus_frame_timeout(index, fd, buff, 2, AR_MODBUS_WORD_CNT) != 0)	{ return -1; }

    return 0;
}

int communicate_rtu_ar_modbus(int fd, unsigned char *buff, int index)
{
    int i=0, prev_idx=0;

    int wbytes=0;
    unsigned char rxtmp[128];
    int regAddr[]={0, 0},
        cmdCode[]={0x03, 0x02},
        wordCnt[]={27, 36};
    struct equip_conn_info *pConnInfo = conn_info[index];

    usleep(100000);

    for(i=0; i<(int)sizeof(regAddr)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmdCode[i], regAddr[i], wordCnt[i], NULL);

        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(WARNING, "SERVER -> RTU_AR_MODBUS eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));

        if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) != 0)	{ return -1; }
		
		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
    }

    return 0;
}

int communicate_ar_modbus_dyrcu(int fd, unsigned char *buff, int index)
{
    int wbytes = 0;
    char rxtmp[300];
    unsigned short crc16 = 0;
    struct equip_conn_info *pConnInfo = conn_info[index];

    usleep(100000);

    memset(rxtmp, 0x00, sizeof(rxtmp));
    wbytes  = make_dyrcu_com_frame_head((unsigned char *)rxtmp, 0x01, 0x0A, pConnInfo->ext_addr, 8);
    wbytes += make_modbus_frame(index, (unsigned char *)rxtmp+wbytes, pConnInfo->id, AR_MODBUS_REQ_CMD, AR_MODBUS_START_ADDR, AR_MODBUS_WORD_CNT, NULL);
    crc16 = modbus_crc16((unsigned char *)rxtmp, wbytes);
    buff[wbytes++] = (crc16 & 0xFF00) >> 8;
    buff[wbytes++] =  crc16 & 0x00FF;

    if(sendto_module(fd, buff, wbytes) != 0)								{ fileLog(WARNING, "SERVER -> AR_MODBUS_DYRCU eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }
    if(read_dyrcu_com_frame_timeout(fd, (unsigned char *)rxtmp, 2) != 0)	{ fileLog(WARNING, "SERVER <- AR_MODBUS_DYRCU eseq=[%d] Packet Recv Fail\n", pConnInfo->eseq); return -1; }

    crc16 = modbus_crc16((unsigned char *)rxtmp+6, rxtmp[8]+3);

    if(rxtmp[rxtmp[8]+ 9] != ((crc16 & 0xFF00) >> 8) || rxtmp[rxtmp[8]+10] != (crc16 & 0x00FF))
    {
        fileLog(WARNING, "MODBUS CRC Check Fail.. Compute CRC[%04X] Recv CRC[%02X][%02X]\n", crc16, rxtmp[rxtmp[8]+9], rxtmp[rxtmp[8]+10]);
        return -1;
    }

    memcpy(buff, rxtmp+6, (rxtmp[4]<<8)|rxtmp[5]);

    return 0;
}
int communicate_svu(int fd, unsigned char *buff, int index)
{
    struct equip_conn_info *pConnInfo = conn_info[index];
    int wbytes = 0;
    int i;
    int requset_SVU_alldata[] = {
        SVU_REQ_TEMP_DB, SVU_REQ_TEMP_PB, SVU_REQ_SHUMI, SVU_REQ_HUMI_DB, SVU_REQ_HUMI_PB,
        SVU_REQ_MODE, SVU_REQ_STATE, SVU_REQ_STEMP, SVU_REQ_CONMODE, SVU_REQ_RHUMI,
        SVU_REQ_RTEMP, SVU_REQ_OUTBUF0, SVU_REQ_OUTBUF1, SVU_REQ_ERR00, SVU_REQ_ERR01};
    unsigned char rxtmp[512];

    usleep(100000);

    for(i=0; i<15; i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_send_frame(rxtmp, index, requset_SVU_alldata[i], 0);
        if(sendto_module(fd, rxtmp, wbytes) != 0)		{ fileLog(WARNING, "SERVER -> SVU eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));
        if(readfrom_module(fd, rxtmp, index, 0) != 0)	{ fileLog(WARNING, "SERVER <- SVU eseq=[%d] Packet Recv Fail\n", pConnInfo->eseq); return -1; }
        memcpy(buff+(i*2), rxtmp, 2);
    }

    return 0;
}

int communicate_ddc_shinsung(int fd, unsigned char *buff, int index)
{
    int wbytes = 0;
    struct equip_conn_info *pConnInfo = conn_info[index];
    usleep(100000);

    wbytes = make_send_frame(buff, index, DDC_REQ_DATA_TYPE, 0);
    if(sendto_module(fd, buff, wbytes) != 0)						{ fileLog(WARNING, "SERVER -> DDC_SHINSUNG eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }
    if(readfrom_module(fd, buff, index, DDC_REQ_DATA_TYPE) != 0)	{ fileLog(WARNING, "SERVER <- DDC_SHINSUNG eseq=[%d] Packet Recv Fail\n", pConnInfo->eseq); return -1; }

    return 0;
}

int communicate_ddc_hangul(int fd, unsigned char *buff, int index)
{
    int wbytes = 0;
    struct equip_conn_info *pConnInfo = conn_info[index];
    usleep(100000);

    wbytes = make_send_frame(buff, index, DDC_REQ_DATA_TYPE, 0);
    if(sendto_module(fd, buff, wbytes) != 0)						{ fileLog(WARNING, "SERVER -> DDC_HANGUL eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }
    if(readfrom_module(fd, buff, index, DDC_REQ_DATA_TYPE) != 0)	{ fileLog(WARNING, "SERVER <- DDC_HANGUL eseq=[%d] Packet Recv Fail\n", pConnInfo->eseq); return -1; }

    return 0;
}

int communicate_ddc_hangul2(int fd, unsigned char *buff, int index)
{
    int wbytes = 0;
    struct equip_conn_info *pConnInfo = conn_info[index];
    unsigned char rxtmp[512];
    usleep(100000);

    memset(rxtmp, 0, sizeof(rxtmp));
    wbytes = make_send_frame(rxtmp, index, DDC_REQ_DATA_TYPE, 0);
    if(sendto_module(fd, rxtmp, wbytes) != 0)						{ fileLog(WARNING, "SERVER -> DDC_HANGUL2 eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

    memset(rxtmp, 0, sizeof(rxtmp));
    if(readfrom_module(fd, rxtmp, index, DDC_REQ_DATA_TYPE) != 0)	{ fileLog(WARNING, "SERVER <- DDC_HANGUL2 eseq=[%d] Packet Recv Fail\n", pConnInfo->eseq); return -1; }
    memcpy(buff, rxtmp+15, DDC_HANGUL_DATA_RES_SIZE-16);

    return 0;
}

int communicate_fac1000(int fd, unsigned char *buff, int index)
{
    int wbytes = 0;
    struct equip_conn_info *pConnInfo = conn_info[index];
    usleep(100000);

    wbytes = make_send_frame(buff, index, FAC1000_REQ_CMD, 0);
    if(sendto_module(fd, buff, wbytes) != 0)					{ fileLog(WARNING, "SERVER -> FAC1000 eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }
    if(readfrom_module(fd, buff, index, FAC1000_REQ_CMD) != 0)	{ fileLog(WARNING, "SERVER <- FAC1000 eseq=[%d] Packet Recv Fail\n", pConnInfo->eseq); return -1; }

    return 0;
}

int communicate_seungil(int fd, unsigned char *buff, int index)
{
    int i, prev_rbyte=0, down_index[]={0,1};
    struct equip_conn_info *pConnInfo = conn_info[index];


    int wbytes=0;
    unsigned char rxtmp[512];

    for(i=0; i<(sizeof(down_index)/sizeof(down_index[0])); i++)
    {
        wbytes = make_send_frame(rxtmp, index, DDC_REQ_DATA_TYPE, down_index[i]);
        usleep(80000);
        if(sendto_module(fd, rxtmp, wbytes) != 0)						{ fileLog(WARNING, "SERVER -> SEUNGIL eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0, sizeof(rxtmp));
        if(readfrom_module(fd, rxtmp, index, DDC_REQ_DATA_TYPE) != 0)	{ fileLog(WARNING, "SERVER <- SEUNGIL eseq=[%d] Packet Recv Fail\n", pConnInfo->eseq); return -1; }
        memcpy(buff+prev_rbyte, rxtmp+15, DDC_HANGUL_DATA_RES_SIZE-16);
        prev_rbyte += DDC_HANGUL_DATA_RES_SIZE-16;
    }

    return 0;
}

int communicate_seungil_sthc_mb_fw36(int fd, unsigned char *buff, int index)
{
    int wbytes=0;
    struct equip_conn_info *pConnInfo = conn_info[index];
    unsigned char rxtmp[512];

	wbytes = make_send_frame(rxtmp, index, DDC_REQ_DATA_TYPE, 64);
	usleep(80000);
	if(sendto_module(fd, rxtmp, wbytes) != 0)						{ fileLog(WARNING, "SERVER -> SEUNGIL_STHC_MB_FW36 eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

	memset(rxtmp, 0, sizeof(rxtmp));
	if(readfrom_module(fd, rxtmp, index, DDC_REQ_DATA_TYPE) != 0)	{ fileLog(WARNING, "SERVER <- SEUNGIL_STHC_MB_FW36 eseq=[%d] Packet Recv Fail\n", pConnInfo->eseq); return -1; }
	memcpy(buff, rxtmp+15, 0xFE);
	

    return 0;
}

/*
 * SYSKOREA 항온항습기
 */
int communicate_syskorea(int fd, unsigned char *buff, int index)
{
    int wbytes = 0;
    struct equip_conn_info *pConnInfo = conn_info[index];
    usleep(100000);

    wbytes = make_send_frame(buff, index, BY_CRC2004_REQ_CMD, 0);
    if(sendto_module(fd, buff, wbytes) != 0)					{ fileLog(WARNING, "SERVER -> SYSKOREA eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

    memset(buff, 0x00, SYSKOREA_REQ_RES_SIZE);
    if(readfrom_module(fd, buff, index, SYSKOREA_REQ_CMD) != 0)	{ fileLog(WARNING, "SERVER <- SYSKOREA eseq=[%d] Packet Recv Fail\n", pConnInfo->eseq); return -1; }

    return 0;
}

/*
 * 범양 CRC-2004 항온항습기
 */
int communicate_by_crc2004(int fd, unsigned char *buff, int index)
{
    int wbytes = 0;
    struct equip_conn_info *pConnInfo = conn_info[index];
    usleep(100000);

    wbytes = make_send_frame(buff, index, BY_CRC2004_REQ_CMD, 0);
    if(sendto_module(fd, buff, wbytes) != 0)						{ fileLog(WARNING, "SERVER -> BY_CRC2004 eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

    memset(buff, 0x00, BY_CRC2004_REQ_RES_SIZE);
    if(readfrom_module(fd, buff, index, BY_CRC2004_REQ_CMD) != 0)	{ fileLog(WARNING, "SERVER <- BY_CRC2004 eseq=[%d] Packet Recv Fail\n", pConnInfo->eseq); return -1; }

    return 0;
}

/*
 * 서우 항온항습기(대연MUX 연결 모델)
 */
int communicate_dy_ss2000(int fd, unsigned char *buff, int index)
{
    int wbytes = 0;
    unsigned char rxtmp[128];
    struct equip_conn_info *pConnInfo = conn_info[index];
    usleep(100000);

    wbytes = make_send_frame(rxtmp, index, SYSTRONIC_P_REQ_CMD, 0);
    if(sendto_module(fd, rxtmp, wbytes) != 0)						{ fileLog(WARNING, "SERVER -> DY_SS2000 eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

    memset(rxtmp, 0x00, sizeof(rxtmp));
    if(readfrom_module(fd, rxtmp, index, SYSTRONIC_P_REQ_CMD) != 0)	{ fileLog(WARNING, "SERVER <- DY_SS2000 eseq=[%d] Packet Recv Fail\n", pConnInfo->eseq); return -1; }
    memcpy(buff, rxtmp+6, 38);

    return 0;
}

/*
 * Liebert CHTP
 */
int communicate_liebert_chtp(int fd, unsigned char *buff, int index)
{
    int i=0, prev_idx=0;
    int wbytes=0;
    unsigned char rxtmp[128];
    int regAddr[][2]={{  0, 288},  // Unit 1
                      { 24, 293},  // Unit 2
                      { 48, 298},  // Unit 3
                      { 72, 303},  // Unit 4
                      { 96, 308},  // Unit 5
                      {120, 313},  // Unit 6
                      {144, 318},  // Unit 7
                      {168, 323},  // Unit 8
                      {192, 328},  // Unit 9
                      {216, 333},  // Unit 10
                      {240, 338},  // Unit 11
                      {264, 343}}, // Unit 12
        wordCnt[]={24, 5};
    struct equip_conn_info *pConnInfo = conn_info[index];

    usleep(100000);

    for(i=0; i<(int)sizeof(regAddr)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, 3, regAddr[pConnInfo->ext_addr-1][i], wordCnt[i], NULL);

        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(WARNING, "SERVER -> LIEBERT_CHTP eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));

        if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) != 0)	{ return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
    }

    return 0;
}


int communicate_rtu_liebert_chtp(int fd, unsigned char *buff, int index)
{
    int i=0, prev_idx=0;
    int wbytes=0;
    unsigned char rxtmp[128];
    int regAddr[]={0, 0},
        cmdCode[]={0x03, 0x02},
        wordCnt[]={28, 23};
    struct equip_conn_info *pConnInfo = conn_info[index];

    usleep(100000);

    for(i=0; i<(int)sizeof(regAddr)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmdCode[i], regAddr[i], wordCnt[i], NULL);
        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(WARNING, "SERVER -> RTU_LIEBERT_CHTP eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));
        if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) != 0)	{ return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
    }

    return 0;
}

int communicate_rtu_oc485(int fd, unsigned char *buff, int index)
{
    int i=0, prev_idx=0;
    int wbytes=0;
    unsigned char rxtmp[1024];
    int regAddr[]={0, 0},
        cmdCode[]={0x03, 0x02},
        wordCnt[]={84, 63};
    struct equip_conn_info *pConnInfo = conn_info[index];

    usleep(100000);

    for(i=0; i<(int)sizeof(regAddr)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmdCode[i], regAddr[i], wordCnt[i], NULL);
        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(WARNING, "SERVER -> RTU_OC485 eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));
        if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) != 0)	{ return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
    }

    return 0;
}

int communicate_rtu_ar_hane09(int fd, unsigned char *buff, int index)
{
    int i=0, prev_idx=0;

    int wbytes=0;
    unsigned char rxtmp[1024];
    int regAddr[]={0, 0},
        cmdCode[]={0x03, 0x02},
        wordCnt[]={12, 22};
    struct equip_conn_info *pConnInfo = conn_info[index];

    usleep(100000);

    for(i=0; i<(int)sizeof(regAddr)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmdCode[i], regAddr[i], wordCnt[i], NULL);
        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(WARNING, "SERVER -> RTU_AR_HANE09 eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));
        if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) != 0)	{ return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
    }

    return 0;
}

int communicate_rtu_ar_hane09_q(int fd, unsigned char *buff, int index)
{
    int i=0, prev_idx=0;

    int wbytes=0;
    unsigned char rxtmp[1024];
    int regAddr[]={0, 0},
        cmdCode[]={0x03, 0x02},
        wordCnt[]={12, 22};
    struct equip_conn_info *pConnInfo = conn_info[index];

    usleep(100000);

    for(i=0; i<(int)sizeof(regAddr)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmdCode[i], regAddr[i], wordCnt[i], NULL);
        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(WARNING, "SERVER -> RTU_AR_HANE09_Q eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));
        if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) != 0)	{ return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
    }

    return 0;
}

int communicate_rtu_gimaci(int fd, unsigned char *buff, int index)
{
    int i=0, prev_idx=0;

    int wbytes=0;
    unsigned char rxtmp[128];
    int regAddr[]={0, 0},
        cmdCode[]={0x03, 0x02},
        wordCnt[]={22, 1};
    struct equip_conn_info *pConnInfo = conn_info[index];

    usleep(100000);

    for(i=0; i<(int)sizeof(regAddr)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmdCode[i], regAddr[i], wordCnt[i], NULL);
        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(WARNING, "SERVER -> RTU_GIMACI eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));
        if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) != 0)	{ return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
    }

    return 0;
}

int communicate_rtu_gimaciiplus(int fd, unsigned char *buff, int index)
{
    int i=0, prev_idx=0;

    int wbytes=0;
    unsigned char rxtmp[128];
    int regAddr[]={0, 0},
        cmdCode[]={0x03, 0x02},
        wordCnt[]={34, 1};
    struct equip_conn_info *pConnInfo = conn_info[index];

    usleep(100000);

    for(i=0; i<(int)sizeof(regAddr)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmdCode[i], regAddr[i], wordCnt[i], NULL);
        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(WARNING, "SERVER -> RTU_GIMACIIPLUS eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));
        if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) != 0)	{ return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
    }

    return 0;
}

int communicate_rtu_gipam115fi(int fd, unsigned char *buff, int index)
{
    int i=0, prev_idx=0;

    int wbytes=0;
    unsigned char rxtmp[128];
    int regAddr[]={0, 0},
        cmdCode[]={0x03, 0x02},
        wordCnt[]={21, 1};
    struct equip_conn_info *pConnInfo = conn_info[index];

    usleep(100000);

    for(i=0; i<(int)sizeof(regAddr)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmdCode[i], regAddr[i], wordCnt[i], NULL);
        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(WARNING, "SERVER -> RTU_GIPAM115FI eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));
        if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) != 0)	{ return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
    }

    return 0;
}

int communicate_rtu_mpm330a(int fd, unsigned char *buff, int index)
{
    int i=0, prev_idx=0;

    int wbytes=0;
    unsigned char rxtmp[128];
    int regAddr[]={0, 0},
        cmdCode[]={0x03, 0x02},
        wordCnt[]={61, 1};
    struct equip_conn_info *pConnInfo = conn_info[index];

    usleep(100000);

    for(i=0; i<(int)sizeof(regAddr)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmdCode[i], regAddr[i], wordCnt[i], NULL);
        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(WARNING, "SERVER -> RTU_MPM330A eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));
        if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) != 0)	{ return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
    }

    return 0;
}
/*
 * 두텍 DX100
 */
int communicate_dx100(int fd, unsigned char *buff, int index)
{
    int i=0, prev_idx=0;

    int wbytes=0;
    unsigned char rxtmp[128];
    int regAddr[]={0, 25, 150, 0},
        cmdCode[]={4, 3, 3, 1},
        wordCnt[]={22, 13, 17, 8};
    struct equip_conn_info *pConnInfo = conn_info[index];

    usleep(100000);

    for(i=0; i<(int)sizeof(regAddr)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmdCode[i], regAddr[i], wordCnt[i], NULL);
        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(WARNING, "SERVER -> DX100 eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));
        if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) != 0)	{ return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
    }

    return 0;
}

/*
 * MeX100
 */
int communicate_mex100(int fd, unsigned char *buff, int index)
{

    int i=0, prev_idx=0;
    int wbytes=0;
    unsigned char rxtmp[128];
    int cmd[2]={3, 1},
        wordCnt[]={37, 32};
    struct equip_conn_info *pConnInfo = conn_info[index];

    for(i=0; i<(int)sizeof(cmd)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmd[i], 0, wordCnt[i], NULL);
        usleep(80000);
        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(WARNING, "SERVER -> MEX100 eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));
        if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) != 0)	{ return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
    }

    return 0;
}

int communicate_mst_hmm21(int fd, unsigned char *buff, int index)
{
    int wbytes = 0;
    struct equip_conn_info *pConnInfo = conn_info[index];
    usleep(100000);

    wbytes = make_send_frame(buff, index, '?', 0);
    if(sendto_module(fd, buff, wbytes) != 0)		{ fileLog(WARNING, "SERVER -> MST_HMM21 eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }
    if(readfrom_module(fd, buff, index, 0) != 0)	{ fileLog(WARNING, "SERVER <- MST_HMM21 eseq=[%d] Packet Recv Fail\n", pConnInfo->eseq); return -1; }

    return 0;
}

/*
 * OC485
 */
int communicate_oc485(int fd, unsigned char *buff, int index)
{
    int i=0, prev_idx=0;
    int wbytes=0;
    unsigned char rxtmp[128];
    int cmd[]    ={4, 4, 4, 4, 4, 2},
        regAddr[]={22, 49, 101, 129, 140, 24},
        wordCnt[]={8, 8, 18, 5, 10, 85};
    struct equip_conn_info *pConnInfo = conn_info[index];

    for(i=0; i<(int)sizeof(cmd)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmd[i], regAddr[i], wordCnt[i], NULL);
        usleep(80000);
        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(WARNING, "SERVER -> OC485 eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0, sizeof(rxtmp));
        if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) != 0)	{ return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
    }

    return 0;
}


int communicate_mes100(int fd, unsigned char *buff, int index)
{
    int wbytes = 0;
    struct equip_conn_info *pConnInfo = conn_info[index];
    usleep(100000);

    wbytes = make_modbus_frame(index, buff, pConnInfo->id, 4, MES100_START_ADDR, MES_100_REQ_SIZE, NULL);
    if(sendto_module(fd, buff, wbytes) != 0)									{ fileLog(WARNING, "SERVER -> MES100 eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }
    if(read_modbus_frame_timeout(index, fd, buff, 2, MES_100_REQ_SIZE) != 0)	{ return -1; }

    return 0;
}

int communicate_rm6054(int fd, unsigned char *buff, int index)
{
    int wbytes = 0;
    struct equip_conn_info *pConnInfo = conn_info[index];
    usleep(100000);

    wbytes = make_modbus_frame(index, buff, pConnInfo->id, 4, RM6054_START_ADDR, RM6054_REQ_SIZE, NULL);
    if(sendto_module(fd, buff, wbytes) != 0)								{ fileLog(WARNING, "SERVER -> RM6054 eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }
    if(read_modbus_frame_timeout(index, fd, buff, 2, RM6054_REQ_SIZE) != 0)	{ return -1; }

    return 0;
}

int communicate_rtu_rtu5000(int fd, unsigned char *buff, int index)
{
    int i=0, prev_idx=0;

    int wbytes=0;
    unsigned char rxtmp[128];
    int regAddr[]={0},
        cmdCode[]={0x02},
        wordCnt[]={16};
    struct equip_conn_info *pConnInfo = conn_info[index];
    usleep(100000);

    for(i=0; i<(int)sizeof(regAddr)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmdCode[i], regAddr[i], wordCnt[i], NULL);
        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(WARNING, "SERVER -> RTU_RTU5000 eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));
        if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) != 0)	{ return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
    }

    return 0;
}


int communicate_mpm330(int fd, unsigned char *buff, int index)
{
    unsigned char rxtmp[128];
    int word_count=39;
	int wbytes=0;
    struct equip_conn_info *pConnInfo = conn_info[index];
    usleep(10000);

    memset(rxtmp, 0x00, sizeof(rxtmp));
	wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, 0x03, 1, word_count, NULL);
    if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(WARNING, "SERVER -> MPM330 eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

    memset(rxtmp, 0x00, sizeof(rxtmp));
    if(read_modbus_frame_timeout(index, fd, rxtmp, 2, word_count) != 0)	{ return -1; }

    memcpy(buff, rxtmp, rxtmp[2]+5); //확인 필요 1. 왜 사이즈를 rxtmp[2]+5만큼 하는지, 2. MODBUS_TCP, MODBUS_RTU 분리

    return 0;
}

int communicate_mpm330a(int fd, unsigned char *buff, int index)
{
	int prev_idx=0;
    unsigned char rxtmp[128];
    int word_count=61;
	int wbytes=0;
    struct equip_conn_info *pConnInfo = conn_info[index];
    usleep(10000);

    memset(rxtmp, 0x00, sizeof(rxtmp));
	make_modbus_frame(index, rxtmp, pConnInfo->id, 0x03, 1, word_count, NULL);
    if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(WARNING, "SERVER -> MPM330A eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

    memset(rxtmp, 0x00, sizeof(rxtmp));
    if(read_modbus_frame_timeout(index, fd, rxtmp, 2, word_count) != 0)	{ return -1; }

	if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
	else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }

    return 0;
}


int communicate_solid(int fd, unsigned char *buff, int index)
{
    int wbytes = 0;
    struct equip_conn_info *pConnInfo = conn_info[index];
    usleep(100000);

    wbytes = make_modbus_frame(index, buff, pConnInfo->id, 4, SOLID_START_ADDR, SOLID_REQ_SIZE, NULL);
    if(sendto_module(fd, buff, wbytes) != 0)								{ fileLog(WARNING, "SERVER -> SOLID eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }
    if(read_modbus_frame_timeout(index, fd, buff, 2, SOLID_REQ_SIZE) != 0)	{ return -1; }

    return 0;
}


int communicate_itts2(int fd, unsigned char *buff, int index)
{
    int wbytes = 0;
    struct equip_conn_info *pConnInfo = conn_info[index];
    usleep(100000);

    wbytes = make_send_frame(buff, index, 0, 2);
    if(sendto_module(fd, buff, wbytes) != 0)		{ fileLog(WARNING, "SERVER -> ITTS2 eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }
    if(readfrom_module(fd, buff, index, 0) != 0)	{ fileLog(WARNING, "SERVER <- ITTS2 eseq=[%d] Packet Recv Fail\n", pConnInfo->eseq); return -1; }

    return 0;
}


int communicate_HipulseU(int fd, unsigned char *buff, int index)
{
    int wbytes=0, i, prev_idx=0;
    int cmdCode[]={0x03, 0x03},
		regAddr[]={1050, 1030},
        wordCnt[]={52, 5};
    unsigned char rxtmp[1024];
    struct equip_conn_info *pConnInfo = conn_info[index];

    usleep(50000);

    for(i=0; i<(int)(sizeof(regAddr)/sizeof(int)); i++)
    {
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmdCode[i], regAddr[i], wordCnt[i], NULL);
        if(sendto_module(fd, rxtmp, wbytes) != 0)								{ fileLog(WARNING, "SERVER -> HIPULSEU eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0, sizeof(rxtmp));
        if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) == -1)	{ return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
        usleep(100000);
    }

    return 0;
}

int communicate_cimon(int fd, unsigned char *buff, int index)
{
    int i=0, prev_idx=0;
    int wbytes=0;
    unsigned char rxtmp[128];
    int regAddr[]={6},
        cmdCode[]={3},
        wordCnt[]={22};
    struct equip_conn_info *pConnInfo = conn_info[index];
    usleep(100000);

    for(i=0; i<(int)sizeof(regAddr)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmdCode[i], regAddr[i], wordCnt[i], NULL);
        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(WARNING, "SERVER -> CIMON eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));
        if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) != 0)	{ return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
    }

    return 0;
}


int communication_innotube(int fd, unsigned char *buff, int index)
{

    int wbytes = 0;
    struct equip_conn_info *pConnInfo = conn_info[index];
    usleep(100000);

    wbytes = make_send_frame(buff, index, pConnInfo->id, 0);
    if(sendto_module(fd, buff, wbytes) != 0)		{ fileLog(WARNING, "innotube INNOTUEBE eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }
    if(readfrom_module(fd, buff, index, 0) != 0)	{ fileLog(WARNING, "innotube INNOTUEBE eseq=[%d] Packet Recv Fail\n", pConnInfo->eseq); return -1; }

	return 0;
}

int communication_rtu_th(int fd, unsigned char *buff, int index)
{
	int regAddr=0;
    int wbytes=0, word_count = 2;
	int prev_idx=0;
    unsigned char rxtmp[256];
    struct equip_conn_info *pConnInfo = conn_info[index];

    usleep(50000);

	switch(pConnInfo->model_seq)
	{
		case RTU_TH1: regAddr= 0;	break;
		case RTU_TH2: regAddr= 2;	break;
		case RTU_TH3: regAddr= 4;	break;
		case RTU_TH4: regAddr= 6;	break;
		case RTU_TH5: regAddr= 8;	break;
		case RTU_TH6: regAddr=10;	break;
		case RTU_TH7: regAddr=12;	break;
		case RTU_TH8: regAddr=14;	break;
	}
	wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, 3, regAddr, word_count, NULL);
	if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(WARNING, "SERVER -> RTU_TH eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

	memset(rxtmp, 0x00, sizeof(rxtmp));
	if(read_modbus_frame_timeout(index, fd, rxtmp, 2, word_count) != 0)	{ return -1; }

	if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
	else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }

    return 0;
}

int communication_rtu_ttdm128(int fd, unsigned char *buff, int index)
{
    int i=0, prev_idx=0;

    int wbytes=0;
    unsigned char rxtmp[512];
    int cmdCode[]={0x03, 0x02},
        wordCnt[]={2, 1};
    struct equip_conn_info *pConnInfo = conn_info[index];

    usleep(100000);

    for(i=0; i<(int)sizeof(cmdCode)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
		if(cmdCode[i] == 0x02)
		{
			wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmdCode[i], (pConnInfo->ext_addr-1), wordCnt[i], NULL);
		}
		else
		{
			wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmdCode[i], (pConnInfo->ext_addr-1)*2, wordCnt[i], NULL);
		}
        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(WARNING, "SERVER -> RTU_TTDM128 eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));
        if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) != 0)	{ return -1; }

        if(1 != (unsigned int)rxtmp[0])
        {
            fileLog(WARNING, "RTU_TTDM128 eseq=[%d] Response ID Error: Request[%02X] Response[%02X]\n", pConnInfo->eseq, pConnInfo->id, rxtmp[0]);
            return -1;
        }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
    }

    return 0;
}

int communicate_modbus(int fd, unsigned char *buff, int index)
{
    int wbytes=0;
	int cmd=0, addr=0, count=0;
    unsigned char rxtmp[1024];
    struct equip_conn_info *pConnInfo = conn_info[index];
	int prev_idx=0;
    usleep(100000);

    memset(rxtmp, 0x00, sizeof(rxtmp));
	switch(pConnInfo->model_seq)
	{
		case DPM_GiMACi			:
		case DPM_GiMACi_SG1040	: cmd = GiMACi_REQ_CMD;			addr = GiMACi_REQ_ADDR;			count = GiMACi_REQ_CNT;			break;
		case DPM_GiMACIIPlus	: cmd = GiMACIIPlus_REQ_CMD;	addr = GiMACIIPlus_REQ_ADDR;	count = GiMACIIPlus_REQ_CNT;	break;
		case DPM_GiPAM115FI		: cmd = GiPAM115FI_REQ_CMD;		addr = GiPAM115FI_REQ_ADDR;		count = GiPAM115FI_REQ_CNT;		break;
		case DPM_KDY_200		: cmd = KDY200_REQ_CMD;			addr = KDY200_REQ_ADDR;			count = KDY200_REQ_CNT;			break;
		case HIMAP_FI			: cmd = 0x03;					addr = 0x0000;					count = 87;						break;
		case HIMAP_T			: cmd = 0x03;					addr = 0x0000;					count = 87;						break;
		case P_300C				: cmd = 0x04;					addr = 0x0000;					count = 6;						break;
		case SP_INC				: cmd = 0x03;					addr = 0x0000;					count = 2;						break;
		case KM6053				: cmd = 0x04;					addr = 0x0080;					count = 1;						break;
		case LBSM200			: cmd = 0x04;					addr = 0x0001;					count = 2;						break;
		case DDC400				: cmd = 0x03;					addr = 200;						count = 62;						break;
		case DDC_UNKNOWN		: cmd = 0x03;					addr = 200;						count = 82;						break;
		default : return -1;
	}
	wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmd, addr, count, NULL);
    if(sendto_module(fd, rxtmp, wbytes) != 0) { fileLog(WARNING, "SERVER -> MODBUS eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

    memset(rxtmp, 0x00, sizeof(rxtmp));
    if(read_modbus_frame_timeout(index, fd, rxtmp, 2, count) != 0) { return -1; }

	if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
	else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }

    return 0;
}


int communicate_btech(int fd, unsigned char *buff, int index)
{

    int wbytes=0, i, prev_idx=0;
    /* btech 장비에서 한번에 가져올 수 있는 Max 바이트는 0xFE(254)이다.
     * 전압, 온도, 저항이 float 값이기 때문에 한번에 가져오는 수는 50개
     * 50*4 = 200바이트 50 word 로 정하기로 한다. 왜~ 내 맘이니까 ㅋ */
    /* Modbus TCP/IP 프로토콜에 맞추기 위해 register_addr를 400000 단위로 한다. */
    int regAddr[18] =	{24, 124, 224, 324, 424, 524,
						1024, 1124, 1224, 1324, 1424, 1524,
						2029, 2129, 2229, 2329, 2429, 2529},
        cmdCode[18] =	{3,3,3,3,3,3,
						3,3,3,3,3,3,
						3,3,3,3,3,3},
        wordCnt[18] =	{100, 100, 100, 100, 100, 12,
						100, 100, 100, 100, 100, 12,
						100, 100, 100, 100, 100, 12};

    unsigned char rxtmp[1024];
    struct equip_conn_info *pConnInfo = conn_info[index];

    usleep(50000);

    for(i=0; i<(int)(sizeof(regAddr)/sizeof(int)); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmdCode[i], regAddr[i], wordCnt[i], NULL);
        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(WARNING, "SERVER -> BTECH eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));
		if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) != 0)	{ return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
    }

    return 0;
}


int communicate_ttdm128(int fd, unsigned char *buff, int index)
{
    int wbytes=0, word_count=2;
	int prev_idx=0;
    unsigned char rxtmp[256];
    struct equip_conn_info *pConnInfo = conn_info[index];
    usleep(50000);

	wbytes = make_modbus_frame(index, rxtmp, 1, TTDM128_REQ_CMD, TTDM128_START_REG_ADDR + (16 * pConnInfo->id), word_count, NULL);
	if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(WARNING, "SERVER -> TTDM128 eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

	memset(rxtmp, 0x00, sizeof(rxtmp));
	if(read_modbus_frame_timeout(index, fd, rxtmp, 2, word_count) != 0)	{ return -1; }

	if(1 != (unsigned int)rxtmp[6])
	{
		fileLog(WARNING, "TTDM128 eseq=[%d] Response Frame Device Id Error: Req[%02X] Res[%02X]\n", pConnInfo->eseq, 1, rxtmp[6]);
		return -1;
	}

	if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
	else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }

    return 0;
}


int communicate_sthav_mb_r1(int fd, unsigned char *buff, int index)
{

    int i=0, prev_idx=0;
    int wbytes=0;
    unsigned char rxtmp[128];
    int cmd[3]		={3,  3,  3},
		regAddr[3]	={0, 14, 53},
        wordCnt[3]	={8,  5, 28};

    struct equip_conn_info *pConnInfo = conn_info[index];

    for(i=0; i<(int)sizeof(cmd)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmd[i], regAddr[i], wordCnt[i], NULL);
        usleep(80000);
        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(WARNING, "SERVER -> STHAV_MB_R1 eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));
        if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) != 0)	{ return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
		usleep(1000000);
    }

    return 0;
}

int communicate_a_llm1(int fd, unsigned char *buff, int index)
{
	int prev_idx=0;
    int wbytes=0;
    unsigned char rxtmp[256];
    struct equip_conn_info *pConnInfo = conn_info[index];
    usleep(50000);

	wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, A_LLM1_REQ_CMD, A_LLM1_START_REG_ADDR, A_LLM1_MODBUS_WORD_CNT, NULL);
	if(sendto_module(fd, rxtmp, wbytes) != 0)										{ fileLog(WARNING, "SERVER -> A_LLM1 eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

	memset(rxtmp, 0x00, sizeof(rxtmp));
	if(read_modbus_frame_timeout(index, fd, rxtmp, 2, A_LLM1_MODBUS_WORD_CNT) != 0)	{ return -1; }

	if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
	else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }

    return 0;
}

int communicate_rtu_SuperSwitch3_STS(int fd, unsigned char *buff, int index)
{
    int i=0, prev_idx=0;

    int wbytes=0;
    unsigned char rxtmp[512];
    int regAddr[]={0, 0},
        cmdCode[]={0x03, 0x02},
        wordCnt[]={123, 39};
    struct equip_conn_info *pConnInfo = conn_info[index];

    usleep(100000);

    for(i=0; i<(int)sizeof(regAddr)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmdCode[i], regAddr[i], wordCnt[i], NULL);
        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(WARNING, "SERVER -> RTU_SUPERSWITCH3_STS eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));
        if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) != 0) { return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
    }

    return 0;
}

int communicate_SuperSwitch3_STS(int fd, unsigned char *buff, int index)
{
    int idx, prev_idx;
    int wbytes=0;
    unsigned char rxtmp[128];
    struct equip_conn_info *pConnInfo = conn_info[index];

    int ss3_cmd_type[][2] = {
        {1000, 8},{1009, 3},{1018, 6},
        {1024, 1},{1026, 1},{1028, 1},
        {1034, 6},{1062, 9},{1100, 2},{1114, 2},
        {1128, 2},{1142, 2},{1156, 2},{1170, 2},

        {2000, 8},{2009, 3},{2018, 6},
        {2024, 1},{2026, 1},{2028, 1},
        {2034, 6},{2062, 9},{2100, 2},{2114, 2},
        {2128, 2},{2142, 2},{2156, 2},{2170, 2},

        {1040, 6},{1047, 1},{1053 ,2},{1307, 1},{1500, 1},{1051, 2},
        {2040, 6},{2047, 1},{2053 ,2},{2307, 1},{2500, 1},{2051, 2}};


	for(idx=0, prev_idx=0; idx < (int)(sizeof(ss3_cmd_type)/sizeof(ss3_cmd_type[0])); idx++)
	{
		memset(rxtmp, 0x00, sizeof(rxtmp));
		wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, UPSILON_READ_CMD, ss3_cmd_type[idx][0], ss3_cmd_type[idx][1], NULL);
		usleep(50000);
		if(sendto_module(fd, rxtmp, wbytes) != 0)										{ fileLog(WARNING, "SERVER -> SUPERSWITCH3_STS eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

		memset(rxtmp, 0x00, sizeof(rxtmp));
		if(read_modbus_frame_timeout(index, fd, rxtmp, 1, ss3_cmd_type[idx][1]) != 0)	{ return -1; }
		
		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
	}

    return 0;
}


int communicate_iljin_alts(int fd, unsigned char *buff, int index)
{
    int i=0, prev_idx=0;

    int wbytes=0;
    unsigned char rxtmp[1024];
    int regAddr[]={1000, 1500},
        cmdCode[]={0x04, 0x04},
        wordCnt[]={30, 4};
    struct equip_conn_info *pConnInfo = conn_info[index];

    for(i=0; i<(int)sizeof(regAddr)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmdCode[i], regAddr[i], wordCnt[i], NULL);
        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(CRITICAL, "Server -> ILJIN_ALTS eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));
		if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) != 0)	{ return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
	    usleep(100000);

    }

    return 0;
}


int communicate_pac100(int fd, unsigned char *buff, int index)
{
    int i=0, prev_idx=0;

    int wbytes=0;
    unsigned char rxtmp[1024];
    int cmdCode[]={0x04, 0x04, 0x04, 0x04, 0x04},
		regAddr[]={1000, 1021, 1050, 1065, 1134},
		wordCnt[]={14,   6,    4,    2,   1};
    struct equip_conn_info *pConnInfo = conn_info[index];

    usleep(100000);

    for(i=0; i<(int)sizeof(regAddr)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmdCode[i], regAddr[i], wordCnt[i], NULL);
        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(CRITICAL, "Server -> PAC100 eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));
		if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) != 0)	{ return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
    }

    return 0;
}


int communicate_accura3700(int fd, unsigned char *buff, int index)
{
    int i=0, prev_idx=0;

    int wbytes=0;
    unsigned char rxtmp[1024];
    int cmdCode[]={0x03,  0x03,  0x03},
		regAddr[]={11100, 11412, 12208 },
		wordCnt[]={70,    2,     4};
    struct equip_conn_info *pConnInfo = conn_info[index];

    usleep(100000);

    for(i=0; i<(int)sizeof(regAddr)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmdCode[i], regAddr[i], wordCnt[i], NULL);
        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(CRITICAL, "Server -> PAC100 eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));
		if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) != 0)	{ return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
    }

    return 0;
}


int communicate_accura2300s_3p1(int fd, unsigned char *buff, int index)
{
    int i=0, prev_idx=0;

    int wbytes=0;
    unsigned char rxtmp[1024];
    int cmdCode[]={0x03,  0x03},
		regAddr[]={11100, 11200},
		wordCnt[]={100,   100};
    struct equip_conn_info *pConnInfo = conn_info[index];

    usleep(100000);

    for(i=0; i<(int)sizeof(regAddr)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmdCode[i], regAddr[i], wordCnt[i], NULL);
        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(CRITICAL, "Server -> ACCURA2300S_3P1 eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));
		if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) != 0)	{ return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
    }

    return 0;
}

int communicate_accura2300s_3p41(int fd, unsigned char *buff, int index)
{
    int i=0, prev_idx=0;

    int wbytes=0;
    unsigned char rxtmp[1024*16];
    int regAddr[]={11100, 
					11200, 11300, 11400, 11500, 11600, 11700, 11800, 11900, 12000, 12100, 12200, 12300, 12400, 12500, 12600, // #00 ~ #09
					12700, 12800, 12900, 13000, 13100, 13200, 13300, 13400, 13500, 13600, 13700, 13800, 13900, 14000, 14100, // #10 ~ #19
					14200, 14300, 14400, 14500, 14600, 14700, 14800, 14900, 15000, 15100, 15200, 15300, 15400, 15500, 15600, // #20 ~ #29
					15700, 15800, 15900, 16000, 16100, 16200, 16300, 16400, 16500, 16600, 16700, 16800, 16900, 17000, 17100, // #30 ~ #39
					17200, 17300}; // #40

    struct equip_conn_info *pConnInfo = conn_info[index];

    usleep(100000);

    for(i=0; i<(int)sizeof(regAddr)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, 0x03, regAddr[i], 100, NULL);
        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(CRITICAL, "Server -> ACCURA2300S_3P41 eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));
		if(read_modbus_frame_timeout(index, fd, rxtmp, 2, 100) != 0)	{ return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
    }

    return 0;
}

int communicate_accura2300s_3p3_1p3f18(int fd, unsigned char *buff, int index)
{
    int i=0, prev_idx=0;

    int wbytes=0;
    unsigned char rxtmp[1024*8];
    int regAddr[]={11100, 
					11200, 11300, 11400, 11500, 11600, 11700, 11800, 11900, 12000, 12100, 12200, 12300, 12400, 12500, 12600, // #00 ~ #09
					12700, 12800, 12900, 13000, 13100, 13200, 13300, 13400, 13500, 13600, 13700, 13800, 13900, 14000, 14100, // #10 ~ #19
					14200, 14300}; // #20
    struct equip_conn_info *pConnInfo = conn_info[index];

    usleep(100000);

    for(i=0; i<(int)sizeof(regAddr)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, 0x03, regAddr[i], 100, NULL);
        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(CRITICAL, "Server -> ACCURA2300S_3P3_1P3F18 eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));
		if(read_modbus_frame_timeout(index, fd, rxtmp, 2, 100) != 0)	{ return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
    }

    return 0;
}

int communicate_accura3300e(int fd, unsigned char *buff, int index)
{
    int i=0, prev_idx=0;

    int wbytes=0;
    unsigned char rxtmp[1024];
    int cmdCode[]={0x03,  0x03},
		regAddr[]={11000, 11100},
		wordCnt[]={100,   100};
    struct equip_conn_info *pConnInfo = conn_info[index];

    usleep(100000);

    for(i=0; i<(int)sizeof(regAddr)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmdCode[i], regAddr[i], wordCnt[i], NULL);
        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(CRITICAL, "Server -> ACCURA3300E eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));
		if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) != 0)	{ return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
    }

    return 0;
}

int communicate_gdr(int fd, unsigned char *buff, int index)
{
    int i=0, prev_idx=0;

    int wbytes=0;
    unsigned char rxtmp[1024];
    int regAddr[]={1000, 1100},
        cmdCode[]={0x04, 0x04},
        wordCnt[]={1,    2};
    struct equip_conn_info *pConnInfo = conn_info[index];

    usleep(100000);

    for(i=0; i<(int)sizeof(regAddr)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmdCode[i], regAddr[i], wordCnt[i], NULL);
        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(CRITICAL, "Server -> PAC100 eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));
		if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) != 0)	{ return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
    }

    return 0;
}

int communicate_exp40_ttf_dc(int fd, unsigned char *buff, int index)
{
    int i=0, prev_idx=0;

    int wbytes=0;
    unsigned char rxtmp[512];
	int cmdCode[]={0x03, 0x02},
		regAddr[]={0, 0},
		wordCnt[]={11, 13};
    struct equip_conn_info *pConnInfo = conn_info[index];

    usleep(100000);

    for(i=0; i<(int)sizeof(regAddr)/(int)sizeof(int); i++)
    {
        memset(rxtmp, 0x00, sizeof(rxtmp));
        wbytes = make_modbus_frame(index, rxtmp, pConnInfo->id, cmdCode[i], regAddr[i], wordCnt[i], NULL);
        if(sendto_module(fd, rxtmp, wbytes) != 0)							{ fileLog(WARNING, "SERVER -> EXP40_TTF_DC eseq=[%d] Packet Send Fail\n", pConnInfo->eseq); return -1; }

        memset(rxtmp, 0x00, sizeof(rxtmp));
        if(read_modbus_frame_timeout(index, fd, rxtmp, 2, wordCnt[i]) != 0) { return -1; }

		if(pConnInfo->header == HEADER_UNKNOWN_TYPE)	{ memcpy(buff+prev_idx, rxtmp+3, rxtmp[2]); prev_idx += rxtmp[2]; }
		else if(pConnInfo->header == HEADER_MODBUS_TCP)	{ memcpy(buff+prev_idx, &rxtmp[9], rxtmp[8]); prev_idx += rxtmp[8]; }
    }

    return 0;
}