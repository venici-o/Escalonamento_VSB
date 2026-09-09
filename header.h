#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>

typedef struct {
    char *nome;
    long long periodo, deadline, burst;
    unsigned long long prazo;
    long long restante, perdidas, completas, mortas;
} Tarefa;

int ler(FILE *entrada, Tarefa **tarefas, size_t *quantidade, long long *total);
void simular(FILE *saida, Tarefa *tarefas, size_t n, long long total, int edf);

#endif
