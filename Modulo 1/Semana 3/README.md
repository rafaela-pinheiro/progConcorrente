# Implementação Concorrente da Multiplicação de Matrizes

### Avaliação dos tempos de execução

Para cada tamanho de matriz, foram feitos 5 testes, usando 4 threads para a etapa de  multiplicação. As médias dos tempos de execução de cada etapa foram as seguintes:

|  Tamanho  |  Leitura  | Multiplicação |  Escrita  |   Total   |
| :-------: | :-------: | :-----------: | :-------: | :-------: |
|  500x500  | 0,0018290 |   0,21272060  | 0,0007508 | 0,215300  |
| 1000x1000 | 0,0028034 |   1,78431840  | 0,0137600 | 1,789874  |
| 2000x2000 | 0,0170502 |   17,7871322  | 0,0103254 | 17,81451  |

### Ganho de desempenho

Usando a _Lei de Amdahl_, o _speedup_ foi
- 7,9 para matrizes $500$ x $500$
- 8,13 para matrizes $1000$ x $1000$
- 8,2 para matrizes $2000$ x $2000$

_Obs._: Em um processador com 4 núcleos.