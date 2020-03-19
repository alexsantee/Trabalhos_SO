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

    fp = (FILE *) fopen("Arquivo.txt", "wt");

    if(fp == NULL)
    {
        printf("Erro ao abrir arquivo.");
        return ERRO;
    }

    err = fwrite("Hello", 1, TAM, fp);

    if(err != TAM)
    {
        printf("Erro ao escrever.");
        return ERRO;
    }

    fclose(fp);

    return SUCESSO;
}
