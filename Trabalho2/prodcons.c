/*
 *	Esse programa imprime uma sequência de bytes aleatoria utilizando de um modelo
 *	produtor/consumidor entre quem gera o numero aleatorio e quem o escreve na tela  
 *
 *  Da forma que esta implementado, nao pode haver mais de um produtor ou consumidor
 *  Por causa de nao proteger as variaveis produzido e consumido usadas no while principal
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

//define tamanho da fila e quantos itens produzir no total
#define TAM_BUFFER 7
#define NUM_ITENS 240

//buffer de itens
int buffer[TAM_BUFFER] = {0};
//contadores de itens no buffer e total produzido/consumido
int n_disponiveis=0;
int produzido=0;
int consumido=0;
//ponteiros para inserir/remover itens do buffer
int proximo_a_colocar = 0;
int proximo_a_retirar = 0;


//locks compartilhadas
pthread_mutex_t item_buff_cond_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_nao_cheio = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_nao_vazio = PTHREAD_COND_INITIALIZER;

//gera um valor para colocar na fila
int cria_item(void) {
	int item = rand() % 256;
	return(item);
}

//guarda um item no buffer e atualiza posicao
void insere_item(int item) {
	//printf("colocando %4d no buffer (i=%4d)\n", item, proximo_a_colocar);
	//fflush(stdout);
	buffer[proximo_a_colocar] = item;
	proximo_a_colocar = (proximo_a_colocar + 1) % TAM_BUFFER;
	produzido++;
	n_disponiveis++;
	printf("Item adicionado, tamanho do buffer = %d\n", n_disponiveis);
	fflush(stdout);
	return;
}

//utiliza um item recebido (imprime hex no stderr)
void usa_item(int item) {
	printf("Item usado: %x\n", item);
	fflush(stdout);
	return;
}

//remove um item no buffer e atualiza posicao
int retira_item() {
	int item = buffer[proximo_a_retirar];
	//printf("retirando %4d do buffer (i=%4d)\n", item, proximo_a_retirar);
	//fflush(stdout);
	proximo_a_retirar = (proximo_a_retirar + 1) % TAM_BUFFER;
	consumido++;
	n_disponiveis--;
	printf("Item removido, tamanho do buffer = %d\n", n_disponiveis);
	fflush(stdout);
	return item;
}

void *produtor(void) {
	int item;
	//coloca NUM_ITENS no buffer
	while (produzido < NUM_ITENS ) {
		//cria um novo item
		item = cria_item();

		//coloca o item no buffer
		pthread_mutex_lock(&item_buff_cond_lock);
		//se buffer estiver cheio entra em espera
		while (n_disponiveis == TAM_BUFFER){
			pthread_cond_wait(&cond_nao_cheio, &item_buff_cond_lock);
		}
		insere_item(item);
		//logo apos colocar um item, o buffer nao esta vazio
        pthread_cond_signal(&cond_nao_vazio);
        pthread_mutex_unlock(&item_buff_cond_lock);
	}
	pthread_exit(0);
}

void *consumidor(void) {
	int item;
	//retira NUM_ITENS no buffer
	while (consumido < NUM_ITENS ) {
		//retira um item do buffer
		pthread_mutex_lock(&item_buff_cond_lock);
		//se buffer estiver vazio entra em espera
		while (n_disponiveis == 0){
			pthread_cond_wait(&cond_nao_vazio, &item_buff_cond_lock);
		}
		item = retira_item();
		//logo apos retirar um item, o buffer nao esta cheio
        pthread_cond_signal(&cond_nao_cheio);
        pthread_mutex_unlock(&item_buff_cond_lock);

		//usa o item retirado
		usa_item(item);
	}
	pthread_exit(0);
}

int main(void) {
	//inicializa gerador de aleatorios
	srand(time(NULL));

	//cria thread de produtor
	pthread_t thread_produtor;
	if (pthread_create(&thread_produtor, 0, (void *) produtor, (void *) 0) != 0) { 
		printf("Erro ao criar produtor\n");
		exit(-1);
	}

	//cria thread consumidor
	pthread_t thread_consumidor;
	if (pthread_create(&thread_consumidor, 0, (void *) consumidor, (void *) 0) != 0) { 
		printf("Erro ao criar consumidor\n");
		exit(-2);
	}

	//aguarda fim das outras threads
	pthread_join(thread_produtor, 0);
	pthread_join(thread_consumidor, 0);

	//coloca newline para finalizar
	putchar('\n');

	return 0;	
}
