#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8000
#define BUFFER_SIZE 200  

struct pdu {
    char type;
    char data[BUFFER_SIZE];
};

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    struct pdu spdu;
    char filename[BUFFER_SIZE];
    
    //  UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // bind socket 
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port %d...\n", PORT);

    while (1) {
        // Receive filename request
        int n = recvfrom(sockfd, &spdu, sizeof(spdu), 0, 
                         (struct sockaddr*)&client_addr, &addr_len);
        spdu.data[n - 1] = '\0';  
        printf("Client file: %s\n", spdu.data);

        // Open file
        FILE *file = fopen(spdu.data, "rb");
        if (file == NULL) {
            printf("File not found: %s\n", spdu.data);
            
            // ERROR PDU
            spdu.type = 'E';
            strcpy(spdu.data, "File not found");
            sendto(sockfd, &spdu, sizeof(spdu), 0, 
                   (struct sockaddr*)&client_addr, addr_len);
            continue;
        }

        // Read and send file 
        while (1) {
            int bytes_read = fread(spdu.data, 1, BUFFER_SIZE, file);
            if (bytes_read <= 0)
                break;

            spdu.type = 'D';  
            sendto(sockfd, &spdu, bytes_read + 1, 0, 
                   (struct sockaddr*)&client_addr, addr_len);
        }

        // FINAL PDU
        spdu.type = 'F';
        memset(spdu.data, 0, BUFFER_SIZE);
        sendto(sockfd, &spdu, sizeof(spdu), 0, 
               (struct sockaddr*)&client_addr, addr_len);

        printf("File %s sent successfully.\n", spdu.data);
        fclose(file);
    }

    close(sockfd);
    return 0;
}
