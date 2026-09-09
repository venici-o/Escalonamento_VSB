# Escalonamento de tarefas críticas de voo

Simulador preemptivo de rate-monotonic (`rate`) e earliest-deadline-first (`edf`).

## Requisitos

Compilador C11 (como GCC), make e ambiente POSIX. Desenvolvido e testado
em CachyOS Linux. Não utiliza bibliotecas externas.

## Arquivos

- `main.c`: argumentos, abertura dos arquivos e chamada da simulação.
- `header.h`: estrutura das tarefas e declarações das funções.
- `scheduler.c`: leitura, validação, escalonamento e gravação dos resultados.
- `Makefile`: compilação e limpeza.
- `voo.txt`: entrada de exemplo do enunciado.

## Como usar

Na raiz do projeto:

```sh
make
./scheduler rate voo.txt
cat rate_vsb.out
./scheduler edf voo.txt
cat edf_vsb.out
```

`make` gera o executável `scheduler`. O programa grava o resultado no `.out`
do algoritmo escolhido e não imprime nada durante a execução normal.
Use `make clean` para remover o executável, objetos e as saídas.

Para usar outra entrada, substitua `voo.txt` pelo caminho do arquivo.
A primeira linha contém o tempo total; as demais, `NOME PERIODO DEADLINE BURST`.
Os números devem ser inteiros positivos, com `BURST <= DEADLINE <= PERIODO`.

## Testes manuais

No exemplo acima, rate perde um deadline de NAV; EDF não perde nenhum.
Ambos concluem cinco instâncias de ATT; rate conclui uma NAV e EDF, duas.

Execute também `./scheduler fifo voo.txt` para testar um argumento inválido:
o programa deve informar o erro em stderr e retornar um código diferente de zero.
Mais entradas de teste e o registro `evidencias.log` acompanham o pacote de entrega.
