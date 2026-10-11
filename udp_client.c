// CN LAB 07 --- UDP CLIENT.C

// Roll: 23105142007 Service Type: B d = 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 30007

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server;
    char input[2048], message[2048], reply[2048];

    if (argc != 3) {
        printf("Usage: %s <server_ip> <roll>\n", argv[0]);
        return 1;
    }

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if (inet_pton(AF_INET, argv[1], &server.sin_addr) != 1) {
        printf("Invalid server IP address\n");
        close(sock);
        return 1;
    }

    printf("UDP client started. Type SUM|5|1 2 3 4 5 or EXIT\n");

    while (1) {
        printf("> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL)
            strcpy(input, "EXIT");

        input[strcspn(input, "\r\n")] = '\0';

        if (snprintf(message, sizeof(message), "%s#%s",
                     argv[2], input) >= (int)sizeof(message)) {
            printf("Input too long\n");
            continue;
        }

        if (sendto(sock, message, strlen(message), 0,
                   (struct sockaddr *)&server, sizeof(server)) < 0) {
            perror("sendto");
            break;
        }

        int n = recvfrom(sock, reply, sizeof(reply)-1, 0, NULL, NULL);
        if (n < 0) {
            perror("recvfrom");
            break;
        }
        reply[n] = '\0';
        printf("Server response: %s\n", reply);

        if (strcmp(input, "EXIT") == 0)
            break;
    }

    close(sock);
    return 0;
}
