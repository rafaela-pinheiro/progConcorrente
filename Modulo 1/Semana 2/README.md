# Multiplicação de Matrizes

### Avaliação dos tempos de execução

Para cada tamanho de matriz, foram feitos 10 testes. As médias dos tempos de execução de cada etapa foram as seguintes:

|  Tamanho  |  Leitura  | Multiplicação |  Escrita  |   Total   |
| :-------: | :-------: | :-----------: | :-------: | :-------: |
|  500x500  | 0,0049870 |   0,4351320   | 0,0019276 | 0,4420469 |
| 1000x1000 | 0,0074960 |   3,6625260   | 0,0029642 | 3,6729679 |
| 2000x2000 | 0,0180210 |   36,694173   | 0,0081175 | 36,720311 |

### Ganho de desempenho na versão concorrente

Para calcular o ganho de desempenho teórico que poderá ser alcançado com uma futura implementação concorrente, usaremos a _Lei de Amdahl_:

$$S = \frac{T_e + T_p + T_s}{T_e + \frac{T_p}{N} + T_s}$$

onde

- $S$ é o _speedup_
- $T_e$ é o tempo de leitura dos arquivos de entrada,
- $T_s$ é o tempo de escrita no arquivo de saída,
- $T_p$ é o tempo de processamento (nesse caso, a multiplicação),
- $N$ é a quantidade de threads usadas.

Considerando o tempo médio de cada etapa e a utilização de 4 threads, temos:

- Multiplicação de duas matrizes $500$ x $500$
  $$S = \frac{0,0049870 + 0,4351320 + 0,0019276}{0,0049870 + \frac{0,4351320}{4} + 0,0019276}$$
  $$S = \frac{0,4420466}{0,1156976} \approx 3,8$$

- Multiplicação de duas matrizes $1000$ x $1000$
  $$S = \frac{0,0074960 + 3,6625260 + 0,0029642}{0,0074960 + \frac{3,6625260}{4} + 0,0029642}$$
  $$S = \frac{3,6730202}{0,9260917} \approx 4$$

- Multiplicação de duas matrizes $2000$ x $2000$
  $$S = \frac{0,0180210 + 36,694173 + 0,0081175}{0,0180210 + \frac{36,694173}{4} + 0,0081175}$$
  $$S = \frac{36,7203115}{9,19968175} \approx 4$$
