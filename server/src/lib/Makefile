################################################################
#Made By Hong-SeungHyeon
#Date: 2020-10-13(화)
################################################################

include ../MakeVars

SRCS1 = db.c
OBJS1 = db.o
SRCS2 = log.c
OBJS2 = log.o
SRCS3 = common.c
OBJS3 = common.o
SRCS4 = crclib.c
OBJS4 = crclib.o
SRCS5 = modbus.c
OBJS5 = modbus.o
SRCS6 = protocol_recv.c
OBJS6 = protocol_recv.o
SRCS7 = protocol_send.c
OBJS7 = protocol_send.o

SRCS8 = write.c
OBJS8 = write.o
SRCS9 = read.c
OBJS9 = read.o
SRCS10 = model.c
OBJS10 = model.o
SRCS11 = dbset.c
OBJS11 = dbset.o
SRCS12 = ping.c
OBJS12 = ping.o
SRCS13 = snmp.c
OBJS13 = snmp.o

TARGETS = $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4) $(OBJS5) $(OBJS6) $(OBJS7) $(OBJS8) $(OBJS9) $(OBJS10) $(OBJS11) $(OBJS12) $(OBJS13)

INCLUDE = -I../include -I/usr/include/mysql
INCLUDE += -Wdate-time -D_FORTIFY_SOURCE=2 -I/usr/include/net-snmp/

TARGETLIB = libcommon.a

usage :
	@echo "make all      : 공용 라이브러리 컴파일"
	@echo "make clean    : 오브젝트 파일 삭제"
	
all : $(TARGETS) library

$(OBJS1) : $(SRCS1)
	$(CC) -c $< -o $@ $(INCLUDE) $(CFLAGS)

$(OBJS2) : $(SRCS2)
	$(CC) -c $< -o $@ $(INCLUDE) $(CFLAGS)
	
$(OBJS3) : $(SRCS3)
	$(CC) -c $< -o $@ $(INCLUDE) $(CFLAGS)

$(OBJS4) : $(SRCS4)
	$(CC) -c $< -o $@ $(INCLUDE) $(CFLAGS)
	
$(OBJS5) : $(SRCS5)
	$(CC) -c $< -o $@ $(INCLUDE) $(CFLAGS)
	
$(OBJS6) : $(SRCS6)
	$(CC) -c $< -o $@ $(INCLUDE) $(CFLAGS)
	
$(OBJS7) : $(SRCS7)
	$(CC) -c $< -o $@ $(INCLUDE) $(CFLAGS)

$(OBJS8) : $(SRCS8)
	$(CC) -c $< -o $@ $(INCLUDE) $(CFLAGS)

$(OBJS9) : $(SRCS9)
	$(CC) -c $< -o $@ $(INCLUDE) $(CFLAGS)

$(OBJS10) : $(SRCS10)
	$(CC) -c $< -o $@ $(INCLUDE) $(CFLAGS)
	
$(OBJS11) : $(SRCS11)
	$(CC) -c $< -o $@ $(INCLUDE) $(CFLAGS)
	
$(OBJS12) : $(SRCS12)
	$(CC) -c $< -o $@ $(INCLUDE) $(CFLAGS)

$(OBJS13) : $(SRCS13)
	$(CC) -c $< -o $@ $(INCLUDE) $(CFLAGS)

library : $(TARGETS)
	$(AR) rcv $(TARGETLIB) $(TARGETS)
	
clean :
	$(RM) -f $(TARGETS)