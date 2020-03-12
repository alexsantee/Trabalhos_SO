/*
 * Esse programa exemplifica a syscall
 * brk(void *addr)
 *
 * A syscall brk(addr) define o endereco addr como novo final do data segment
 * e o sistema operacional eh responsavel por alocar e desalocar memoria para
 * compensar o aumento/reducao da memoria do programa.
 *
 * No caso do malloc da glibc, pequenas alocacoes de memoria acontecem na heap,
 * que fica dentro da memoria do programa. Para aumentar o tamanho da heap ele
 * faz o uso da syscall brk()
 *
 */

#include<stdlib.h>

int main(){
    char *string;
    string = malloc(sizeof(char)*1024);
    free(string);
    return 0;
}
