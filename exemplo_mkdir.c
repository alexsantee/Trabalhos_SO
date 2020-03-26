/*
* Esse programa exemplifica o uso da syscall
* 
* mkdir(const char* pathname, mode_t mode).
*
* Ao executar o programa, ele tenta criar
* um diretorio chamado "Pasta" no diretorio de trabalho.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SUCESSO 0
#define ERRO -1

int main()
{
    int err;

    err = mkdir("Pasta");

    if(err == ERRO) // se err eh -1, significa que nao foi possivel criar a pasta
    {
        printf("Erro ao criar pasta.");
        return ERRO;
    }

    return SUCESSO; // senao, foi criado com sucesso
}
