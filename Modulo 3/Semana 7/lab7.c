// Aluna: Rafaela de Carvalho Machado Pinheiro
// DRE: 121038166

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define TAM 10

int buffer[TAM];
sem_t mutexProd, mutexCons, slotCheio, slotVazio;

void insere(int *item)
{
    static int in = 0;
    sem_wait(&slotVazio); // decrementa slotVazio e não podemos mais inserir até que o buffer esteja vazio novamente

    sem_wait(&mutexProd); // protege a seção crítica
    // enquanto buffer não está cheio, insere elementos
    for (int i = 0; i < TAM; i++)
    {
        buffer[in] = item[i];
        in = (in + 1) % TAM;
        printf("Elemento inserido: [%d] = %d\n", i, item[i]);
        sleep(1);
    }
    // fim da seção crítica
    sem_post(&mutexProd);
    for (int i = 0; i < TAM; i++)
        sem_post(&slotCheio); // incrementa slotCheio até o máximo (tamanho do buffer)
}

int retira(void)
{
    int item;
    static int out = 0;

    // decremeta slotCheio, pois uma posição do buffer não está mais ocupada
    sem_wait(&slotCheio);

    // início da seção crítica
    sem_wait(&mutexCons);
    item = buffer[out];
    out = (out + 1) % TAM;
    // printf("Elemento retirado: %d\n", item);
    sleep(1);

    // se o buffer está cheio (i.e., out voltou para o início da lista circular), podemos inserir novamente
    if (out == 0)
    {
        // incrementa slotVazio para permitir outra inserção
        sem_post(&slotVazio);

        // fim da seção crítica
        sem_post(&mutexCons);
    }
    else
    {
        // fim da seção crítica
        sem_post(&mutexCons);
    }

    return item;
}

void *produtor(void *arg)
{
    // recebe os elementos que devem ser inseridos
    int itens[TAM];

    while (1)
    {
        // preenche o vetor de itens
        for (int i = 0; i < TAM; i++)
            itens[i] = i;
        insere(itens);
    }
    pthread_exit(NULL);
}

void *consumidor(void *arg)
{
    int item;
    while (1)
    {
        item = retira();
        printf("Elemento retirado: %d\n", item);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Digite: %s <quantidade de threads consumidoras> <quantidade de threads produtoras>\n", argv[0]);
        return 1;
    }
    int i = 0;
    int qntdCons = atoi(argv[1]);
    int qntdProd = atoi(argv[2]);

    int nthreads = qntdCons + qntdProd;
    pthread_t tid[nthreads];

    // inicializa os semáforos
    sem_init(&mutexCons, 0, 1); // semáforo para exclusão mútua
    sem_init(&mutexProd, 0, 1); // semáforo para exclusão mútua
    sem_init(&slotCheio, 0, 0); // quantidade de espaços cheios no buffer
    sem_init(&slotVazio, 0, 1); // mostra se o buffer está vazio ou não
    // o semáforo "slotVazio" inicia com 1 porque só pode haver uma inserção de cada vez (já que cada inserção preenche completamente o buffer)

    // cria as threads consumidoras
    for (i = 0; i < qntdCons; i++)
        pthread_create(&tid[i], NULL, consumidor, NULL);

    // cria as threads produtoras
    for (i = qntdCons; i < nthreads; i++)
        pthread_create(&tid[i], NULL, produtor, NULL);

    for (i = 0; i < nthreads; i++)
    {
        if (pthread_join(tid[i], NULL))
        {
            printf("Erro - pthread_join \n");
            exit(-1);
        }
    }
    pthread_exit(NULL);
}
