#include "func.h"

int music[] = {3,3,3,3,0,2,2,0,1,1,1,0,2,2,2,0,3,3,3,0,3,3,3,0,3,3,3,0,0,0,0,2,2,2,
0,2,2,2,0,2,2,2,0,0,0,0,3,3,3,0,5,5,5,0,5,5,5,0,0,0,0,0,3,3,3,3,0,2,2,0,1,1,1,
0,2,2,2,0,3,3,3,0,3,3,3,0,3,3,3,0,0,0,2,2,2,0,2,2,2,0,3,3,3,3,0,2,2,0,1,1,1,1,1,1,1,0,-1};

int lcd_fd, rgb_fd;
int hitFlag;
int curNote;
int point = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

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

void progressQueue(Queue *q) {
    dequeue(q);
    enqueue(q, music[noteIndex]);
}

void displayQueue(Queue *q) {
    char line1[QUEUE_MAX_SIZE] = {0};
    char line2[QUEUE_MAX_SIZE] = {0};

    for (int i = 0; i < QUEUE_MAX_SIZE; i++) {
        line1[i] = q->data[i];
        if (i == STRIKE_ZONE_INDEX - 1 || i == STRIKE_ZONE_INDEX + 1) {
            line2[i] = '|';
        } else {
            line2[i] = ' ';
        }
    }

    printf("%s\n", line1);
    printf("%s\n", line2);

    // LCD에 출력
    printToLCD(line1, line2);
}


// LCD 초기화
void initLCD() {
    lcd_fd = wiringPiI2CSetup(LCD_ADDR);
    rgb_fd = wiringPiI2CSetup(RGB_ADDR);

    if (lcd_fd < 0 || rgb_fd < 0) {
        printf("Failed to initialize I2C.\n");
        exit(1);
    }

    // LCD 초기화 명령
    wiringPiI2CWriteReg8(lcd_fd, 0x80, 0x01); // Clear display
    usleep(2000);
    wiringPiI2CWriteReg8(lcd_fd, 0x80, 0x0C); // Display on, no cursor
    wiringPiI2CWriteReg8(lcd_fd, 0x80, 0x28); // 2 lines
    usleep(2000);

    // RGB 초기화
    wiringPiI2CWriteReg8(rgb_fd, 0x00, 0x00);
    wiringPiI2CWriteReg8(rgb_fd, 0x01, 0x00);
    wiringPiI2CWriteReg8(rgb_fd, 0x08, 0xAA);

    // 기본 배경색 설정 (예: 파란색)
    wiringPiI2CWriteReg8(rgb_fd, 0x04, 0x00); // Red
    wiringPiI2CWriteReg8(rgb_fd, 0x03, 0x00); // Green
    wiringPiI2CWriteReg8(rgb_fd, 0x02, 0xFF); // Blue
}

// LCD에 문자열 출력
void printToLCD(const char *line1, const char *line2) {
    wiringPiI2CWriteReg8(lcd_fd, 0x80, 0x01); // Clear display
    usleep(2000);

    wiringPiI2CWriteReg8(lcd_fd, 0x80, 0x80); // 첫 번째 줄
    for (int i = 0; line1[i] != '\0'; i++) {
        wiringPiI2CWriteReg8(lcd_fd, 0x40, line1[i]);
    }

    wiringPiI2CWriteReg8(lcd_fd, 0x80, 0xC0); // 두 번째 줄
    for (int i = 0; line2[i] != '\0'; i++) {
        wiringPiI2CWriteReg8(lcd_fd, 0x40, line2[i]);
    }
}


void setLCDColor(int r, int g, int b) {
    wiringPiI2CWriteReg8(rgb_fd, 0x04, r); // Red
    wiringPiI2CWriteReg8(rgb_fd, 0x03, g); // Green
    wiringPiI2CWriteReg8(rgb_fd, 0x02, b); // Blue
}

void checkStrikeZone(Queue *q, int joyFlag) {
    pthread_mutex_lock(&lock); // mutex 잠금

    char strikeChar = q->data[STRIKE_ZONE_INDEX];
    int strikeValue = (int)strikeChar;

    if (joyFlag == strikeValue) {
        setLCDColor(0, 255, 0); // 초록색 (성공)
        hitFlag = 1;
        curNote = music[noteIndex - 13];
        point++;
    }
    else if (joyFlag != 20 && joyFlag != strikeValue) {
        setLCDColor(255, 0, 0); // 빨간색 (실패)
        hitFlag = 0;
        point--;
    }
    else {
        setLCDColor(0, 0, 255); // 파랑 idle
        hitFlag = 0;
    }

    pthread_mutex_unlock(&lock); // mutex 해제
}


void displayPointOnLCD() {
    char line1[16];  // LCD의 첫 번째 줄 (최대 16자)
    char line2[16];  // LCD의 두 번째 줄 (최대 16자)

    // 첫 번째 줄에 "Score: "를 출력
    sprintf(line1, "Score: %d", point);

    // 두 번째 줄은 비워두거나, 추가 메시지를 넣을 수 있습니다.
    sprintf(line2, "Keep Going!");

    // LCD에 출력
    printToLCD(line1, line2);
}
