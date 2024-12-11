#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char *message = "Message from client raspberryPi";
    char buffer[1024] = {0};

    // 소켓 생성
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Creat Socket Failure\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 서버 IP 주소 설정 (여기에 서버의 IP 주소를 입력하세요)
    if (inet_pton(AF_INET, "서버_IP주소", &serv_addr.sin_addr) <= 0) {
        printf("wrong address or translate address failure\n");
        return -1;
    }

    // 서버에 연결
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("connect server failure\n");
        return -1;
    }

    send(sock, message, strlen(message), 0);
    printf("Send Message: %s\n", message);

    read(sock, buffer, 1024);
    printf("Server Response: %s\n", buffer);

    close(sock);
    return 0;
}
