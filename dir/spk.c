#include "func.h"

// 부저 초기화
void initBuzzer() {
    pinMode(bzPin, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetRange(100);
    pwmSetClock(192000/261.6256);
    pwmWrite(bzPin, 0); // 소리 끔
}

// 부저 음성 출력
void hitSound() {
    if (curNote == 0) {stopSound(); return;}
    double frequency = BASE_FREQUENCY * pow(WHOLE_TONE_STEP, curNote -1);
    printf("%lf",frequency);
    pwmSetClock(192000 / frequency);
    pwmWrite(bzPin, 50);
}

void stopSound() {
    pwmWrite(bzPin, 0); // 소리 끔
}

// 사운드 관리 메소드
void* soundManager(void* arg) {
    while (1) {
        if (hitFlag == 1) {
            // 정답 음 출력
            hitSound();
        } else if (hitFlag == 2) {
            // 실패 음 출력
            stopSound();
        } else {
            // 소리 멈춤
            stopSound();
        }
        usleep(37500); // 10ms 대기 (너무 빠른 반복 방지)
    }

    return NULL;
}
