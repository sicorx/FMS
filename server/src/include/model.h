#ifndef _MODEL_H_
#define _MODEL_H_ 1

//////////////////////////////header////////////////////////////
int make_dyrcu_com_frame_head(unsigned char *buff, int cpu_addr, int func, int ext_addr, int data_size);
//////////////////////////////header////////////////////////////

int communicate_modbus(int fd, unsigned char *buff, int index); //공통, 모드버스 한번만 요청하는 장비일때 사용



//////////////////////////////UPS////////////////////////////
int communicate_HipulseU(int fd, unsigned char *buff, int index);
//////////////////////////////UPS////////////////////////////



////////////////////////////항온항습기////////////////////////////
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
////////////////////////////항온항습기////////////////////////////



////////////////////////////분전반////////////////////////////
int communicate_mpm330(int fd, unsigned char *buff, int index);
int communicate_mpm330a(int fd, unsigned char *buff, int index);
int communicate_rtu_mpm330a(int fd, unsigned char *buff, int index);
int communicate_rtu_gimaci(int fd, unsigned char *buff, int index);
int communicate_rtu_gimaciiplus(int fd, unsigned char *buff, int index);
int communicate_rtu_gipam115fi(int fd, unsigned char *buff, int index);
int communicate_accura3700(int fd, unsigned char *buff, int index);
int communicate_accura2300s_3p1(int fd, unsigned char *buff, int index);
int communicate_accura2300s_3p41(int fd, unsigned char *buff, int index);
int communicate_accura2300s_3p3_1p3f18(int fd, unsigned char *buff, int index);
int communicate_accura3300e(int fd, unsigned char *buff, int index);
////////////////////////////분전반////////////////////////////



////////////////////////////STS////////////////////////////
int communicate_rtu_SuperSwitch3_STS(int fd, unsigned char *buff, int index);
int communicate_SuperSwitch3_STS(int fd, unsigned char *buff, int index);
////////////////////////////STS////////////////////////////



/////////////////////////////온습도//////////////////////////////
int communication_rtu_th(int fd, unsigned char *buff, int index);

int communicate_itts2(int fd, unsigned char *buff, int index);
int communicate_mes100(int fd, unsigned char *buff, int index);
int communication_innotube(int fd, unsigned char *buff, int index);
/////////////////////////////온습도//////////////////////////////



/////////////////////////////접점////////////////////////////////
int communicate_rm6054(int fd, unsigned char *buff, int index);
int communicate_rtu_rtu5000(int fd, unsigned char *buff, int index);
/////////////////////////////접점////////////////////////////////






/////////////////////////////누수////////////////////////////////
int communicate_ttdm128(int fd, unsigned char *buff, int index);
int communicate_a_llm1(int fd, unsigned char *buff, int index);
int communication_rtu_ttdm128(int fd, unsigned char *buff, int index);
/////////////////////////////누수////////////////////////////////



/////////////////////////////BMS////////////////////////////////
int communicate_btech(int fd, unsigned char *buff, int index);
/////////////////////////////BMS////////////////////////////////


/////////////////////////////ALTS////////////////////////////////
int communicate_iljin_alts(int fd, unsigned char *buff, int index);
/////////////////////////////ALTS////////////////////////////////


/////////////////////////////수배전반////////////////////////////////
int communicate_pac100(int fd, unsigned char *buff, int index);
/////////////////////////////수배전반////////////////////////////////


/////////////////////////////계전기////////////////////////////////
int communicate_cimon(int fd, unsigned char *buff, int index);
int communicate_gdr(int fd, unsigned char *buff, int index);
/////////////////////////////계전기////////////////////////////////

/////////////////////////////지열////////////////////////////////
int communicate_exp40_ttf_dc(int fd, unsigned char *buff, int index);
/////////////////////////////지열////////////////////////////////
#endif /* _MODEL_H_ */