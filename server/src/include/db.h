/***************************************************************
Made By Hong-SeungHyeon
Date: 2020-10-20(화)
***************************************************************/

#ifndef _DB_H_
#define _DB_H_ 1








int mysql_con_init(void);

int get_equip_count(void);
int get_equip_config(void);
int reload_equip_config(int index, int eseq);

void load_ai_conf(int index);
void load_di_conf(int index);
int mysqlinsertData(int eseq, int tseq, float value);
int mysqlUpdateTag(int eseq, int tseq, float value);
int alarm_insert(int flag, int index, int adio, int seq, int alarm_grade);











#endif /* _DB_H_ */
