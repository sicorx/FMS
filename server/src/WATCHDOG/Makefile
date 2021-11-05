include ../MakeVars

SRCS1 = watchdog.c
OBJS1 = watchdog.o

TARGET1 = WATCHDOG

#LDFLAGS += -L$(SRC_DIR)/lib -lcommon -lpthread -lm

#INCLUDE = -I$(SRC_DIR)/include

usage :
	@echo "make all      : 라이브러리 생성 및 실행파일 생성"
	@echo "make install  : 실행파일 bin 디렉토리로 복사"
	@echo "make clean    : 오브젝트 파일 삭제"

all : $(TARGET1)

$(OBJS1) : $(SRCS1)
	$(CC) -c $< -o $@ $(INCLUDE) $(CFLAGS)

$(TARGET1) : $(OBJS1)
	$(CC) -o $@ $(SRCS1) $(INCLUDE) $(CFLAGS) $(LDFLAGS)

install :
	$(INSTALL) $(INS_MOD) $(TARGET1) $(BIN_DIR)
	
clean :
	@echo "making clean WATCHDOG"; \
	$(RM) -f $(OBJS1) $(TARGET1)