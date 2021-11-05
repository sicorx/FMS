#ifndef _WRITE_H_
#define _WRITE_H_ 1

int writen(register int fd, register char *ptr, register int nbytes);
int sendto_module(int fd, unsigned char *buff, int length);

#endif /* _WRITE_H_ */
