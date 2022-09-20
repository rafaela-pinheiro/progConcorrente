// Rafaela C. M. Pinheiro
// DRE: 121038166

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "timer.h"
#include <pthread.h>

// matrizes de entrada
float *matriz1, *matriz2;

// dimensões das matrizes de entrada
int qntdLin1, qntdCol1, qntdLin2, qntdCol2;

// matriz resultante
float *mSaida;
long long int dimSaida;

int nthreads;
int matrizAtual = 0;

typedef struct
{
    int dim; // tamanho da matriz saída
    int id;  // identificador da thread
} tArgs;

int inicializaMatriz(FILE *descArq)
{
    float *matriz;
    int lin, col;      // dimensões da matriz
    long long int dim; // quantidade de elementos da matriz
    size_t ret;        // retorno da função de leitura no arquivo de entrada

    if (!descArq)
    {
        fprintf(stderr, "ERRO -- abertura do arquivo\n");
        return 1;
    }

    // lê as dimensões da matriz
    ret = fread(&lin, sizeof(int), 1, descArq);
    if (!ret)
    {
        fprintf(stderr, "ERRO -- leitura das dimensoes da matriz no arquivo\n");
        return 2;
    }

    ret = fread(&col, sizeof(int), 1, descArq);
    if (!ret)
    {
        fprintf(stderr, "ERRO -- leitura das dimensoes da matriz no arquivo\n");
        return 2;
    }

    dim = lin * col;

    // aloca memória para a matriz
    matriz = (float *)malloc(sizeof(float) * dim);
    if (!matriz)
    {
        fprintf(stderr, "ERRO -- alocacao de memoria da matriz\n");
        return 3;
    }

    // carrega a matriz de elementos do arquivo
    ret = fread(matriz, sizeof(float), dim, descArq);
    if (ret < dim)
    {
        fprintf(stderr, "ERRO -- leitura dos elementos da matriz\n");
        return 4;
    }

    if (!matrizAtual)
    {
        matriz1 = matriz;
        qntdLin1 = lin;
        qntdCol1 = col;
    }
    else
    {
        matriz2 = matriz;
        qntdLin2 = lin;
        qntdCol2 = col;
    }
    matrizAtual++;

    return dim;
}

void *multiplica(void *arg)
{
    tArgs *args = (tArgs *)arg;
    int id = args->id;

    for (int i = id; i < qntdLin1; i += nthreads)
    {
        for (int j = 0; j < qntdCol2; j++)
        {
            mSaida[i * qntdCol2 + j] = 0;
            for (int k = 0; k < qntdCol1; k++)
                mSaida[i * qntdCol2 + j] += matriz1[i * qntdCol1 + k] * matriz2[k * qntdCol2 + j];
        }
    }
    pthread_exit(NULL);
}

int salvaMatriz(FILE *descArqSaida)
{
    size_t ret;

    // abre o arquivo para escrita binária
    if (!descArqSaida)
    {
        fprintf(stderr, "ERRO -- abertura do arquivo para escrita\n");
        return 1;
    }

    // escreve número de linhas e de colunas
    ret = fwrite(&qntdLin1, sizeof(int), 1, descArqSaida);
    ret = fwrite(&qntdCol2, sizeof(int), 1, descArqSaida);

    // escreve a matriz no arquivo
    ret = fwrite(mSaida, sizeof(float), dimSaida, descArqSaida);
    if (ret < dimSaida)
    {
        fprintf(stderr, "ERRO -- escrita no arquivo\n");
        return 2;
    }

    fclose(descArqSaida);
    return 0;
}

int main(int argc, char *argv[])
{
    FILE *descArq1, *descArq2, *descArqSaida;
    pthread_t *tid; // identificadores das threads no sistemas
    tArgs *args;    // identificadores locais das threads e dimensão
    double ini, fim;

    if (argc < 5)
    {
        fprintf(stdout, "Digite: %s <arquivo entrada 1> <arquivo entrada 2> <arquivo saida> <quantidade threads>\n", argv[0]);
        return 1;
    }
    nthreads = atoi(argv[4]);

    // INICIALIZAÇÃO
    GET_TIME(ini);

    // abre os arquivos para leitura binária
    descArq1 = fopen(argv[1], "rb");
    inicializaMatriz(descArq1);

    descArq2 = fopen(argv[2], "rb");
    inicializaMatriz(descArq2);

    fprintf(stdout, "Dimensões da matriz 1: %d x %d\n", qntdLin1, qntdCol1);
    fprintf(stdout, "Dimensões da matriz 2: %d x %d\n", qntdLin2, qntdCol2);

    if (qntdCol1 == qntdLin2)
    {
        fprintf(stdout, "Multiplicação possível\n");

        // aloca matriz de saída
        dimSaida = qntdLin1 * qntdCol2;
        mSaida = (float *)malloc(sizeof(float) * (dimSaida));
        if (!mSaida)
        {
            fprintf(stderr, "ERRO -- alocacao da memoria da matriz saida\n");
            return 2;
        }

        GET_TIME(fim);
        fprintf(stdout, "Tempo de inicializacao: %lf\n", fim - ini);

        // PROCESSAMENTO
        GET_TIME(ini);

        // alocação das estruturas
        tid = (pthread_t *)malloc(sizeof(pthread_t) * nthreads);
        if (tid == NULL)
        {
            fprintf(stderr, "ERRO -- alocacao da memoria das threads\n");
            return 2;
        }

        args = (tArgs *)malloc(sizeof(tArgs) * nthreads);
        if (args == NULL)
        {
            fprintf(stderr, "ERRO -- alocacao da memoria dos args\n");
            return 2;
        }

        // criação das threads
        for (int i = 0; i < nthreads; i++)
        {
            (args + i)->id = i;
            (args + i)->dim = dimSaida;
            if (pthread_create(tid + i, NULL, multiplica, (void *)(args + i)))
            {
                fprintf(stderr, "ERRO -- pthread_create()\n");
                return 3;
            }
        }

        // espera as threads terminarem
        for (int i = 0; i < nthreads; i++)
            if (pthread_join(*(tid + i), NULL))
            {
                fprintf(stderr, "ERRO -- pthread_join\n");
                exit(-1);
            }

        GET_TIME(fim);
        fprintf(stdout, "Tempo de processamento: %lf\n", fim - ini);

        // FINALIZAÇÃO
        GET_TIME(ini);

        // armazena a matriz de saída em um arquivo
        descArqSaida = fopen(argv[3], "a");
        salvaMatriz(descArqSaida);

        GET_TIME(fim);
        fprintf(stdout, "Tempo de finalizacao: %lf\n", fim - ini);

        // liberação da memória
        free(mSaida);
        free(matriz1);
        free(matriz2);
        free(args);
        free(tid);
    }
    else
    {
        fprintf(stderr, "Matrizes incompativeis para multiplicacao");
        return 2;
    }

    return 0;
}