#ifndef _DBSET_H_
#define _DBSET_H_ 1

unsigned short get_ushort_value(unsigned char first, unsigned char second);
short get_short_value(unsigned char first, unsigned char second);
short get_short_value2(unsigned char first, unsigned char second);

int get_int_value2(unsigned char first, unsigned char second);
unsigned int get_uint_value4(unsigned char first, unsigned char second, unsigned char third, unsigned char fourth);
int get_int_value4(unsigned char first, unsigned char second, unsigned char third, unsigned char fourth);

float get_float_value2(unsigned char first, unsigned char second);
float get_float_value3(unsigned char first, unsigned char second, unsigned char third);
float get_float_value4(unsigned char first, unsigned char second, unsigned char third, unsigned char fourth);
float get_superswitch3_fvalue(unsigned char first, unsigned char second, unsigned char third, unsigned char fourth);

void set_module_status(unsigned char *buff, int index, int com_state);

#endif /* _DBSET_H_ */