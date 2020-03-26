#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

/*
Este programa tem como função utilizar o comando
fork() e observar suas syscalls, este comando cria
um processo filho que é uma copia do pai, porém com
um pid diferente
*/

int main(){
	pid_t pid;	//Declaração de variável pid
	pid = fork();	//Cria processo filho copia do pai
	if(pid == 0)	//Se o pid for 0, o processo é o filho, senão é o pai
		printf("sou o filho\n");
	else{
		usleep(1000);	//Sleep faz com que o filho termine primeiro que o pai
				//Se for retirado acontece o contrário pois o processo pai iniciou primeiro
		printf("sou o pai\n");
	}
	return 0;
}
