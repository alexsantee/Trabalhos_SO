#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

int main(){
	pid_t pid;
	pid = fork();
	if(pid == 0)
		printf("sou o filho\n");
	else{
		usleep(1000);
		printf("sou o pai\n");
	}
	return 0;
}
