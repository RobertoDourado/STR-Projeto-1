#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

//Direções 
#define A1 0 //Origem
#define B1 1 //Origem
#define A2 2 //Destino
#define B2 3 //Destino

//Prioridade
#define ALTA 0
#define MEDIA 0
#define BAIXA 0

//Numero de trens 
#define NUM_TRENS 5

// Struct - Especifiações de cada trem
typedef struct {
    int id;
    int origem;
    int destino;
    int prioridade;

} Trem;



//Semáforos
sem_t cruzamento;
int prioridade_atual = ALTA;

//Função para especificações aleatórias dos trens
void *trem(void *arg){
    Trem *in_trem = (Trem *)arg; 
    
    while (1)
    {
        printf("Trem %d Origem: %d Destino: %d Prioridade %d se aproximando do cruzamento\n",in_trem->id,in_trem->origem,in_trem->destino,in_trem->prioridade);

        printf("Trem %d aguardando \n",in_trem->id);

        sem_wait(&cruzamento);

        //Problema com a questão de prioridade. impossivel
        //Se o trem 1 com p1 chegar primeiro em A1 impossível o 
        //trem 2 com p0 que chegou depois em A1 passar primeiro.
        //COrrigir

        //teste
        if (in_trem->prioridade == 1){
            sem_post(&cruzamento);
            usleep(100);
            sem_wait(&cruzamento);
        }else if(in_trem->prioridade == 2){
            sem_post(&cruzamento);
            usleep(200);
            sem_wait(&cruzamento);
        }
        //teste

        printf("Trem %d passando pelo cruzamento \n",in_trem->id);
        sleep(1);
        printf("Trem %d passou pelo cruzamento \n",in_trem->id);

        sem_post(&cruzamento);
        
        //Alterar origem/destino/prioridade para a próxima viagem
        in_trem->origem = rand() % 2;
        in_trem->destino = rand() % 2 + 2;
        in_trem->prioridade = rand() % 3;

        sleep(rand() % 1 + 1);
    }
    
}
int main(){

    while(1){
        //Inicializando semáforo
        sem_init(&cruzamento,0,1);

        //Criação dos trens aleatórios / threads
        pthread_t trens[NUM_TRENS];
        Trem info_trens[NUM_TRENS];

        srand(time(NULL));

        for (int i = 0; i < NUM_TRENS; i++){

            info_trens[i].id = i + 1;
            info_trens[i].origem = rand() % 2; //Gera numero entre 1(A1) e 2(B1)
            info_trens[i].destino = rand() % 2 + 2; //Encontrar uma forma de deixar mais aleatório
            info_trens[i].prioridade = rand() % 3;

            pthread_create(&trens[i], NULL, trem, &info_trens[i]);
        
            //Tempo aleatório para criar o processo
            sleep(rand() % 1 + 1);
        }

        for(int i=0; i < NUM_TRENS; i++){
            pthread_join(trens[i], NULL);
        }

        sem_destroy(&cruzamento);
    }
    return 0;

}