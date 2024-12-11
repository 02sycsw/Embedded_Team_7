#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#define QUEUE_MAX_SIZE 16   // 큐의 최대 크기
#define STRIKE_ZONE_INDEX 2 // 스트라이크존 위치
#define DELAY 250000        // 0.25초 (마이크로초)
#define BUTTON_PIN 17       // 입력 버튼 핀 번호
#define DEBOUNCE_DELAY 10000 // 디바운싱 시간 (10ms)

#define LCD_ADDRESS 0x3e // LCD I2C 주소
#define RGB_ADDRESS 0x62 // RGB 백라이트 I2C 주소

int music[] = {3,3,3,0,2,0,1,1,0,2,2,0,3,3,0,3,3,0,3,3,0};

// 큐 구조체 정의
typedef struct {
    char data[QUEUE_MAX_SIZE];
    int front;
    int rear;
} Queue;

void initQueue(Queue *q);
void enqueue(Queue *q, int value);
void dequeue(Queue *q);
int progressQueue(Queue *q, int idx);
void displayQueue(Queue *q);
void *inputThread(void *arg);
void checkHit(Queue *q);

void lcdInit();
void lcdClear();
void lcdSetText(const char *text);
void lcdSetRGB(int r, int g, int b);

// 전역 변수
int input_flag = 0; // 입력 발생 플래그
int lcd_fd, rgb_fd; // LCD와 RGB 장치 파일 디스크립터

void initQueue(Queue *q) {
    q->front = 0;
    q->rear = 15;
    for (int i = 0; i < QUEUE_MAX_SIZE; i++) {
        q->data[i] = ' '; // 초기값은 공백
    }
}

// 도솔 L 레라 R 미시 U 파도 D
void enqueue(Queue *q, int value) {
    if (value == 1 || value == 5) q->data[q->rear] = 'L';
    else if (value == 2 || value == 6) q->data[q->rear] = 'R';
    else if (value == 3 || value == 7) q->data[q->rear] = 'U';
    else if (value == 4 || value == 8) q->data[q->rear] = 'D';
    else q->data[q->rear] = ' ';
}

void dequeue(Queue *q) {
    for (int i = 0; i < QUEUE_MAX_SIZE - 1; i++) {
        q->data[i] = q->data[i + 1];
    }
    q->data[QUEUE_MAX_SIZE - 1] = ' '; // 마지막 값을 공백으로
}

int progressQueue(Queue *q, int idx) {
    dequeue(q);
    enqueue(q, music[idx]);
    return ++idx;
}

void displayQueue(Queue *q) {
    char lcd_buffer[17] = {0}; // LCD에 출력할 16자 문자열 + NULL
    for (int i = 0; i < QUEUE_MAX_SIZE; i++) {
        lcd_buffer[i] = q->data[i];
    }

    lcdSetText(lcd_buffer); // LCD에 큐 상태 출력
    printf("%s\n", lcd_buffer); // 콘솔에도 출력

    // 스트라이크존 표시
    for (int i = 0; i < QUEUE_MAX_SIZE; i++) {
        if (i == STRIKE_ZONE_INDEX - 1 || i == STRIKE_ZONE_INDEX + 1) {
            printf("|");
        } else {
            printf(" ");
        }
    }
    printf("\n");
}

void checkHit(Queue *q) {
    char curNote = q->data[STRIKE_ZONE_INDEX];
    if (curNote != ' ' && input_flag) {
        printf("Hit: %c\n", curNote);
        lcdSetRGB(0, 255, 0); // 성공 시 초록색
        input_flag = 0; // 입력 플래그 초기화
    } else {
        lcdSetRGB(255, 0, 0); // 실패 시 빨간색
    }
}

void *inputThread(void *arg) {
    while (1) {
        int button_state = digitalRead(BUTTON_PIN); // 버튼 상태 읽기
        //printf("Button state: %d\n", button_state); // 버튼 상태 출력 (HIGH or LOW)

        if (button_state == HIGH) { // 버튼 눌림 감지 (풀다운 저항으로 HIGH 상태로 전환)
            input_flag = 1; // 입력 플래그 설정
            usleep(DEBOUNCE_DELAY); // 디바운싱 방지 (0.01초)
        }
    }
    return NULL;
}

// LCD 초기화
void lcdInit() {
    lcd_fd = wiringPiI2CSetup(LCD_ADDRESS);
    rgb_fd = wiringPiI2CSetup(RGB_ADDRESS);

    lcdClear();
    wiringPiI2CWriteReg8(lcd_fd, 0x80, 0x28); // Function set
    wiringPiI2CWriteReg8(lcd_fd, 0x80, 0x08); // Display off
    wiringPiI2CWriteReg8(lcd_fd, 0x80, 0x01); // Clear display
    wiringPiI2CWriteReg8(lcd_fd, 0x80, 0x06); // Entry mode set
    wiringPiI2CWriteReg8(lcd_fd, 0x80, 0x0C); // Display on
}

void lcdClear() {
    wiringPiI2CWriteReg8(lcd_fd, 0x80, 0x01); // Clear display
}

void lcdSetText(const char *text) {
    lcdClear();
    for (int i = 0; text[i] != '\0'; i++) {
        wiringPiI2CWriteReg8(lcd_fd, 0x40, text[i]);
    }
}

void lcdSetRGB(int r, int g, int b) {
    wiringPiI2CWriteReg8(rgb_fd, 0x00, 0x00); // Mode1
    wiringPiI2CWriteReg8(rgb_fd, 0x01, 0x00); // Mode2
    wiringPiI2CWriteReg8(rgb_fd, 0x08, 0xAA); // Output state
    wiringPiI2CWriteReg8(rgb_fd, 0x04, r);    // Red
    wiringPiI2CWriteReg8(rgb_fd, 0x03, g);    // Green
    wiringPiI2CWriteReg8(rgb_fd, 0x02, b);    // Blue
}

int main() {
    wiringPiSetup();
    wiringPiSetupGpio();
    pinMode(BUTTON_PIN, INPUT);
    pullUpDnControl(BUTTON_PIN, PUD_DOWN); // 풀다운 저항 설정

    lcdInit();
    lcdSetRGB(0, 0, 255); // 초기 LCD 색상 (파랑)

    Queue queue;
    initQueue(&queue);

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, inputThread, NULL); // 입력 스레드 시작

    int note_index = 0;

    while (1) {
        system("clear");      // 콘솔 화면 초기화
        displayQueue(&queue); // 큐 상태 출력
        checkHit(&queue);     // 입력과 큐의 현재 상태 확인

        note_index = progressQueue(&queue, note_index);

        if (note_index == QUEUE_MAX_SIZE + (sizeof(music) / sizeof(music[0]))) {
            break; // 종료조건
        }
        usleep(DELAY); // 0.25초 대기
    }

    printf("Simulation complete.\n");
    pthread_cancel(thread_id); // 스레드 종료
    pthread_join(thread_id, NULL); // 스레드 리소스 정리
    return 0;
}
