#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

//Numero de trens 
#define NUM_TRENS 5
#define INTERFACE 1 //Caso seja 0 a inteface se torna linhas de comando normais

// Struct - Especifiações de cada trem
typedef struct {
    int id;
    int origem;
    int destino;
    int prioridade;
} Trem;

//Criando 5 variaveis do tipo trem e inicializando tudo com 0
Trem info_trens[NUM_TRENS] = {0};

//
int posicao_a[NUM_TRENS],posicao_b[NUM_TRENS];
int i_A,i_B = 0;

//Mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

//Semáforos
sem_t cruzamento, posicao;

//Função direção em string
const char *direcao_para_string(int direcao){
    switch (direcao)
    {
    case 0:
        return "A1";
        break;
    case 1:
        return "B1";
        break;
    case 2:
        return "A2";
        break;
    case 3:
        return "B2";
        break;
    default:
        break;
    }
}

//Função prioridade em string
const char *prioridade_para_string(int prioridade){
    switch (prioridade)
    {
    case 0:
        return "ALTA";
        break;
    case 1:
        return "MEDIA";
        break;
    case 2:
        return "BAIXA";
        break;
    default:
        break;
    }

}


void imprimir_interface(int atual, int origem, int saida, int prio) {

    system("clear");
    printf(" _______________________________\n");
    printf("|----------- ORIGEM ------------|\n|\tA1\t\tB1\t|\n");
    
    for(int i=NUM_TRENS-1; i>=0; i--){

        if(posicao_a[i]==0){
            printf("|\t----\t\t");
        }
        else
        {
            printf("|\t%d:%s\t\t", posicao_a[i], prioridade_para_string(info_trens[posicao_a[i]-1].prioridade));
        }
        
        if(posicao_b[i]==0){
            printf("----\t|\n");
        }
        else
        {
            printf("%d:%s\t|\n", posicao_b[i], prioridade_para_string(info_trens[posicao_b[i]-1].prioridade));
        }
    }
    if(origem>=0){
        //printf("\n");
        printf(" _______________________________\n");
        printf( "|---------- CRUZAMENTO ---------|\n"
                "|\t\t%d:%s\t\t\n"
                "|_______________________________|\n", atual, prioridade_para_string(prio));
    }

    if(origem==-1){
        printf(" _______________________________\n");
        printf("|---------- CRUZAMENTO ---------|\n"
               "|\t\t----\t\t\n"
               "|_______________________________|\n");
    }

    printf(" _______________________________\n");
    printf("|----------- DESTINO -----------|\n|\tA2\t\tB2\t|\n");
    if (saida==2)
    {
        printf("|\t%d:%s\t\t----\t|\n", atual, prioridade_para_string(prio) );
    }
    if (saida==3)
    {
        printf("|\t----\t\t%d:%s\t|\n", atual, prioridade_para_string(prio));
    }

    if (saida==0)
    {
        printf("|\t----\t\t----\t|\n");
    }
    printf("|_______________________________|\n");
}


//Função para especificações aleatórias dos trens
void *trem(void *arg){
    Trem *in_trem = (Trem *)arg; 
    
    while (1)
    {
        //Set de posição e informações
        in_trem->origem = rand() % 2;
        in_trem->destino = rand() % 2 + 2;
        in_trem->prioridade = rand() % 3;  

        //**********Mostrando informações do trem gerado***
        if(INTERFACE==0){
            printf("Trem %d Origem: %s Destino: %s Prioridade %s se aproximando do cruzamento\n",
                    in_trem->id,
                    direcao_para_string(in_trem->origem),
                    direcao_para_string(in_trem->destino),
                    prioridade_para_string(in_trem->prioridade));

            printf("Trem %d aguardando \n",in_trem->id);
        }

        //Definir a posição do trem no seu trilho de origem
        sem_wait(&posicao);
        if (in_trem->origem == 0){
            posicao_a[i_A] = in_trem->id;
            i_A++;
        }else{
            posicao_b[i_B] = in_trem->id;
            i_B++;
        }
        sem_post(&posicao);      
        
        //Esperar chegar a vez na fila do trilho (Espera bloqueada)

        while (in_trem->origem == 0 && posicao_a[0] != in_trem->id
            || in_trem->origem == 1 && posicao_b[0] != in_trem->id){
            pthread_mutex_lock(&mutex);//TESTAR USO DA CPU COM O WHILE
            pthread_cond_wait(&cond, &mutex);//Caso dê erro adicionar while aqui
            pthread_mutex_unlock(&mutex);//ULTIMO TESTE - 5 %
        }

        //Caso a prioridade não seja ALTA espera um pouco antes de solicitar o acesso
        sem_wait(&cruzamento);
        if (in_trem->prioridade == 1){
            sem_post(&cruzamento);
            usleep(10000);
            sem_wait(&cruzamento);
        }else if(in_trem->prioridade == 2){
            sem_post(&cruzamento);
            usleep(30000);
            sem_wait(&cruzamento);
        }
        //Acesso solicitado
            
        //Passagem do trem pelo cruzamento
        if(INTERFACE==0){
            printf("Trem %d passando pelo cruzamento \n",in_trem->id);
            sleep(1);
            printf("Trem %d passou pelo cruzamento \n",in_trem->id);
        }
        if(INTERFACE==1){
            imprimir_interface(in_trem->id,-1, 0, in_trem->prioridade);
            sleep(1);
        }

        //sem_post(&cruzamento);

        //Atualizando a lista de posições apos o trem passar
        //Aqui a condição do mutex vai ser liberada e atendida
        sem_wait(&posicao);
        pthread_mutex_lock(&mutex);
        if (in_trem->origem == 0){
            for (int i = 0; i < NUM_TRENS-1; i++){
            posicao_a[i] = posicao_a[i+1];
            }
            posicao_a[NUM_TRENS-1] = 0;
            i_A--;
            pthread_cond_broadcast(&cond);
        }else{
            for (int i = 0; i < NUM_TRENS-1; i++){
            posicao_b[i] = posicao_b[i+1];
            }
            posicao_b[NUM_TRENS-1] = 0;
            i_B--;
            pthread_cond_broadcast(&cond);
        }
        pthread_mutex_unlock(&mutex);

        if(INTERFACE==1){
            imprimir_interface(in_trem->id,in_trem->origem, 0, in_trem->prioridade);
            sleep(1);
            imprimir_interface(in_trem->id, -1 , in_trem->destino, in_trem->prioridade);
            sleep(1);
        }

        sem_post(&cruzamento);
        sem_post(&posicao); 

        //Espera para gerar outro trem
        sleep(rand() % 1 + 1);
    }  
}

int main(){

    //Inicializando mutex
    pthread_mutex_init(&mutex, NULL);

    //Inicializando semáforos
    sem_init(&cruzamento,0,1);
    sem_init(&posicao,0,1);

    //Criação dos trens aleatórios / threads
    pthread_t trens[NUM_TRENS];

    srand(time(NULL));

    for (int i = 0; i < NUM_TRENS; i++){

        info_trens[i].id = i + 1;

        pthread_create(&trens[i], NULL, trem, &info_trens[i]);
        
        //Tempo aleatório para criar o processo
        sleep(rand() % 1 + 1);
    }

    for(int i=0; i < NUM_TRENS; i++){
        pthread_join(trens[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    sem_destroy(&cruzamento);
    sem_destroy(&posicao);
    
    return 0;
}