#ifndef MYHEADER_H
#define MYHEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <math.h>
#include <nfc/nfc.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <softPwm.h>
#include <wiringSerial.h>

#define QUEUE_MAX_SIZE 16   // 큐의 최대 크기
#define STRIKE_ZONE_INDEX 2 // 스트라이크존 위치
#define DELAY 150000       // 0.25초 (마이크로초)
#define LCD_ADDR 0x3e // LCD I2C 주소
#define RGB_ADDR 0x62 // RGB 백라이트 I2C 주소

#define BASE_FREQUENCY 261.6256 // C4(Do)의 주파수
#define WHOLE_TONE_STEP 1.122462 // 2^(1/6), 한 음 비율
#define bzPin 18

#define MAX_DEVICE_COUNT 16
#define MAX_UID_LENGTH 10

#define SERVO_PIN 19

#define TAG_MAX_LEN 64

// 큐 구조체 정의
typedef struct {
    char data[QUEUE_MAX_SIZE];
    int front;
    int rear;
} Queue;

typedef struct {
    nfc_context *context;
    nfc_device *device;
} NFCReader;

// 함수 선언
int nfc_initialize(NFCReader *reader);
int nfc_read_tag(NFCReader *reader, char *tag);
void nfc_finalize(NFCReader *reader);

void initQueue(Queue *q);
void enqueue(Queue *q, int value);
void dequeue(Queue *q);
void progressQueue(Queue *q);
void displayQueue(Queue *q);
void checkStrikeZone(Queue *q, int joyFlag);
void displayPointOnLCD();

void initLCD();
void printToLCD(const char *line1, const char *line2);
void setLCDColor(int r, int g, int b);

int initJoy();
int joystick();
void *joystickHandler(void *arg);

void initBuzzer();
void hitSound();
void missSound();
void stopSound();
void* soundManager(void* arg);

void initServo();
void setServoAngle(int angle);

void selectMusic();

extern int joyFlag;
extern int hitFlag;
extern int curNote;
extern int noteIndex;
extern int point;
extern int music[];
extern char tag[TAG_MAX_LEN];

#endif // MYHEADER_H
