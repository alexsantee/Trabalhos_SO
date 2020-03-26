/*
* Esse programa exemplifica o uso das syscalls
*
* open(const char* pathname, int flags, mode_t mode)
*
* read(int fd, const void *buf, size_t count)
*
* close(int fd)
*
* Ao executar o programa, ele cria um arquivo chamado Arquivo de extensao
* txt, escreve "Hello" e depois fecha o arquivo. O arquivo fica salvo no
* diretorio de trabalho.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SUCESSO 0
#define ERRO -1
#define TAM 5

int main()
{
    FILE * fp;
    int err;

    fp = (FILE *) fopen("Arquivo.txt", "wt"); // cria um arquivo para escrita

    if(fp == NULL)
    {
        printf("Erro ao abrir arquivo."); // se o descritor for nulo, entao houve erro na criacao
        return ERRO;
    }

    err = fwrite("Hello", 1, TAM, fp); // escreve Hello no arquivo

    if(err != TAM)
    {
        printf("Erro ao escrever."); // se o numero de caracteres for diferente, entao houve erro
        return ERRO;
    }

    fclose(fp); // fecha o arquivo

    return SUCESSO;
}
