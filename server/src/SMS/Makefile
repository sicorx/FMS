include ../MakeVars

SRCS1 = sms.c
OBJS1 = sms.o

TARGET = SMS

LDFLAGS = -lpthread -lmysqlclient -L$(LIB_DIR) -lcommon -lmysqlclient -lnetsnmp

INCLUDE = -I$(SRC_DIR)/include -I/usr/include/mysql -I/usr/include/net-snmp

usage :
	@echo "make all      : 라이브러리 생성 및 실행파일 생성"
	@echo "make install  : 실행파일 bin 디렉토리로 복사"
	@echo "make clean    : 오브젝트 파일 삭제"

all : $(TARGET)


$(OBJS1) : $(SRCS1)
	$(CC) -c $< -o $@ $(INCLUDE) $(CFLAGS) $(LDFLAGS)

$(TARGET) : $(OBJS1)
	$(CC) -o $@ $(SRCS1) $(INCLUDE) $(CFLAGS) $(LDFLAGS)

install :
	$(INSTALL) $(INS_MOD) $(TARGET) $(BIN_DIR)

clean :
	@echo "making clean sms"; \
	$(RM) -f $(OBJS1) $(TARGET)
