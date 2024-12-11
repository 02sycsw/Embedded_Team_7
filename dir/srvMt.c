#include "func.h"

void setServoAngle(int angle);
// 서보 초기화
void initServo() {
    // PWM 모드 설정
    pinMode(SERVO_PIN, PWM_OUTPUT);

    // PWM 주파수 설정 (50Hz, 서보 모터의 기본 주파수)
    pwmSetMode(PWM_MODE_MS);
    pwmSetRange(1024);    // 1024로 설정하여 0~1024 범위의 값을 사용
    pwmSetClock(192);     // 50Hz로 설정 (19,200,000 Hz / 192 = 50Hz)

    // 서보를 초기 위치(0도)로 설정
    setServoAngle(0);
    printf("Servo initialized to 0 degrees.\n");
}

// 각도 설정 함수
void setServoAngle(int angle) {
    if (angle < 0 || angle > 180) {
        printf("Invalid angle: %d. Angle must be between 0 and 180.\n", angle);
        return;
    }

    // SG90 서보의 경우, 1ms에서 2ms 사이의 펄스를 사용 (0 ~ 1024)
    // 0도 -> 1ms, 180도 -> 2ms로 변환 (1ms는 0에 대응, 2ms는 1024에 대응)
    int pulseWidth = 1 + (angle * 1.0 / 180);  // 1ms ~ 2ms 사이로 변환 (0 ~ 1024 범위로 매핑)
    int pwmValue = (pulseWidth * 1024) / 2;  // 1ms~2ms를 0~1024로 변환
    pwmWrite(SERVO_PIN, pwmValue);

    printf("Angle set to: %d° (PWM Value: %d)\n", angle, pwmValue);
}

