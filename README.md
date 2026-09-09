# Escalonamento de tarefas críticas de voo

Simulador preemptivo de rate-monotonic e earliest-deadline-first (EDF).
Implementado e testado em CachyOS Linux x86_64, com GCC 16.1.1,
GNU Make 4.4.1 e fish 4.8.1. Requer compilador C11, make e funções POSIX
(Linux, por exemplo). Não utiliza bibliotecas externas.

## Compilar e executar

Na raiz de `Escalonamento_VSB`, ou em `vsb/` após extrair a entrega:

```sh
make clean
make
printf '100\nATT 20 12 8\nNAV 50 30 15\n' > voo.txt
./scheduler rate voo.txt
cat rate_vsb.out
./scheduler edf voo.txt
cat edf_vsb.out
```

`make` sem alvo produz um único executável, `scheduler`. É possível escolher
outro compilador com `make CC=clang`, após `make clean`. A limpeza remove o
executável, objetos e as duas saídas. A execução normal é silenciosa;
o resultado é gravado no diretório corrente em `rate_vsb.out` ou `edf_vsb.out`.
O identificador `vsb` conserva o utilizado nesta atividade e nas entregas anteriores.

## Arquivos

- `main.c`: contém somente `main`; valida argumentos, abre arquivos, chama
  a leitura e a simulação, trata falhas de escrita e libera os recursos.
- `header.h`: define a estrutura `Tarefa` e declara as funções compartilhadas.
- `scheduler.c`: implementa leitura, validação, seleção rate/EDF, simulação
  e escrita dos trechos e contadores.
- `Makefile`: compilação e limpeza; `.gitignore`: exclusão de artefatos gerados.

Os arquivos de entrada dos testes manuais, as referências e `evidencias.log`
acompanham o `.tar`, todos na raiz de `vsb/`. No ambiente de trabalho,
ficam na raiz de `Escalonamento_VSB`, fora dos commits. Não há suíte automatizada.
O Git contém somente os três fontes, Makefile, README e `.gitignore`.
O PDF e o `.tar` são gerados fora do repositório.

## Entrada e decisões

A primeira linha contém o tempo total. As seguintes contêm
`NOME PERIODO DEADLINE BURST`. Os números são inteiros decimais positivos
representáveis em `long long`, com `BURST <= DEADLINE <= PERIODO`.
Cada nome é um token sem espaços. A posição no arquivo identifica a tarefa,
mesmo com nomes repetidos. Campos extras, linhas vazias, byte nulo e arquivo
sem tarefas são rejeitados; CRLF e última linha sem quebra são aceitos.

Todas as tarefas chegam em zero e novamente a cada período. Rate escolhe
o menor período; EDF escolhe o menor deadline absoluto (chegada + deadline).
Em qualquer empate, inclusive numa chegada, vence a primeira tarefa no arquivo.
A preempção preserva a rajada restante; a perda descarta a instância no prazo
exato, mesmo quando ela está esperando. Ela só retorna na próxima chegada.

Conclusões no instante do deadline são válidas. No instante final, processam-se
primeiro conclusões e perdas, depois contam-se as instâncias ainda pendentes
como Killed, sem novas chegadas. Essa interpretação mantém a regra de perda
no deadline exato também quando ele coincide com o fim da simulação.

Os trechos terminam em F (conclusão), H (preempção), L (perda) ou K (fim).
Perdas e mortes em espera aparecem somente nos contadores, pois não houve
execução naquele intervalo. Ociosidade consecutiva é agrupada em `idle`.
A simulação avança por unidade: O(tempo total × número de tarefas), com
memória O(número de tarefas). Tempos totais muito grandes podem demorar.

Entrada inválida produz mensagem em stderr e retorno diferente de zero.
Toda a entrada é validada antes de abrir a saída, portanto um erro de entrada
não cria arquivo nem sobrescreve um resultado anterior.

## Testes manuais

No exemplo acima, rate perde uma instância de NAV e conclui outra; EDF conclui
as duas sem perdas. Ambos concluem cinco instâncias de ATT. Em t=20,
rate interrompe NAV pelo período menor de ATT. EDF mantém NAV, cujo prazo
é 30, antes da nova ATT, cujo prazo é 32: NAV termina em 23 e ATT em 31.

No pacote, compare as saídas completas com as referências:

```sh
./scheduler rate voo.txt
diff -u rate_esperado.txt rate_vsb.out
./scheduler edf voo.txt
diff -u edf_esperado.txt edf_vsb.out
```

Cada `diff` deve retornar zero, sem diferenças. Execute também
`./scheduler rate CASO.txt` e `./scheduler edf CASO.txt` para os casos abaixo,
consultando o respectivo `.out` com `cat`.

| Caso | Resultado a conferir |
| --- | --- |
| `empate.txt` / `empate_invertido.txt` | EDF interrompe B em t=4 somente quando A aparece antes. |
| `periodos_iguais.txt` | Z precede A, obedecendo à ordem do arquivo. |
| `descarte.txt` | B perde duas instâncias; há uma unidade ociosa antes de cada nova chegada. |
| `fronteira.txt` | A completa e B perde no instante final; nenhum Killed. |
| `conclusao.txt` | C = D = P: uma conclusão, sem perdas nem Killed. |
| `killed.txt` | A e B têm um Killed cada, inclusive B em espera. |
| `perda_espera.txt` | Rate perde B em espera; EDF conclui B e perde A. |
| `ocioso.txt` | Duas conclusões e dois intervalos ociosos de quatro unidades. |
| `grande.txt` / `crlf.txt` | Valores acima de INT_MAX, CRLF e ausência da quebra final são aceitos. |

Para um teste de erro reproduzível também a partir do Git:

```sh
make clean
make
printf '10\nA 5 3 4\n' > invalido.txt
./scheduler rate invalido.txt >stdout.txt 2>stderr.txt
echo $status
cat stderr.txt
wc -c stdout.txt
test ! -e rate_vsb.out
echo $status
```

Em fish, o primeiro retorno deve ser 1 e o último 0. A mensagem informa
entrada inválida na linha 2; stdout tem zero bytes. Em bash, use `$?`.
Confira também `./scheduler`, `./scheduler fifo voo.txt` e uma entrada
inexistente. O pacote inclui casos de campos faltantes/extras, números
inválidos, zero, negativos, D > P, C > D, estouro e arquivo sem tarefas.
Para leitura ilegível, use `chmod 000 sem_permissao.txt`, execute o programa
como usuário comum e restaure com `chmod 644 sem_permissao.txt`.

`evidencias.log` contém comandos, saídas e retornos reais capturados com
`script` e fish no CachyOS, incluindo compilação limpa, casos válidos,
erros, compilação com Clang e verificação da entrega extraída.
