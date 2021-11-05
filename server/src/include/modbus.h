#ifndef _MODBUS_H_
#define _MODBUS_H_ 1

void modbus_exception(char code);

void *modbus_Server(void *arg);
void *modbus_Client(void *arg);

int modbus_frame_check(unsigned char *buff);
int modbus_response_nak(int fd, int err_code, unsigned char *rxbuf);
int modbus_response_ack(int fd, unsigned char *rxbuf);

int make_modbus_frame(int index, unsigned char *rxbuff, int id, int cmd, int addr, int amount, unsigned char *wdata);
int make_modbus_rtu_frame(unsigned char *rxbuff, int id, int cmd, int addr, int amount);
int make_modbus_tcp_frame(unsigned char *rxbuff, int id, int cmd, int addr, int amount, unsigned char *wdata);

int read_modbus_frame_timeout(int index, int fd, unsigned char *rxbuff, time_t interval, int req_word_cnt);
int read_modbus_rtu_frame_timeout(int index, int fd, unsigned char *rxbuff, time_t interval, int req_word_cnt);
int read_modbus_tcp_frame_timeout(int index, int fd, unsigned char *rxbuff, time_t interval, int req_word_cnt);

#endif /* _MODBUS_H_ */
