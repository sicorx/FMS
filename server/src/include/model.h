#ifndef _MODEL_H_
#define _MODEL_H_ 1

//////////////////////////////header////////////////////////////
int make_dyrcu_com_frame_head(unsigned char *buff, int cpu_addr, int func, int ext_addr, int data_size);
//////////////////////////////header////////////////////////////

int communicate_modbus(int fd, unsigned char *buff, int index); //����, ������ �ѹ��� ��û�ϴ� ����϶� ���



//////////////////////////////UPS////////////////////////////
int communicate_HipulseU(int fd, unsigned char *buff, int index);
//////////////////////////////UPS////////////////////////////



////////////////////////////�׿��׽���////////////////////////////
int communicate_mst(int fd, unsigned char *buff, int index);
int communicate_nemo(int fd, unsigned char *buff, int index);
int communicate_ar(int fd, unsigned char *buff, int index);
int communicate_ar_dyrcu(int fd, unsigned char *buff, int index);
int communicate_ar_old(int fd, unsigned char *buff, int index);
int communicate_ar_question(int fd, unsigned char *buff, int index);
int communicate_ar_modbus(int fd, unsigned char *buff, int index);
int communicate_ar_modbus_dyrcu(int fd, unsigned char *buff, int index);
int communicate_svu(int fd, unsigned char *buff, int index);
int communicate_ddc_shinsung(int fd, unsigned char *buff, int index);
int communicate_ddc_hangul(int fd, unsigned char *buff, int index);
int communicate_ddc_hangul2(int fd, unsigned char *buff, int index);
int communicate_fac1000(int fd, unsigned char *buff, int index);
int communicate_seungil(int fd, unsigned char *buff, int index);
int communicate_seungil_sthc_mb_fw36(int fd, unsigned char *buff, int index);
int communicate_syskorea(int fd, unsigned char *buff, int index);
int communicate_by_crc2004(int fd, unsigned char *buff, int index);
int communicate_dy_ss2000(int fd, unsigned char *buff, int index);
int communicate_liebert_chtp(int fd, unsigned char *buff, int index);
int communicate_dx100(int fd, unsigned char *buff, int index);
int communicate_mex100(int fd, unsigned char *buff, int index);
int communicate_mst_hmm21(int fd, unsigned char *buff, int index);
int communicate_oc485(int fd, unsigned char *buff, int index);
int communicate_solid(int fd, unsigned char *buff, int index);
int communicate_sthav_mb_r1(int fd, unsigned char *buff, int index);
int communicate_rtu_ar_modbus(int fd, unsigned char *buff, int index);
int communicate_rtu_liebert_chtp(int fd, unsigned char *buff, int index);
int communicate_rtu_oc485(int fd, unsigned char *buff, int index);
int communicate_rtu_ar_hane09(int fd, unsigned char *buff, int index);
int communicate_rtu_ar_hane09_q(int fd, unsigned char *buff, int index);
////////////////////////////�׿��׽���////////////////////////////



////////////////////////////������////////////////////////////
int communicate_mpm330(int fd, unsigned char *buff, int index);
int communicate_mpm330a(int fd, unsigned char *buff, int index);
int communicate_rtu_mpm330a(int fd, unsigned char *buff, int index);
int communicate_rtu_gimaci(int fd, unsigned char *buff, int index);
int communicate_rtu_gimaciiplus(int fd, unsigned char *buff, int index);
int communicate_rtu_gipam115fi(int fd, unsigned char *buff, int index);
////////////////////////////������////////////////////////////



////////////////////////////STS////////////////////////////
int communicate_rtu_SuperSwitch3_STS(int fd, unsigned char *buff, int index);
int communicate_SuperSwitch3_STS(int fd, unsigned char *buff, int index);
////////////////////////////STS////////////////////////////



/////////////////////////////�½���//////////////////////////////
int communication_rtu_th(int fd, unsigned char *buff, int index);

int communicate_itts2(int fd, unsigned char *buff, int index);
int communicate_mes100(int fd, unsigned char *buff, int index);
int communication_innotube(int fd, unsigned char *buff, int index);
/////////////////////////////�½���//////////////////////////////



/////////////////////////////����////////////////////////////////
int communicate_km6053(int fd, unsigned char *buff, int index);
int communicate_rm6054(int fd, unsigned char *buff, int index);
int communicate_rtu_rtu5000(int fd, unsigned char *buff, int index);
/////////////////////////////����////////////////////////////////



/////////////////////////////������////////////////////////////////
int communicate_cimon(int fd, unsigned char *buff, int index);
/////////////////////////////������////////////////////////////////



/////////////////////////////����////////////////////////////////
int communicate_ttdm128(int fd, unsigned char *buff, int index);
int communicate_a_llm1(int fd, unsigned char *buff, int index);
int communication_rtu_ttdm128(int fd, unsigned char *buff, int index);
/////////////////////////////����////////////////////////////////



/////////////////////////////BMS////////////////////////////////
int communicate_btech(int fd, unsigned char *buff, int index);
/////////////////////////////BMS////////////////////////////////
#endif /* _MODEL_H_ */