#include "include.h"

int readn(register int fd, register char *ptr, register int nbytes)
{
    int nleft, nread, nfound, maxfd;
    struct timeval timeout;
    fd_set rmask, mask;

    timeout.tv_sec  = TIMEOUT;
    timeout.tv_usec = 0L;

    FD_ZERO(&mask);
    FD_SET (fd, &mask);

    maxfd = fd;
    nleft = nbytes;

    while(nleft > 0)
    {
        rmask = mask;
        nfound = select(maxfd+1, &rmask, (fd_set *)0, (fd_set *)0, &timeout);

        if(nfound < 0)
        {
            if(errno == EINTR)
            {
                fileLog(WARNING, "interrupted system call\n");
                break;
            }

            fileLog(WARNING, "select[exit] %s\n", strerror(errno));
            FD_CLR(fd, &rmask);
            return -1;
        }

        if(nfound == 0)
        {
            fileLog(WARNING, "Read time out!!\n");
            break;
        }

        if(FD_ISSET(fd, &rmask))
        {
            nread = read(fd, ptr, nleft);
            if(nread < 0)
            {
                FD_CLR(fd, &rmask);
                return (nread);
            }
            else if(nread == 0)
                break;

#ifdef DEBUG
			DataMsg(ptr, nread, RECV);
#endif

            nleft -= nread;
            ptr   += nread;
        }
    }

    FD_CLR(fd, &rmask);
    return (nbytes - nleft);
}

int readn_timeout(register int fd, register char *ptr, register int nbytes, time_t interval)
{
    int nleft, nread, nfound, maxfd;
    struct timeval timeout;
    fd_set rmask, mask;

    timeout.tv_sec  = interval;
    timeout.tv_usec = 0L;

    FD_ZERO(&mask);
    FD_SET (fd, &mask);

    maxfd = fd;
    nleft = nbytes;

    while(nleft > 0)
    {
        rmask = mask;
        nfound = select(maxfd+1, &rmask, (fd_set *)0, (fd_set *)0, &timeout);

        if(nfound < 0)
        {
            if(errno == EINTR)
            {
                fileLog(WARNING, "interrupted system call\n");
                break;
            }
            fileLog(WARNING, "select[exit] %s\n", strerror(errno));
            FD_CLR(fd, &rmask);
            return -1;
        }

        if(nfound == 0)
        {
            //fileLog(WARNING, "Read time out!!\n");
            break;
        }

        if(FD_ISSET(fd, &rmask))
        {
            nread = read(fd, ptr, nleft);
            if(nread < 0)
            {
                FD_CLR(fd, &rmask);
                return (nread);
            }
            else if(nread == 0)
                break;

#ifdef DEBUG
			DataMsg(ptr, nread, RECV);
#endif

            nleft -= nread;
            ptr   += nread;
        }
    }

    FD_CLR(fd, &rmask);
    return (nbytes - nleft);
}

int read_dyrcu_com_frame_timeout(int fd, unsigned char *rxbuff, time_t interval)
{
    int ret=0, reply_size=0, read_size=0;
    unsigned short crc16=0;

    do {
        if(readn_timeout(fd, (char *)rxbuff, 6, interval) != 6)
        {
            fileLog(WARNING, "DYRCU Response Packet Header Recv Fail..\n");
            ret = -1;
            break;
        }

        reply_size = (rxbuff[4]<<8)|rxbuff[5];
        read_size  = reply_size + 2;

        if(readn_timeout(fd, (char *)rxbuff+6, read_size, interval) != read_size)
        {
            fileLog(WARNING, "DYRCU Response Packet Body Recv Fail..\n");
            ret = -1;
            break;
        }

        crc16 = modbus_crc16(rxbuff, reply_size+6);
        if(rxbuff[reply_size+6] != ((crc16 & 0xFF00) >> 8) ||
           rxbuff[reply_size+7] !=  (crc16 & 0x00FF))
        {
            fileLog(WARNING, "MODBUS Response Packet CRC Check Fail.. Compute CRC=[%04X] Recv CRC=[%02X][%02X]\n", crc16, rxbuff[reply_size+6], rxbuff[reply_size+7]);
            ret = -1;
        }
    } while(0);

    return ret;
}