/*
 * Esse programa exemplifica um processo do tipo io bound.
 *
 * O que ele faz eh simplesmente repetir o que o usuario digita no terminal,
 * o que depende bastante de esperar por uma entrada porque o tempo de o 
 * usuario digitar eh muito grande frente a velocidade do processador
 *
 * A entrada deve ser encerrada com Ctrl+d para o programa finalizar
 */

#include<stdio.h>

int main(){
    int c;
    while( (c = getchar()) != EOF ){
        putchar(c);
    }
    return 0;
}
