#define _POSIX_C_SOURCE 200809L
#include "header.h"
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int positivo(const char *texto, long long *valor) {
    char *fim;
    for (const char *p = texto; *p; p++)
        if (!isdigit((unsigned char)*p)) return 0;
    errno = 0;
    *valor = strtoll(texto, &fim, 10);
    return !errno && fim != texto && !*fim && *valor > 0;
}

int ler(FILE *entrada, Tarefa **tarefas, size_t *quantidade, long long *total) {
    char *linha = NULL;
    size_t capacidade = 0;
    int numero = 0, ok = 1;
    ssize_t tamanho;
    while ((tamanho = getline(&linha, &capacidade, entrada)) != -1) {
        char *campos[5], *estado, *campo;
        int n = 0;
        numero++;
        if (memchr(linha, '\0', (size_t)tamanho)) { ok = 0; break; }
        campo = strtok_r(linha, " \t\r\n", &estado);
        while (campo && n < 5) {
            campos[n++] = campo;
            campo = strtok_r(NULL, " \t\r\n", &estado);
        }
        if (numero == 1) {
            if (n != 1 || !positivo(campos[0], total)) { ok = 0; break; }
        } else {
            Tarefa tarefa = {0};
            if (n != 4 || !positivo(campos[1], &tarefa.periodo) ||
                !positivo(campos[2], &tarefa.deadline) ||
                !positivo(campos[3], &tarefa.burst) ||
                tarefa.burst > tarefa.deadline || tarefa.deadline > tarefa.periodo) {
                ok = 0; break;
            }
            tarefa.nome = strdup(campos[0]);
            Tarefa *novas = realloc(*tarefas, (*quantidade + 1) * sizeof(Tarefa));
            if (!novas || !tarefa.nome) {
                free(tarefa.nome);
                if (novas) *tarefas = novas;
                ok = 0; break;
            }
            *tarefas = novas;
            (*tarefas)[(*quantidade)++] = tarefa;
        }
    }
    if (ferror(entrada) || !numero || !*quantidade) ok = 0;
    if (!ok) fprintf(stderr, "Erro: entrada inválida ou falha de leitura/alocação na linha %d.\n", numero);
    free(linha);
    return ok;
}
