#ifndef _CRCLIB_H_
#define _CRCLIB_H_ 1

extern unsigned short modbus_crc16(unsigned char *puchMsg, unsigned short usDataLen);
extern unsigned char two_complement_crc(unsigned char *buff, int len);
extern unsigned char crc_gen(unsigned char *buff, int length);
extern unsigned char exelusive_or_crc_gen(unsigned char *buff, int len);
extern unsigned char RawSumOneComplement(unsigned char *buff, int len);
extern unsigned char zbus_check_sum(unsigned char *buff, int len);
extern unsigned short itts2_crc16(unsigned char *ptr,unsigned short size);
extern unsigned char modbus_lrc(unsigned char *auchMsg, unsigned short usDataLen);

#endif /* _CRCLIB_H_ */
