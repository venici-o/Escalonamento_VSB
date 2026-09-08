#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>

typedef struct {
    char *nome;
    long long periodo, deadline, burst;
} Tarefa;

int ler(FILE *entrada, Tarefa **tarefas, size_t *quantidade, long long *total);

#endif
