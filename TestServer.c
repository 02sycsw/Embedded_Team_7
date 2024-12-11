#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *response = "Ack from ";

    // 소켓 생성
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket Failure");
        exit(EXIT_FAILURE);
    } 

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 바인딩
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Binding Failure");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 클라이언트 연결 대기
    if (listen(server_fd, 3) < 0) {
        perror("Listening Failure");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server is waiting...\n");

    // 클라이언트 연결 수락
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("Accept Failure");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    read(new_socket, buffer, 1024);
    printf("Client Message: %s\n", buffer);

    send(new_socket, response, strlen(response), 0);
    printf("Send Response Complete\n");

    close(new_socket);
    close(server_fd);
    return 0;
}
