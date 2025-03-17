#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "172.20.10.5"  
#define PORT 8000
#define BUFFER_SIZE 200

struct pdu {
    char type;
    char data[BUFFER_SIZE];
};

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    struct pdu cpdu;
    char filename[BUFFER_SIZE];

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    while (1) {
        printf("Enter filename to download ('exit' to quit): ");
        scanf("%s", filename);

        if (strcmp(filename, "exit") == 0) {
            printf("Exiting \n");
            break;
        }

        // Send filename 
        cpdu.type = 'C';
        strcpy(cpdu.data, filename);
        sendto(sockfd, &cpdu, sizeof(cpdu), 0,
               (struct sockaddr*)&server_addr, sizeof(server_addr));

        // Open file 
        FILE *file = fopen(filename, "wb");
        if (!file) {
            perror("Error opening file");
            continue;
        }

        // Receive file data
        while (1) {
            int n = recvfrom(sockfd, &cpdu, sizeof(cpdu), 0, NULL, NULL);
            
            if (cpdu.type == 'F') {
                printf("Download complete: %s\n", filename);
                break;
            } else if (cpdu.type == 'E') {
                printf("Error: %s\n", cpdu.data);
                break;
            }

            fwrite(cpdu.data, 1, n - 1, file);
        }

        fclose(file);
    }

    close(sockfd);
    return 0;
}
