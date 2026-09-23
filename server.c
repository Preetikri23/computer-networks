#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 5);

    printf("Server waiting for clients...\n");

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&address,
                           (socklen_t *)&addrlen);

        if (fork() == 0) {
            close(server_fd);

            read(client_fd, buffer, sizeof(buffer));
            buffer[1023] = '\0';

            FILE *file = fopen("data.txt", "r");
            char line[256];
            char response[256] = "Record Not Found";
            char id[20];

            sscanf(buffer, "%s", id);

            while (fgets(line, sizeof(line), file)) {
                if (strncmp(line, id, strlen(id)) == 0) {
                    snprintf(response, sizeof(response),
                             "Record Found : %s", line);
                    break;
                }
            }

            fclose(file);

            write(client_fd, response, strlen(response));
            close(client_fd);
            exit(0);
        }

        close(client_fd);
    }

    close(server_fd);
    return 0;
}
