#include "header.h"
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3 || (strcmp(argv[1], "rate") && strcmp(argv[1], "edf"))) {
        fprintf(stderr, "Erro: uso: ./scheduler <rate|edf> <arquivo>\n");
        return 1;
    }
    FILE *entrada = fopen(argv[2], "r");
    if (!entrada) { perror("Erro ao abrir entrada"); return 1; }
    Tarefa *tarefas = NULL;
    size_t n = 0;
    long long total = 0;
    int ok = ler(entrada, &tarefas, &n, &total);
    fclose(entrada);
    if (ok) {
        const char *nome = !strcmp(argv[1], "edf") ? "edf_vsb.out" : "rate_vsb.out";
        FILE *saida = fopen(nome, "w");
        if (!saida) { perror("Erro ao criar saída"); ok = 0; }
        else {
            simular(saida, tarefas, n, total, !strcmp(argv[1], "edf"));
            int falhou = ferror(saida);
            if (fclose(saida)) falhou = 1;
            if (falhou) {
                fprintf(stderr, "Erro ao gravar saída.\n");
                remove(nome); ok = 0;
            }
        }
    }
    for (size_t i = 0; i < n; i++) free(tarefas[i].nome);
    free(tarefas);
    return ok ? 0 : 1;
}
