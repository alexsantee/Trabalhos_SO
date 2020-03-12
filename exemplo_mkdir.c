#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SUCESSO 0
#define ERRO -1

int main()
{
    int err;

    err = mkdir("Pasta");

    if(err == ERRO)
    {
        printf("Erro ao criar pasta.");
        return ERRO;
    }

    return SUCESSO;
}
