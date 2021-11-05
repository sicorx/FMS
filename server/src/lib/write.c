#include "include.h"

int writen(register int fd, register char *ptr, register int nbytes)
{
    int nleft, nwritten, nfound, maxfd;
    struct timeval timeout;
    fd_set wmask, mask;

    timeout.tv_sec  = TIMEOUT;
    timeout.tv_usec = 0L;

    FD_ZERO(&mask);
    FD_SET (fd, &mask);

    maxfd = fd;
    nleft = nbytes;

    while(nleft > 0)
    {
        wmask = mask;
        nfound = select(maxfd+1, (fd_set *)0, &wmask, (fd_set *)0, &timeout);

        if(nfound < 0)
        {
            if(errno == EINTR)
            {
                fileLog(WARNING, "interrupted system call\n");
                break;
            }
            fileLog(WARNING, "select[exit] %s\n", strerror(errno));
            FD_CLR(fd, &wmask);
            return -1;
        }

        if(nfound == 0)
        {
            fileLog(WARNING, "Write time out!!\n");
            break;
        }

        if(FD_ISSET(fd, &wmask))
        {
            nwritten = write(fd, ptr, nleft);
            if(nwritten <= 0)
            {
                return (nwritten);
            }

#ifdef DEBUG
			DataMsg((char*)ptr, nwritten, SEND);
#endif

            nleft -= nwritten;
            ptr   += nwritten;
        }
    }

    return (nbytes - nleft);
}

int sendto_module(int fd, unsigned char *buff, int length)
{
    if(writen(fd, (char *)buff, length) != length)
    {
        fileLog(CRITICAL, "[%s:%d] REQUEST Packet Send Fail..\n", __FUNCTION__, __LINE__);
        return -1;
    }

    return 0;
}