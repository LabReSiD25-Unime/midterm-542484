#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

void print_help() {
    printf("Connesso al server.\n");
    printf("Comandi disponibili:\n");
    printf("  ADD    <id> <importo> <causale>   - Aggiunge una nuova operazione\n");
    printf("  DELETE <id>                       - Elimina l'operazione con id specificato\n");
    printf("  UPDATE <id> <importo> <causale>   - Aggiorna l'operazione con id specificato\n");
    printf("  LIST                              - Visualizza tutte le operazioni\n");
    printf("  QUIT                              - Chiude la connessione\n");
}

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[1024];
    char recvbuf[2048];
    int n;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock);
        exit(EXIT_FAILURE);
    }
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock);
        exit(EXIT_FAILURE);
    }

    print_help();

    while (1) {
        printf("> ");
        if (!fgets(buffer, sizeof(buffer), stdin))
            break;
        if (strncmp(buffer, "QUIT", 4) == 0)
            break;
        send(sock, buffer, strlen(buffer), 0);
        n = recv(sock, recvbuf, sizeof(recvbuf)-1, 0);
        if (n > 0) {
            recvbuf[n] = '\0';
            printf("%s", recvbuf);
        }
    }
    close(sock);
    return 0;
}