#include "include.h"

extern int equip_count;
extern struct equip_conn_info *conn_info[];
extern struct equip_status *gp_status[];

void modbus_exception(char code)
{
	switch(code)
	{
		case 0x01:	fileLog(CRITICAL, "MODBUS Exception Response Illegal Function\n");					break;
		case 0x02:	fileLog(CRITICAL, "MODBUS Exception Response Illegal Data Address\n");				break;
		case 0x03:	fileLog(CRITICAL, "MODBUS Exception Response Illegal Data Value\n");				break;
		case 0x04:	fileLog(CRITICAL, "MODBUS Exception Response Slave Device Failure\n");				break;
		case 0x05:	fileLog(CRITICAL, "MODBUS Exception Response Acknowledge\n");						break;
		case 0x06:	fileLog(CRITICAL, "MODBUS Exception Response Slave Device Busy\n");					break;
		case 0x07:	fileLog(CRITICAL, "MODBUS Exception Response Negative Acknowledge\n");				break;
		case 0x08:	fileLog(CRITICAL, "MODBUS Exception Response Memory Parity Error\n");				break;
		case 0x0A:	fileLog(CRITICAL, "MODBUS Exception Gateway Path Unavailable\n");					break;
		case 0x0B:	fileLog(CRITICAL, "MODBUS Exception Gateway Target Device Failed to Respond\n");	break;
		default  :	fileLog(CRITICAL, "MODBUS Exception code=[%02X]\n",code);							break;
	}
}

void *modbus_Server(void *arg)
{
	fileLog(INFO, "[%s:%d] \n", __FUNCTION__, __LINE__);
    int server_sockfd, client_sockfd, *pclient_sockfd, err_status;
    struct sockaddr client_addr;
    socklen_t client_len;
    pthread_t t_id;
    pthread_attr_t t_attr;

    char err_buff[256] = {0x00, };
#ifdef DEBUG
    fileLog(INFO, "MODBUS SERVER Thread Start..\n");
#endif
    pthread_attr_init(&t_attr);
    pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

    if((server_sockfd = makeServer(MODBUS_SERVER_PORT)) == -1)
        fileLog(CRITICAL, "Create Modbus Server Listen Port Fail modbus_server_port=[%d]\n", MODBUS_SERVER_PORT);

    while(1)
    {
        client_len = sizeof(client_addr);
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &client_len);
        if(client_sockfd == -1)
        {
            fileLog(WARNING, "Accept Fail.. %s\n", strerror(errno));
            continue;
        }
#ifdef DEBUG
        fileLog(INFO, "Accept OK.. Accept Socket Fd=[%d]\n", client_sockfd);
#endif
        pclient_sockfd  = (int *)malloc(sizeof(int));

        *pclient_sockfd = client_sockfd;
        err_status = pthread_create(&t_id, &t_attr, modbus_Client, (void *)pclient_sockfd);

		if(0 != err_status)
		{
			fileLog(WARNING, "[%s:%d] Thread Create Fail: %s\n", __FUNCTION__, __LINE__, (char *)strerror_r(err_status, err_buff, sizeof(err_buff)));
			close(client_sockfd);
		}

		
        usleep(100);
    }
	return (void*)1;	//void return인데 리턴 안쓰면 warning 남 확인 필요
}

void *modbus_Client(void *arg)
{
    int fd=0, err_code=0, nfound=0;
    unsigned char rxbuf[10];
    struct timeval timeout;
    fd_set rmask, mask;

    fd = *((int *)arg);
    free(arg);

    while(1)
    {
        timeout.tv_sec  = 60L;
        timeout.tv_usec = 0L;

        FD_ZERO(&mask);
        FD_SET (fd, &mask);

        rmask = mask;
        nfound = select(fd+1, &rmask, (fd_set *)0, (fd_set *)0, &timeout);

        if(nfound == 0)
        {
            fileLog(WARNING, "No Request ... in %d sec\n", timeout.tv_sec);
            break;
        }

        else if(nfound < 0)
        {
            fileLog(WARNING, "Interrupted System Call.. or On error.. \n");
            break;
        }

        if(FD_ISSET(fd, &rmask))
        {
            if(readn(fd, (char *)rxbuf, 8) != 8)
            {
                fileLog(WARNING, "Read Modbus Frame Fail..\n");
                break;
            }

            err_code = modbus_frame_check(rxbuf);
            fileLog(WARNING, "modbus frame check result: %d\n", err_code);
            if(err_code == 0)
                modbus_response_ack(fd, rxbuf);
            else
                modbus_response_nak(fd, err_code, rxbuf);
        }
    }
#ifdef DEBUG
    fileLog(INFO, "Modbus Thread End.. Socket FD(%d)\n", fd);
#endif
    FD_CLR(fd, &rmask);
    close(fd);
	return (void*)1;	//void return인데 리턴 안쓰면 warning 남 확인 필요
}

int modbus_frame_check(unsigned char *buff)
{
    int reg_addr=0, word_count=0;
    unsigned short crc16=0;

    if(!(buff[0]>=1 && buff[0]<=equip_count))
        return 4;   /* 04 - Slave Device Fail\n */

    if(!(
//         buff[1]==1 ||/* Read Coil Status */
         buff[1]==2 || /* Read Input Status */
         buff[1]==3    /* Read Holding Registers */
//         buff[1]==4 || /* Read Input Registers */
//         buff[1]==5 || /* Force Single Coil */
//         buff[1]==6 /* Preset Single Register */
         ))
        return 1;       /* 01h - Illegal Function */

    reg_addr = (buff[2]<<8) | buff[3];
    if(!(reg_addr >= 0 && reg_addr <= 0xFFFF))
        return 2;       /* 02h - Illegal Data Address */

    word_count = (buff[4]<<8) | buff[5];
    if(buff[1] == 1 || buff[1] == 2) // Coil Status, Input Status
    {
        if(!(word_count >= 0 && word_count <= 2040))
            return 3;   // 03h - Illegal Data Value
    }

    if(buff[1] == 3 || buff[1] == 4) // Holding Register, Input Register
    {
        if(!(word_count >= 0 && word_count <= 127))
            return 3;   // 03h - Illegal Data Value
    }
/*
    if(buff[1] == 5)
    {
        if(!(reg_addr >= 0 && reg_addr <= 1))
            return 2;   // 02h - Illegal Data Address
    }

    if(buff[1] == 6)
    {
        if(!(reg_addr >= 20 && reg_addr <= 23))
            return 2;   // 02h - Illegal Data Address
    }
*/
    crc16 = modbus_crc16(buff, 6);

    if( ((crc16 & 0xFF00) >> 8) != buff[6] || (crc16 & 0x00FF) != buff[7])
        return 3;       /* 03h - Illegal Data Value */

    return 0;
}

int modbus_response_nak(int fd, int err_code, unsigned char *rxbuf)
{
    int wbytes=0;
    unsigned char txbuf[10];
    unsigned short crc16=0;

    txbuf[wbytes++] = rxbuf[0];
    txbuf[wbytes++] = rxbuf[1] | 0x80;
    txbuf[wbytes++] = err_code;
    crc16 = modbus_crc16(txbuf, wbytes);
    txbuf[wbytes++] = (crc16 & 0xFF00) >> 8;
    txbuf[wbytes++] =  crc16 & 0x00FF;

    writen(fd, (char *)txbuf, wbytes);

    return 0;
}

int modbus_response_ack(int fd, unsigned char *rxbuf)
{
    int wbytes=0, reg_addr=0, word_count=0, i=0;
    unsigned char txbuf[300];
    unsigned short crc16=0;
    struct equip_ai *pAI=NULL;
    struct equip_di *pDI=NULL;
//    struct equip_do *pDO=NULL;
    struct equip_conn_info *pConnInfo=conn_info[rxbuf[0]-1];

    reg_addr   = (rxbuf[2]<<8) | rxbuf[3];
    word_count = (rxbuf[4]<<8) | rxbuf[5];

    memset(txbuf, 0x00, sizeof(txbuf));

    txbuf[wbytes++] = rxbuf[0];
    txbuf[wbytes++] = rxbuf[1];

    // Coil Status
/*
    if(rxbuf[1] == 1)
    {
        txbuf[wbytes++] = (word_count/8)+((word_count%8)?1:0);
        for(i=0; i<word_count; i++)
        {
            if(pConnInfo->do_count == 0 || (reg_addr+i)/pConnInfo->do_count >= 1)
                txbuf[3+(i/8)] |= 0<<(i/8);
            else
            {
                pDO = gp_status[rxbuf[0]-1]->_do+((reg_addr+i)%pConnInfo->do_count);
                txbuf[3+(i/8)] |= (pDO->stat)<<(i%8);
            }
        }
        wbytes = 3 + (word_count/8) + ((word_count%8)?1:0);
    }
*/
    /* Input Status */
    if(rxbuf[1] == 0x02)
    {
        txbuf[wbytes++] = (word_count/8)+((word_count%8)?1:0);
        for(i=0; i<word_count; i++)
        {
            if(pConnInfo->di_count == 0 || (reg_addr+i)/pConnInfo->di_count >= 1)
                txbuf[3+(i/8)] |= 0<<(i/8);
            else
            {
                pDI = gp_status[rxbuf[0]-1]->_di+((reg_addr+i)%pConnInfo->di_count);
                txbuf[3+(i/8)] |= (pDI->curr_val)<<(i%8);
            }
        }
        wbytes = 3 + (word_count/8) + ((word_count%8)?1:0);
    }

    /* Holding Register */
    else if(rxbuf[1] == 0x03)
    {
        txbuf[wbytes++] = word_count*2;

        int k=0, txindex=0, ai_number=0;
        float f_val=0.F;
        unsigned char txtemp[1024*2];
#ifdef DEBUG
		fileLog(CRITICAL, "AAAAAAAAAAAAAAAAAAAAAAAA\n"); //에러남 확인 필요
#endif
        pAI = gp_status[rxbuf[0]-1]->_ai;
#ifdef DEBUG
		fileLog(CRITICAL, "BBBBBBBBBBBBBBBBBBBBB\n");
#endif

        for(k=0; k<pConnInfo->ai_count; k++)
        {
            f_val = (pAI+k)->curr_val;
            memmove(&txtemp[txindex], &f_val, sizeof(float));
            txindex += sizeof(float);
            ai_number += 2;
        }

        for(i=0; i<word_count; i++)
        {
            if(pConnInfo->ai_count == 0 || (reg_addr+i)/ai_number >= 1)
            {
                txbuf[wbytes++] = 0x00;
                txbuf[wbytes++] = 0x00;
            }
            else
            {
                txbuf[wbytes++] = txtemp[(reg_addr*2)+(i*2)];
                txbuf[wbytes++] = txtemp[(reg_addr*2)+(i*2)+1];
            }
        }
    }
/*
    // Force Single Coil
    else if(rxbuf[1] == 5)
    {
        control_module_do(rxbuf[0]-1, (rxbuf[2]<<8)|rxbuf[3], ((rxbuf[4]<<8)|rxbuf[5])==0xFF00?DO_ON:DO_OFF);
        txbuf[wbytes++] = rxbuf[2];
        txbuf[wbytes++] = rxbuf[3];
        txbuf[wbytes++] = rxbuf[4];
        txbuf[wbytes++] = rxbuf[5];
    }

    / Preset Single Register
    else if(rxbuf[1] == 6)
    {
        // 없음.
    }
*/
    crc16 = modbus_crc16(txbuf, wbytes);

    txbuf[wbytes++] = (crc16 & 0xFF00) >> 8;
    txbuf[wbytes++] =  crc16 & 0x00FF;

    writen(fd, (char *)txbuf, wbytes);

    return 0;
}

int make_modbus_frame(int index, unsigned char *rxbuff, int id, int cmd, int addr, int amount, unsigned char *wdata)
{
	int ret = 0;
    struct equip_conn_info *pConnInfo = conn_info[index];

	switch (pConnInfo->header)
	{
		case HEADER_UNKNOWN_TYPE:	ret = make_modbus_rtu_frame(rxbuff, id, cmd, addr, amount);			break;
		case HEADER_MODBUS_TCP:		ret = make_modbus_tcp_frame(rxbuff, id, cmd, addr, amount, wdata);	break;
	
	}

	return ret;
}

int make_modbus_rtu_frame(unsigned char *rxbuff, int id, int cmd, int addr, int amount)
{
	int wbytes=0;
    unsigned short crc16 = 0;

    rxbuff[wbytes++] = (unsigned char) id; /* 장비 번호 */
    rxbuff[wbytes++] = (unsigned char) cmd;
    rxbuff[wbytes++] = (unsigned char) ((addr & 0xFF00) >> 8);  /* start address hi */
    rxbuff[wbytes++] = (unsigned char)  (addr & 0x00FF);        /* start address low */
    rxbuff[wbytes++] = (unsigned char) ((amount & 0xFF00) >> 8);   /* word count hi */
    rxbuff[wbytes++] = (unsigned char)  (amount & 0x00FF);         /* word count low */
    crc16 = modbus_crc16(rxbuff, 6);
    rxbuff[wbytes++] = (unsigned char) ((crc16 & 0xFF00) >> 8);  /* crc hi */
    rxbuff[wbytes++] = (unsigned char)  (crc16 & 0x00FF);        /* crc low */

    return wbytes;
}

int make_modbus_tcp_frame(unsigned char *rxbuff, int id, int cmd, int addr, int amount, unsigned char *wdata)
{
    int wbytes=0;
    static unsigned short transaction_id=0;

    if(cmd != 0x10)
    {
        /* TCP Header */
        rxbuff[wbytes++] = (transaction_id & 0xFF00) >> 8;  // Transaction Identifier Hi
        rxbuff[wbytes++] =  transaction_id & 0x00FF;        // Transaction Identifier Low
        rxbuff[wbytes++] = 0x00;  // Protocol Identifier
        rxbuff[wbytes++] = 0x00;  // Protocol Identifier
        rxbuff[wbytes++] = 0x00;  // Length
        rxbuff[wbytes++] = 0x00;  // Length

        /* Request */
        rxbuff[wbytes++] = id;  // Unit Identifier
        rxbuff[wbytes++] = cmd;
        rxbuff[wbytes++] = (addr & 0xFF00) >> 8;
        rxbuff[wbytes++] =  addr & 0x00FF;
        rxbuff[wbytes++] = (amount & 0xFF00) >> 8;  /* word count hi  */
        rxbuff[wbytes++] =  amount & 0x00FF;        /* word count low */
    }
    else if(cmd == 0x10)
    {
        if(wdata == NULL)
            return 0;

        /* TCP Header */
        rxbuff[wbytes++] = (transaction_id & 0xFF00) >> 8;  // Transaction Identifier Hi
        rxbuff[wbytes++] =  transaction_id & 0x00FF;        // Transaction Identifier Low
        rxbuff[wbytes++] = 0x00;  // Protocol Identifier
        rxbuff[wbytes++] = 0x00;  // Protocol Identifier
        rxbuff[wbytes++] = 0x00;  // Length
        rxbuff[wbytes++] = 0x00;  // Length

        /* Request */
        rxbuff[wbytes++] = id;  // Unit Identifier
        rxbuff[wbytes++] = cmd;  // Function Code
        rxbuff[wbytes++] = (addr & 0xFF00) >> 8; // Address
        rxbuff[wbytes++] =  addr & 0x00FF;
        rxbuff[wbytes++] = (amount & 0xFF00) >> 8;  // Word Count
        rxbuff[wbytes++] =  amount & 0x00FF;
        rxbuff[wbytes++] = (amount*2) & 0x00FF;     // Byte Count
        wbytes += snprintf((char *)rxbuff+wbytes, amount*2, "%s", wdata);
    }

    rxbuff[4] = ((wbytes-6) & 0xFF00) >> 8;   // Length
    rxbuff[5] =  (wbytes-6) & 0x00FF;         // Length

    transaction_id++;

    return wbytes;
}

int read_modbus_frame_timeout(int index, int fd, unsigned char *rxbuff, time_t interval, int req_word_cnt)
{
	int ret = 0;
    struct equip_conn_info *pConnInfo = conn_info[index];

	switch (pConnInfo->header)
	{
		case HEADER_UNKNOWN_TYPE:	ret = read_modbus_rtu_frame_timeout(index, fd, rxbuff, interval, req_word_cnt);	break;
		case HEADER_MODBUS_TCP:		ret = read_modbus_tcp_frame_timeout(index, fd, rxbuff, interval, req_word_cnt);	break;	
	}

	return ret;
}
int read_modbus_rtu_frame_timeout(int index, int fd, unsigned char *rxbuff, time_t interval, int req_word_cnt)
{
    unsigned short crc16 = 0;
    struct equip_conn_info *pConnInfo = conn_info[index];


	if(readn_timeout(fd, (char *)rxbuff, 5, interval) != 5)
	{
		fileLog(WARNING, "[%s:%d] eseq=[%d] MODBUS Response Packet Header Recv Fail..\n", __FUNCTION__, __LINE__, pConnInfo->eseq);
		return -1;
	}

	if(readn_timeout(fd, (char *)rxbuff+5, rxbuff[2], 1) != rxbuff[2])
	{
		fileLog(WARNING, "[%s:%d] eseq=[%d] MODBUS Response Packet Body Recv Fail..\n", __FUNCTION__, __LINE__);
		return -1;
	}

	if(rxbuff[1] & 0x80)
	{
		modbus_exception(rxbuff[2]);
		return -1;
	}

	if(!(rxbuff[1] == 1 || rxbuff[1] == 2 || rxbuff[1] == 3 || rxbuff[1] == 4 || rxbuff[1] == 5 || rxbuff[1] == 6 || rxbuff[1] == 16))
	{
		fileLog(WARNING, "[%s:%d] eseq=[%d] MODBUS Response FUNCTION CODE: [%02X]\n", __FUNCTION__, __LINE__, pConnInfo->eseq, rxbuff[1]);
		return -1;
	}

	if(pConnInfo->id != (unsigned int)rxbuff[0])
	{
		fileLog(WARNING, "[%s:%d] eseq=[%d] MODBUS Response Device Id Error: Req[%02X] Res[%02X]\n", __FUNCTION__, __LINE__, pConnInfo->eseq, pConnInfo->id, rxbuff[0]);
		return -1;
	}

	if(rxbuff[1] == 0x03 || rxbuff[1] == 0x04)
	{
		if(req_word_cnt*2 != rxbuff[2])
		{
			fileLog(WARNING, "[%s:%d] eseq=[%d] MODBUS Response REQ amount[%d], RES amout[%d] Byte Count isn't same!!\n", __FUNCTION__, __LINE__, pConnInfo->eseq, req_word_cnt*2, rxbuff[2]);
			return -1;
		}
	}

	crc16 = modbus_crc16(rxbuff, rxbuff[2]+3);
	if(rxbuff[rxbuff[2]+3] != ((crc16 & 0xFF00) >> 8) || rxbuff[rxbuff[2]+4] !=  (crc16 & 0x00FF))
	{
		if(rxbuff[rxbuff[2]+3] != (crc16 & 0x00FF) || rxbuff[rxbuff[2]+4] != ((crc16 & 0xFF00) >> 8))
		{
			fileLog(WARNING, "[%s:%d] MODBUS Response CRC Check Fail.. Compute Crc=[%04X], Recv CRC=[%02X][%02X]\n", __FUNCTION__, __LINE__, crc16, rxbuff[rxbuff[2]+3], rxbuff[rxbuff[2]+4]);
			return -1;
		}
	}

    return 0;
}

int read_modbus_tcp_frame_timeout(int index, int fd, unsigned char *rxbuff, time_t interval, int req_word_cnt)
{
    int ret=0, rbytes=0, tmp=0;
    struct equip_conn_info *pConnInfo = conn_info[index];

    do {
		tmp = readn_timeout(fd, (char *)rxbuff, 6, interval);
        if(tmp != 6)
        {
            fileLog(WARNING, "[%s:%d] eseq=[%d] tmp=[%d] MODBUS TCP Response Packet Header Recv Fail..\n", __FUNCTION__, __LINE__, pConnInfo->eseq, tmp);
            ret = -1;
            break;
        }

        rbytes = (rxbuff[4] << 8) | rxbuff[5];
		tmp = readn_timeout(fd, (char *)rxbuff+6, rbytes, interval);
        if(tmp != rbytes)
        {
            fileLog(WARNING, "[%s:%d] eseq=[%d] tmp=[%d] rbytes=[%d] MODBUS TCP Respoonse Packet Body Recv Fail..\n", __FUNCTION__, __LINE__, pConnInfo->eseq, tmp, rbytes);
            ret = -1;
			break;
        }

		//아이디 체크
		if(pConnInfo->id != (unsigned int)rxbuff[6])
		{
			fileLog(WARNING, "[%s:%d] eseq=[%d] MODBUS TCP Response Frame Device Id Error: Req[%02X] Res[%02X]\n", __FUNCTION__, __LINE__, pConnInfo->eseq, pConnInfo->id, rxbuff[6]);
			ret = -1;
			break;
		}

		if(rxbuff[7] & 0x80)
		{
			modbus_exception(rxbuff[8]);
			return -1;
		}

		if(rxbuff[7] == 0x03 || rxbuff[7] == 0x04)
        {
            if(req_word_cnt*2 != rxbuff[8])
            {
                fileLog(WARNING, "[%s:%d] eseq=[%d] MODBUS TCP Response REQ amount[%d], RES amout[%d] Byte Count isn't same!!\n", __FUNCTION__, __LINE__, pConnInfo->eseq, req_word_cnt*2, rxbuff[8]);
                ret = -1;
                break;
            }
        }
    } while(0);

    return ret;
}
