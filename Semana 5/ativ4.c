// Aluna: Rafaela de Carvalho Machado Pinheiro
// DRE: 121038166

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS 4

// variáveis globais
int estado = 0;
pthread_mutex_t estMutex;
pthread_cond_t estCond;

void *tarefaT4(void *arg)
{
    // 1a tarefa, executada pela thread 4

    // início da seção crítica
    pthread_mutex_lock(&estMutex);
    printf("\nSeja bem-vindo!\n");
    estado++;
    if (estado == 1)
        // essa tarefa já foi executada, podemos enviar o sinal para executar a próxima thread
        pthread_cond_signal(&estCond);

    // fim da seção crítica
    pthread_mutex_unlock(&estMutex);
    pthread_exit(NULL);
}

void *tarefaT2(void *arg)
{
    // 2a ou 3a tarefa, executada pela thread 2

    pthread_mutex_lock(&estMutex);
    while (estado == 0)
        pthread_cond_wait(&estCond, &estMutex);

    printf("\nFique a vontade.\n");
    estado++;

    pthread_cond_signal(&estCond);

    pthread_mutex_unlock(&estMutex);
    pthread_exit(NULL);
}

void *tarefaT3(void *arg)
{
    // 2a ou 3a tarefa, executada pela thread 3

    // início da seção crítica
    pthread_mutex_lock(&estMutex);

    while (estado == 0)
        // se estivermos executando a 1a thread, essa fica na espera
        pthread_cond_wait(&estCond, &estMutex);

    printf("\nSente-se, por favor.\n");
    estado++;

    pthread_cond_signal(&estCond);

    // fim da seção crítica
    pthread_mutex_unlock(&estMutex);
    pthread_exit(NULL);
}

void *tarefaT1(void *arg)
{
    // 4a e última tarefa , executada pela thread 1

    // início da seção crítica
    pthread_mutex_lock(&estMutex);

    // se estado não for 3 ainda, temos de esperar as outras threads antes de executarmos essa
    while (estado < 3)
        pthread_cond_wait(&estCond, &estMutex);

    printf("\nVolte sempre!\n");

    // fim da seção crítica
    pthread_mutex_unlock(&estMutex);
    pthread_exit(NULL);
}

int main()
{
    int i;
    pthread_t threads[NTHREADS];

    printf("\nINICIO\n");

    // inicializa o mutex e a var de condição
    pthread_mutex_init(&estMutex, NULL);
    pthread_cond_init(&estCond, NULL);

    // cria as threads
    pthread_create(&threads[3], NULL, tarefaT4, NULL);
    pthread_create(&threads[2], NULL, tarefaT3, NULL);
    pthread_create(&threads[1], NULL, tarefaT2, NULL);
    pthread_create(&threads[0], NULL, tarefaT1, NULL);

    // espera todas as threads acabarem
    for (i = 0; i < NTHREADS; i++)
        pthread_join(threads[i], NULL);

    printf("\nFIM\n");

    // desaloca variáveis
    pthread_mutex_destroy(&estMutex);
    pthread_cond_destroy(&estCond);
}