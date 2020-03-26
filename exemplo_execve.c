#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//Esse programa utiliza o comando execve,
//este chama uma série de syscalls que realizam a
//execução do programa passado como parâmetro, nesse
//caso, a Bash

int main(){
	int res;
	res = execve("/bin/bash", 0, 0);   //Utilização do execve
	if(res != -1)	//Caso a resposta retorne -1, significa que o execve falhou
	{
		printf("O programa executou corretamente!\n");
	}else{
		printf("Ocorreu um erro durante a execução!\n");
	}
}
