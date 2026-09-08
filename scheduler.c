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

static void trecho(FILE *saida, Tarefa *tarefas, int atual, long long unidades, char motivo) {
    if (!unidades) return;
    if (atual < 0) fprintf(saida, "idle for %lld units\n", unidades);
    else fprintf(saida, "[%s] for %lld units - %c\n", tarefas[atual].nome, unidades, motivo);
}

void simular(FILE *saida, Tarefa *tarefas, size_t n, long long total) {
    int atual = -1;
    long long unidades = 0;
    fprintf(saida, "EXECUTION BY RATE\n\n");
    for (long long tempo = 0; tempo <= total; tempo++) {
        /* Conclusões precedem deadlines; não há chegadas no fim da simulação. */
        if (atual >= 0 && !tarefas[atual].restante) {
            tarefas[atual].completas++;
            trecho(saida, tarefas, atual, unidades, 'F');
            atual = -1; unidades = 0;
        }
        for (size_t i = 0; i < n; i++) {
            if (tarefas[i].restante && tarefas[i].prazo == (unsigned long long)tempo) {
                tarefas[i].perdidas++;
                tarefas[i].restante = 0;
                if ((int)i == atual) {
                    trecho(saida, tarefas, atual, unidades, 'L');
                    atual = -1; unidades = 0;
                }
            }
        }
        if (tempo == total) {
            trecho(saida, tarefas, atual, unidades, 'K');
            for (size_t i = 0; i < n; i++)
                if (tarefas[i].restante) tarefas[i].mortas++;
            break;
        }
        int proxima = -1;
        for (size_t i = 0; i < n; i++) {
            if (tempo % tarefas[i].periodo == 0) {
                tarefas[i].restante = tarefas[i].burst;
                /* A soma de dois long long positivos cabe no tipo sem sinal. */
                tarefas[i].prazo = (unsigned long long)tempo + tarefas[i].deadline;
            }
            if (!tarefas[i].restante) continue;
            if (proxima < 0 || tarefas[i].periodo < tarefas[proxima].periodo)
                proxima = (int)i;
        }
        if (proxima != atual) {
            trecho(saida, tarefas, atual, unidades, 'H');
            atual = proxima; unidades = 0;
        }
        if (atual >= 0) tarefas[atual].restante--;
        unidades++;
    }
    const char *titulos[] = {"LOST DEADLINES", "COMPLETE EXECUTION", "KILLED"};
    for (int secao = 0; secao < 3; secao++) {
        fprintf(saida, "\n%s\n", titulos[secao]);
        for (size_t i = 0; i < n; i++)
            fprintf(saida, "[%s] %lld\n", tarefas[i].nome,
                    secao == 0 ? tarefas[i].perdidas : secao == 1 ? tarefas[i].completas : tarefas[i].mortas);
    }
}
