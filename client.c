#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock;
    struct sockaddr_in server;
    char data[1024];
    char id[20];
    char response[256];

    FILE *fp = fopen("data.txt", "r");

    if (fp == NULL) {
        perror("data.txt");
        return 1;
    }

    int len = fread(data, 1, sizeof(data) - 1, fp);
    data[len] = '\0';
    fclose(fp);

    sock = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (struct sockaddr *)&server, sizeof(server));

    printf("Enter Student ID to search : ");
    scanf("%s", id);

    write(sock, id, sizeof(id));
    write(sock, data, sizeof(data));

    int n = read(sock, response, sizeof(response) - 1);
    response[n] = '\0';

    printf("Server Response : %s\n", response);

    close(sock);

    return 0;
}
