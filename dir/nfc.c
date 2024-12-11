#include "func.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nfc/nfc.h>

// NFC 초기화
int nfc_initialize(NFCReader *reader) {
    nfc_init(&(reader->context));
    if (reader->context == NULL) {
        fprintf(stderr, "Unable to init libnfc.\n");
        return -1;
    }

    reader->device = nfc_open(reader->context, NULL);
    if (reader->device == NULL) {
        fprintf(stderr, "Unable to open NFC device.\n");
        nfc_exit(reader->context);
        return -1;
    }

    printf("NFC device opened successfully.\n");
    nfc_device_set_property_bool(reader->device, NP_ACTIVATE_FIELD, true);
    return 0;
}

// UID 배열을 문자열로 변환
void uid_to_string(const uint8_t *uid, size_t uid_length, char *tag) {
    for (size_t i = 0; i < uid_length; i++) {
        sprintf(tag + (i * 2), "%02X", uid[i]);
    }
    tag[uid_length * 2] = '\0';  // 문자열 끝에 NULL 추가
}

// NFC 태그 감지 및 UID 반환
int nfc_read_tag(NFCReader *reader, char *tag) {
    nfc_target target;

    // 태그 감지
    if (nfc_initiator_poll_target(
            reader->device,
            (const nfc_modulation[]){{NMT_ISO14443A, NBR_106}},  // ISO14443A 프로토콜
            1, 2, 2, &target) > 0) {
        printf("Tag detected!\n");

        if (target.nti.nai.abtUid != NULL) {
            // UID를 문자열로 변환
            uid_to_string(target.nti.nai.abtUid, target.nti.nai.szUidLen, tag);
            return 0;
        } else {
            fprintf(stderr, "No UID available on detected tag.\n");
            return -1;
        }
    }

    printf("No tag detected.\n");
    return -1;
}

// NFC 종료
void nfc_finalize(NFCReader *reader) {
    if (reader->device != NULL) {
        nfc_close(reader->device);
    }
    if (reader->context != NULL) {
        nfc_exit(reader->context);
    }
}
