#include "header.h"
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3 || strcmp(argv[1], "rate")) {
        fprintf(stderr, "Erro: uso: ./scheduler rate <arquivo>\n");
        return 1;
    }
    FILE *entrada = fopen(argv[2], "r");
    if (!entrada) { perror("Erro ao abrir entrada"); return 1; }
    Tarefa *tarefas = NULL;
    size_t n = 0;
    long long total = 0;
    int ok = ler(entrada, &tarefas, &n, &total);
    fclose(entrada);
    for (size_t i = 0; i < n; i++) free(tarefas[i].nome);
    free(tarefas);
    return ok ? 0 : 1;
}
