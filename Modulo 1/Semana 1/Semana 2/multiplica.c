// Rafaela C. M. Pinheiro
// DRE: 121038166

#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

#include <time.h>

// matrizes que serão carregadas dos arquivos
float *matriz1, *matriz2;

// dimensões das matrizes 1 e 2
int qntdCol1, qntdLin1, qntdCol2, qntdLin2;

// matriz resultante
float *mSaida;

int matrizAtual = 0;

double start, finish, elapsed;

int inicializaMatriz(FILE *descritorArquivo)
{
    float *matriz;
    int lin, col;      // dimensões da matriz
    long long int tam; // quantidade de elementos da matriz
    size_t ret;        // retorno da função de leitura no arquivo de entrada

    if (!descritorArquivo)
    {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 1;
    }

    // lê as dimensoes da matriz
    ret = fread(&lin, sizeof(int), 1, descritorArquivo);
    if (!ret)
    {
        fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
        return 2;
    }
    ret = fread(&col, sizeof(int), 1, descritorArquivo);
    if (!ret)
    {
        fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
        return 2;
    }
    // calcula a qtde de elementos da matriz
    tam = lin * col;

    // aloca memória para a matriz
    matriz = (float *)malloc(sizeof(float) * tam);
    if (!matriz)
    {
        fprintf(stderr, "Erro de alocacao da memoria da matriz\n");
        return 3;
    }

    // carrega a matriz de elementos do tipo float do arquivo
    ret = fread(matriz, sizeof(float), tam, descritorArquivo);
    if (ret < tam)
    {
        fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
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

    return tam;
}

void multiplica()
{
    // aloca memória para a matriz resultante
    mSaida = (float *)malloc(sizeof(float) * (qntdLin1 * qntdCol2));
    if (!mSaida)
    {
        fprintf(stderr, "Erro de alocacao da memoria da matriz\n");
        return;
    }

    // multiplica as matrizes
    for (int linAtual = 0; linAtual < qntdLin1; linAtual++)
    {
        for (int colAtual = 0; colAtual < qntdCol2; colAtual++)
        {
            mSaida[linAtual * qntdCol2 + colAtual] = 0;
            for (int elemAtual = 0; elemAtual < qntdCol1; elemAtual++)
            {
                mSaida[linAtual * qntdCol2 + colAtual] += matriz1[linAtual * qntdCol1 + elemAtual] * matriz2[elemAtual * qntdCol2 + colAtual];
            }
        }
    }
}

int salvaMatriz(FILE *descArqSaida)
{
    size_t ret;
    int tam = qntdLin1 * qntdCol2;

    // abre o arquivo para escrita binária
    if (!descArqSaida)
    {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 1;
    }

    // escreve número de linhas e de colunas
    ret = fwrite(&qntdLin1, sizeof(int), 1, descArqSaida);
    ret = fwrite(&qntdCol2, sizeof(int), 1, descArqSaida);

    // escreve a matriz no arquivo
    ret = fwrite(mSaida, sizeof(float), tam, descArqSaida);
    if (ret < tam)
    {
        fprintf(stderr, "Erro de escrita no  arquivo\n");
        return 2;
    }

    // finaliza o uso das variáveis
    fclose(descArqSaida);
    return 0;
}

int main(int argc, char *argv[])
{
    FILE *descritorArq1, *descritorArq2, *descArqSaida; // descritores dos arquivos de entrada e saída
    double somaTempo = 0;

    if (argc < 4)
    {
        fprintf(stderr, "Digite: %s <arquivo entrada 1> <arquivo entrada 2> <arquivo saida>\n", argv[0]);
        return 1;
    }

    GET_TIME(start);

    // abre os arquivos para leitura binária
    descritorArq1 = fopen(argv[1], "rb");
    inicializaMatriz(descritorArq1);

    descritorArq2 = fopen(argv[2], "rb");
    inicializaMatriz(descritorArq2);

    GET_TIME(finish);
    elapsed = finish - start;
    somaTempo += elapsed;
    fprintf(stdout, "Tempo de leitura dos arquivos: %lf\n", elapsed);

    // verifica se a multiplicação das matrizes é possível
    if (qntdCol1 == qntdLin2)
    {
        GET_TIME(start);
        multiplica();
        GET_TIME(finish);
        elapsed = finish - start;
        somaTempo += elapsed;
        fprintf(stdout, "Tempo de multiplicacao das matrizes: %lf\n", elapsed);

        GET_TIME(start);
        descArqSaida = fopen(argv[3], "a");
        salvaMatriz(descArqSaida);
        GET_TIME(finish);
        elapsed = finish - start;
        somaTempo += elapsed;
        fprintf(stdout, "Tempo de escrita no arquivo: %lf\n", elapsed);
        fprintf(stdout, "Tempo total de execução: %lf\n", somaTempo);
    }
    else
        fprintf(stderr, "Matrizes incompativeis para multiplicacao");
    return 0;
}