/***************************************************************
Made By Hong-SeungHyeon
Date: 2020-10-20(화)
***************************************************************/

#ifndef _LOG_H_
#define _LOG_H_ 1




void fileLog(int flag, const char *fmt, ... );
void DataMsg(register char *buffer, int len, int flag);
void err_exit(int flag, const char *fmt, ... );
void print_start_log(void);
void print_end_log(void);














#endif /* _LOG_H_ */
