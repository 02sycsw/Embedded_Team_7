#include "func.h"

int noteIndex = 0;
char tag[TAG_MAX_LEN];

int main() {
    pthread_t pthread[2];

    wiringPiSetup();
    wiringPiSetupGpio();

    initLCD(); // LCD 초기화
    initJoy();
    initBuzzer();

    if (pthread_create(&pthread[0], NULL, joystickHandler, NULL) != 0) {
        perror("Failed to create joystick thread");
        return 1;
    }
    if (pthread_create(&pthread[1], NULL, soundManager, NULL) != 0) {
        perror("Failed to create soundManager thread");
        return 1;
    }

     NFCReader reader;
    uint8_t uid[10];
    size_t uid_length;

    // NFC 초기화
    if (nfc_initialize(&reader) != 0) {
        return 1;
    }

    // 태그 감지 및 UID 출력
    while (1) {
        if (nfc_read_tag(&reader, tag) == 0) {
            printf("Tag UID as string: %s\n", tag);
            break;  // 태그를 성공적으로 읽으면 종료
        }

        sleep(1);  // 태그가 없으면 1초 대기 후 재시도
    }

    // NFC 종료
    nfc_finalize(&reader);
    
    //selectMusic();

    Queue queue;
    initQueue(&queue);
    

    // 큐 시뮬레이션
    while (1) {
        system("clear");      // 콘솔 화면 초기화
        displayQueue(&queue); // 큐 상태 출력

        // 입력과 비교
        checkStrikeZone(&queue, joyFlag);
        progressQueue(&queue);
        printf("flag = %d\n", joyFlag);
        usleep(DELAY); // 0.25초 대기
        noteIndex++;
        if (music[noteIndex -13] == -1 &&  noteIndex > 13) break;
    }
    pwmWrite(bzPin,0);
    pinMode(bzPin,INPUT); // 핀 해제
    usleep(100000);

    pthread_cancel(pthread[0]);
    pthread_cancel(pthread[1]);
    pthread_join(pthread[0], NULL);
    pthread_join(pthread[1], NULL);

    printf("Simulation complete.\n");
    printf("points : %d\n", point);

    int ret;
    displayPointOnLCD();
    if(point > 40) ret = system("/home/pi/embedded/proj/open");

    return 0;
}
