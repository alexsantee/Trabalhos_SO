#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
	pid_t pid;
	pid = fork();

	if(pid == 0)	//Verifica se o pid corresponde ao processo pai ou filho
	{
		usleep(1000);	//Faz o filho "dormir" por um segundo
		printf("Processo filho finaliza\n");
	}else{
		waitpid(0, NULL, 0);	//Syscall que faz um processo esperar outro finalizar
		printf("Processo Pai finaliza após esperar o filho finalizar\n");
	}
}
