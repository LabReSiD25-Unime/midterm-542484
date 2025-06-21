#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include "common.h"

Operazione operazioni[MAX_OP];
int num_operazioni = 0;
extern pthread_mutex_t lock;

void add_operazione(int client_sock, char *args) {
    pthread_mutex_lock(&lock);
    if (num_operazioni >= MAX_OP) {
        send(client_sock, "[SERVER] Archivio pieno\n", 25, 0);
    } else {
        Operazione op;
        int ret = sscanf(args, "%d %f %99s", &op.id, &op.importo, op.causale);
        if (ret != 3) {
            send(client_sock, "[SERVER] Formato ADD non valido\n", 32, 0);
        } else {
            operazioni[num_operazioni++] = op;
            send(client_sock, "[SERVER] Operazione aggiunta\n", 30, 0);
        }
    }
    pthread_mutex_unlock(&lock);
}

void delete_operazione(int client_sock, int id) {
    pthread_mutex_lock(&lock);
    int found = 0;
    for (int i = 0; i < num_operazioni; i++) {
        if (operazioni[i].id == id) {
            operazioni[i] = operazioni[--num_operazioni];
            found = 1;
            break;
        }
    }
    if (found)
        send(client_sock, "[SERVER] Operazione rimossa\n",30,0);
    else
        send(client_sock, "[SERVER] ID non trovato\n", 25, 0);
    pthread_mutex_unlock(&lock);
}

void update_operazione(int client_sock, char *args) {
    pthread_mutex_lock(&lock);
    int id;
    float importo;
    char causale[100];
    int ret = sscanf(args, "%d %f %99s", &id, &importo, causale);
    int found = 0;
    if (ret != 3) {
        send(client_sock, "[SERVER] Formato UPDATE non valido\n", 34, 0);
    } else {
        for (int i = 0; i < num_operazioni; i++) {
            if (operazioni[i].id == id) {
                operazioni[i].importo = importo;
                strcpy(operazioni[i].causale, causale);
                found = 1;
                break;
            }
        }
        if (found)
            send(client_sock, "[SERVER] Operazione aggiornata\n", 33, 0);
        else
            send(client_sock, "[SERVER] ID non trovato\n",25,0);
    }
    pthread_mutex_unlock(&lock);
}

void list_operazioni(int client_sock) {
    pthread_mutex_lock(&lock);
    char response[2048] = "[SERVER] Operazioni:\n";
    char temp[150];
    for (int i = 0; i < num_operazioni; i++) {
        snprintf(temp, sizeof(temp), "ID: %d, Importo: %.2f, Causale: %s\n",
                 operazioni[i].id, operazioni[i].importo, operazioni[i].causale);
        if (strlen(response)+strlen(temp)<sizeof(response)-1)
            strcat(response, temp);
        else
            break;
    }
    send(client_sock, response, strlen(response), 0);
    pthread_mutex_unlock(&lock);
}