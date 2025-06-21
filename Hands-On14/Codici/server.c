#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <errno.h>
#include <ctype.h>
#include "common.h"

pthread_mutex_t lock;

// Prototipi
void *client_handler(void *socket_desc);
void handle_command(int client_sock, char *cmd);
void upper_n(char *dest, const char *src, int n);

void *client_handler(void *socket_desc) {
    int sock = *(int*)socket_desc;
    printf("[SERVER] Nuova connessione accettata (socket %d)\n", sock);
    char buffer[1024];
    ssize_t read_size;

    while ((read_size = recv(sock, buffer, sizeof(buffer)-1, 0)) > 0) {
        buffer[read_size] = '\0';
        handle_command(sock, buffer);
    }
    printf("[SERVER] Disconnessione client (socket %d)\n", sock); // <--- AGGIUNGI QUESTA RIGA
    close(sock);
    free(socket_desc);
    return 0;
}

void upper_n(char *dest, const char *src, int n) {
    for (int i = 0; i < n && src[i]; i++)
        dest[i] = toupper((unsigned char)src[i]);
    dest[n] = '\0';
}

void handle_command(int client_sock, char *cmd) {
    char command[10];
    upper_n(command, cmd, 6);

    if (strncmp(command, "ADD", 3) == 0) {
        printf("[SERVER] Ricevuto comando ADD: %s\n", cmd + 4);
        add_operazione(client_sock, cmd + 4);
    } else if (strncmp(command, "DELETE", 6) == 0) {
        int id = atoi(cmd + 7);
        printf("[SERVER] Ricevuto comando DELETE: id=%d\n", id);
        delete_operazione(client_sock, id);
    } else if (strncmp(command, "UPDATE", 6) == 0) {
        printf("[SERVER] Ricevuto comando UPDATE: %s\n", cmd + 7);
        update_operazione(client_sock, cmd + 7);
    } else if (strncmp(command, "LIST", 4) == 0) {
        printf("[SERVER] Ricevuto comando LIST\n");
        list_operazioni(client_sock);
    } else {
        printf("[SERVER] Comando sconosciuto: %s\n", cmd);
        send(client_sock, "[SERVER] Comando sconosciuto\n", 30, 0);
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    pthread_t threads[MAX_THREADS];
    int thread_count = 0;

    pthread_mutex_init(&lock, NULL);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("[SERVER] In ascolto su porta %d...\n", PORT);

    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("accept failed");
            continue;
        }
        if (thread_count >= MAX_THREADS) {
            printf("[SERVER] Numero massimo di connessioni raggiunto.\n");
            close(new_socket);
            continue;
        }
        int *client_sock = malloc(sizeof(int));
        if (!client_sock) {
            perror("malloc failed");
            close(new_socket);
            continue;
        }
        *client_sock = new_socket;
        if (pthread_create(&threads[thread_count++], NULL, client_handler, (void*) client_sock) != 0) {
            perror("pthread_create failed");
            close(new_socket);
            free(client_sock);
            continue;
        }
    }

    for (int i = 0; i < thread_count; i++)
        pthread_join(threads[i], NULL);

    pthread_mutex_destroy(&lock);
    close(server_fd);
    return 0;
}