# 컴파일러 및 플래그 설정
CC = gcc
CFLAGS = -lwiringPi -lm -lpthread -lnfc

# 소스 파일 및 실행 파일 이름 설정
SRC = game.c joy.c queue.c spk.c nfc.c srvMt.c
TARGET = game

# 기본 빌드 규칙
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) -o $@ $^ $(CFLAGS)

# 클린 규칙 (생성된 파일 삭제)
clean:
	rm -f $(TARGET)
