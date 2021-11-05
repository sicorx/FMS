#ifndef _READ_H_
#define _READ_H_ 1

int readn(register int fd, register char *ptr, register int nbytes);
int readn_timeout(register int fd, register char *ptr, register int nbytes, time_t interval);
int read_dyrcu_com_frame_timeout(int fd, unsigned char *rxbuff, time_t interval);

#endif /* _READ_H_ */