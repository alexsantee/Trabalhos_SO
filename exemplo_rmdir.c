/*
* Esse programa exemplifica o uso da syscall
* 
* rmdir(const char* pathname).
*
* Ao executar o programa, ele tenta remover
* um diretorio vazio chamado "Pasta" no diretorio de trabalho.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SUCESSO 0
#define ERRO -1

int main()
{
    int err;

    err = rmdir("Pasta");

    if(err == ERRO) // se err eh -1, nao foi possivel excluir o diretorio
    {
        printf("Erro ao remover pasta.");
        return ERRO;
    }

    return SUCESSO; // senao, o diretorio foi removido com sucesso
}
