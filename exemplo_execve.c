#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
	int res = 0;
	res = execve("/bin/bash", 0, 0);
	if(res != -1)
	{
		printf("O programa executou corretamente!\n");
	}else{
		printf("Ocorreu um erro durante a execução!\n");
	}
}
