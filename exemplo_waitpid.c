#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/*
Esse programa se utiliza do comando waitpid,
a fim de analisar as syscalls do mesmo.
O comando faz com que um processo não finalize
até que o referenciado finalize.
*/

int main() {
	pid_t pid;	//Declaração de variavel pid
	pid = fork();	//Cria um processo filho identico ao pai, mas com pid diferente

	if(pid == 0)	//Verifica se o pid corresponde ao processo pai ou filho
	{
		usleep(1000);	//Faz o filho "dormir" por um segundo
		printf("Processo filho finaliza\n");
	}else{
		waitpid(0, NULL, 0);	//Syscall que faz um processo esperar outro finalizar
					//Com isso, o processo pai tem que esperar o filho sair
					//do sleep pois não pode terminar antes do filho
		printf("Processo Pai finaliza após esperar o filho finalizar\n");
	}
}
