// CN LAB 07 --- UDP SERVER.C


// Roll: 23105142007 Service Type: B d = 1


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 30007
#define ROLL "23105142007"

int main() {
    int s;
    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);
    char buf[2048], reply[2048];

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        perror("socket");
        return 1;
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind");
        close(s);
        return 1;
    }

    printf("Server running on port %d\n", PORT);
    fflush(stdout);

    while (1) {
        int n = recvfrom(s, buf, sizeof(buf)-1, 0,
                         (struct sockaddr *)&client, &len);
        if (n < 0) continue;
        buf[n] = '\0';

        char *p = strchr(buf, '#');
        if (!p) {
            strcpy(reply, "ERROR: Unauthorized client");
        } else {
            *p = '\0';
            char *msg = p + 1;

            printf("Client: %s:%d\n",
                   inet_ntoa(client.sin_addr), ntohs(client.sin_port));
            printf("Received message: %s\n", msg);

            if (strcmp(buf, ROLL) != 0) {
                strcpy(reply, "ERROR: Unauthorized client");
            } else if (strcmp(msg, "EXIT") == 0) {
                strcpy(reply, "Server shutting down");
                sendto(s, reply, strlen(reply), 0,
                       (struct sockaddr *)&client, len);
                printf("Sent response: %s\n", reply);
                break;
            } else {
                char op[10], extra[1800];
                int count, used = 0;
                long long a[500], sum = 0;
                char *q;

                if (sscanf(msg, "%9[^|]|%d|%1799[^\n]",
                           op, &count, extra) != 3 ||
                    count < 1 || count > 500) {
                    strcpy(reply, "ERROR: Use OP|n|integers");
                } else {
                    q = extra;
                    int valid = 1;
                    for (int i = 0; i < count; i++) {
                        char *end;
                        while (isspace((unsigned char)*q)) q++;
                        if (*q == '\0') { valid = 0; break; }
                        a[i] = strtoll(q, &end, 10);
                        if (end == q) { valid = 0; break; }
                        q = end;
                        sum += a[i];
                    }
                    while (isspace((unsigned char)*q)) q++;
                    if (!valid || *q != '\0') {
                        strcpy(reply, "ERROR: Invalid integer list");
                    } else if (!strcmp(op, "SUM")) {
                        snprintf(reply, sizeof(reply), "%lld", sum);
                    } else if (!strcmp(op, "AVG")) {
                        snprintf(reply, sizeof(reply), "%.2f",
                                 (double)sum/count);
                    } else if (!strcmp(op, "MAX") || !strcmp(op, "MIN")) {
                        long long result = a[0];
                        for (int i=1; i<count; i++) {
                            if (!strcmp(op, "MAX") && a[i] > result)
                                result = a[i];
                            if (!strcmp(op, "MIN") && a[i] < result)
                                result = a[i];
                        }
                        snprintf(reply, sizeof(reply), "%lld", result);
                    } else {
                        strcpy(reply, "ERROR: Use SUM, MAX, MIN, or AVG");
                    }
                }
            }
        }

        sendto(s, reply, strlen(reply), 0,
               (struct sockaddr *)&client, len);
        printf("Sent response: %s\n", reply);
    }

    close(s);
    return 0;
}
