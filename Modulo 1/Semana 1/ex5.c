/*
Rafaela C. M. Pinheiro
DRE: 121038166
*/

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

// valores conforme pedidos no enunciado
#define NTHREADS 2 
#define TAMVETOR 100

double vetor[TAMVETOR];
double vCopia[TAMVETOR];

// função a ser executada pelas threads
void * incVetor(void *arg){
    int idThread = * (int*) arg;
    for(int i = idThread; i < TAMVETOR; i += NTHREADS){
        vetor[i] *= 1.1;
    }
     
    printf("-- Fim da thread %d\n", idThread + 1);   
    pthread_exit(NULL);
}

int main(){
    pthread_t sysTids[NTHREADS];    // ids das threads no sistema
    int localTids[NTHREADS];        // ids locais das threads
    
    // preenchendo o vetor
    for(int i = 1; i <= TAMVETOR; i++){
        vetor[i - 1] = i;
        vCopia[i - 1] = i;
    }

    // criação das threads
    for(int i = 0; i < NTHREADS; i++){       
        localTids[i] = i;
        if(pthread_create(&sysTids[i], NULL, incVetor, (void*) &localTids[i])){
            printf("ERRO: pthread_create()\n");
        }
    }
    
    // espera todas as threads terminarem
    for (int i = 0; i < NTHREADS; i++){
        if (pthread_join(sysTids[i], NULL)){
            printf("ERRO: pthread_join() \n");
            exit(-1);
        }
    }
    
    // verificação do resultado
    for(int i = 0; i <TAMVETOR; i++){
        if(vetor[i] != vCopia[i]*1.1){
            printf("ERRO: vetor[%d] = %f != %f\n", i, vetor[i], (vCopia[i]*1.1));
        }
    }

    printf("-- Fim da thread principal --\n");
    return 0;
}
