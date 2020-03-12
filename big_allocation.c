/*
 * Esse programa exemplifica as syscalls
 * mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) e 
 * munmap(void *addr, size_t length)
 *
 * A syscall mmap() cria um novo espaco de enderecamento na memoria virtual do
 * programa e retorna um ponteiro para o inicio dele. O sistema operacional eh
 * responsavel por fazer a alocacao.
 * A syscall munmap() retira essa regiao de memoria do espaco de enderecamento
 * do programa, retornando a memoria para o sistema.
 *
 * No caso do malloc da glibc, grandes alocacoes de memoria acontecem
 * fora da heap e sao mapeadas em outra regiao com a chamada do mmap().
 * O comando free desfaz a alocacao da memoria e faz chamada para o munmap
 *
 */

#include<stdlib.h>

int main(){
    char *string;
    string = malloc(sizeof(char)*(64*1024*1024));
    free(string);
    return 0;
}
