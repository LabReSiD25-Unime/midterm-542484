#ifndef COMMON_H
#define COMMON_H

#include <pthread.h>

#define MAX_OP 100
#define PORT 8080
#define MAX_THREADS 10

typedef struct {
    int id;
    float importo;
    char causale[100];
} Operazione;

extern Operazione operazioni[MAX_OP];
extern int num_operazioni;
extern pthread_mutex_t lock;

// Funzioni operazioni
void add_operazione(int client_sock, char *args);
void delete_operazione(int client_sock, int id);
void update_operazione(int client_sock, char *args);
void list_operazioni(int client_sock);

#endif