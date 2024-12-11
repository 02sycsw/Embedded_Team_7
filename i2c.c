#include <nfc/nfc.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_DEVICE_COUNT 16
#define MAX_UID_LENGTH 10



int main(void) {
    nfc_context *context;
    nfc_device *device = NULL;
    nfc_target target;

    int tag = 0;

    // libnfc 초기화
    nfc_init(&context);
    if (context == NULL) {
        fprintf(stderr, "Unable to init libnfc.\n");
        return 1;
    }

    // NFC 장치 열기
    device = nfc_open(context, NULL);
    if (device == NULL) {
        fprintf(stderr, "Unable to open NFC device.\n");
        nfc_exit(context);
        return 1;
    }

    printf("NFC device opened successfully.\n");

    // 장치 정보 출력
    nfc_device_set_property_bool(device, NP_ACTIVATE_FIELD, true);
    printf("Polling for NFC tags...\n");

    while (tag == 0) {
        // NFC 태그 탐지
        if (nfc_initiator_poll_target(device, 
                                      (const nfc_modulation[]){{NMT_ISO14443A, NBR_106}},
                                      1, 2, 2, &target) > 0) {
            printf("Tag detected!\n");
            if (target.nti.nai.abtUid != NULL) {
                printf("UID: ");
                for (size_t i = 0; i < target.nti.nai.szUidLen; i++) {
                    printf("%02X ", target.nti.nai.abtUid[i]);
                }
                printf("\n");
                tag = 1;
            } else {
                printf("No UID available.\n");
            }
        } else {
            printf("No tag detected.\n");
        }

        // 약간의 대기
        sleep(1);
    }

    // NFC 장치 닫기
    nfc_close(device);
    nfc_exit(context);

    return 0;
}
