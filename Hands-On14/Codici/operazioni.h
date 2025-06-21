#ifndef OPERAZIONI_H
#define OPERAZIONI_H

#define MAX_OP 100

typedef struct {
    int id;
    float importo;
    char causale[100];
} Operazione;

extern Operazione operazioni[MAX_OP];
extern int num_operazioni;

void add_operazione(int client_sock, char *args);
void delete_operazione(int client_sock, int id);
void update_operazione(int client_sock, char *args);
void list_operazioni(int client_sock);

#endif