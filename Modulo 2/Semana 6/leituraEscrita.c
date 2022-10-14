// Rafaela de Carvalho Machado Pinheiro
// DRE: 121038166

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int qntdL; // quantidade de threads leitoras
int qntdE; // quantidade de threads escritoras

int cLeit = 0; // contador de threads lendo
int cEsc = 0;  // contador de threads escrevendo

// variáveis para sincronização
pthread_mutex_t mutex;
pthread_cond_t condL, condE; // variáveis de condição de leitura e escrita respectivamente

int varGlobal; // variável para representar a base de dados

// entrada leitura
int inicLeit(int id)
{
    pthread_mutex_lock(&mutex);
    printf("L[%d] quer ler\n", id);
    while (cEsc > 0)
    {
        printf("L[%d] foi bloqueada\n", id);
        pthread_cond_wait(&condL, &mutex);
        printf("L[%d] foi desbloqueada\n", id);
    }
    cLeit++;
    pthread_mutex_unlock(&mutex);
}

// saída leitura
int fimLeit(int id)
{
    pthread_mutex_lock(&mutex);
    printf("L[%d] terminou de ler\n", id);
    cLeit--;
    if (cLeit == 0)
        pthread_cond_signal(&condE);
    pthread_mutex_unlock(&mutex);
}

// entrada escrita
int inicEsc(int id)
{
    pthread_mutex_lock(&mutex);
    printf("E[%d] quer escrever\n", id);
    while (cEsc > 0 || cLeit > 0)
    {
        printf("E[%d] foi bloqueada\n", id);
        pthread_cond_wait(&condE, &mutex);
        printf("E[%d] foi desbloqueada\n", id);
    }
    cEsc++;
    pthread_mutex_unlock(&mutex);
}

// saída escrita
int fimEsc(int id)
{
    pthread_mutex_lock(&mutex);
    printf("E[%d] terminou de escrever\n", id);
    cEsc--;
    pthread_cond_signal(&condE);
    pthread_cond_broadcast(&condL);
    pthread_mutex_unlock(&mutex);
}

void *tarefa1(void *arg)
{
    // apenas escrita
    int *id = (int *)arg;
    inicEsc(*id);
    printf("Thread %d esta escrevendo na tarefa 1\n", *id);
    varGlobal++;
    fimEsc(*id);
    sleep(1);
    pthread_exit(NULL);
}

void *tarefa2(void *arg)
{
    // apenas leitura
    int *id = (int *)arg;
    inicLeit(*id);
    printf("Thread %d esta lendo na tarefa 2\n", *id);
    if (varGlobal % 2 == 0)
        printf("\nA variavel global eh par\n");
    else
        printf("\nA variavel global eh impar\n");
    fimLeit(*id);
    sleep(1);
    pthread_exit(NULL);
}

void *tarefa3(void *arg)
{
    // leitura e escrita
    int *id = (int *)arg;
    int boba1, boba2;
    inicLeit(*id);
    printf("Thread %d esta lendo na tarefa 3\n", *id);
    printf("A variavel global eh %d\n", varGlobal);
    fimLeit(*id);

    // processamento para gastar tempo
    boba1 = 100;
    boba2 = -100;
    while (boba2 < boba1)
        boba2++;

    // escrita
    inicEsc(*id);
    printf("Thread %d esta escrevendo na tarefa 3\n", *id);
    varGlobal = (int)id;
    fimEsc(*id);
    sleep(1);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        fprintf(stderr, "Digite: %s <quantidade de threads T1> <quantidade de threads T2> <quatidade de threads T3>\n", argv[0]);
        return 1;
    }

    // identificadores das threads
    int qntdT1 = atoi(argv[1]);
    int qntdT2 = atoi(argv[2]);
    int qntdT3 = atoi(argv[3]);
    int nThreads = qntdT1 + qntdT2 + qntdT3;
    printf("\n %d \n", nThreads);

    pthread_t tid[nThreads];
    int id[nThreads];

    // inicializa as variáveis de sincronização
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condL, NULL);
    pthread_cond_init(&condE, NULL);

    // cria as threads que vão executar a tarefa 1 (somente escrita)
    for (int i = 0; i < qntdT1; i++)
    {
        id[i] = i + 1;
        if (pthread_create(&tid[i], NULL, tarefa1, (void *)&id[i]))
            exit(-1);
    }

    // cria as threads que vão executar a tarefa 2 (somente leitura)
    for (int i = 0; i < qntdT2; i++)
    {
        id[i + qntdT1] = i + 1;
        if (pthread_create(&tid[i + qntdT1], NULL, tarefa2, (void *)&id[i + qntdT1]))
            exit(-1);
    }

    // cria as threads que vão executar a tarefa 3 (leitura e  escrita)
    for (int i = 0; i < qntdT3; i++)
    {
        id[i + qntdT1 + qntdT2] = i + 1;
        if (pthread_create(&tid[i + qntdT1 + qntdT2], NULL, tarefa3, (void *)&id[i + qntdT1 + qntdT2]))
            exit(-1);
    }

    // espera todas as threads terminarem
    for (int i = 0; i < nThreads; i++)
        pthread_join(tid[i], NULL);

    // desaloca variáveis e encerra o programa
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condL);
    pthread_cond_destroy(&condE);

    pthread_exit(NULL);

    return 0;
}
