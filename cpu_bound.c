/*
 * Esse programa exemplifica um processo do tipo cpu bound.
 *
 * O que ele faz eh gerar duas matrizes aleatorias e realizar a multiplicacao,
 * um processo que demanda bastante do processador, mas que nao envolve entrada
 * e saida
 */

#include<stdlib.h>

#define MATRIX_SIZE 500
#define MAX_NUMBER 1024

int main(){
    srand(0);
    float A[MATRIX_SIZE][MATRIX_SIZE];
    float B[MATRIX_SIZE][MATRIX_SIZE];
    float C[MATRIX_SIZE][MATRIX_SIZE];

    //inicializacao das matrizes A e B
    for(int i = 0; i < MATRIX_SIZE; i++){
        for(int j = 0; j < MATRIX_SIZE; j++){
            A[i][j] = rand() % MAX_NUMBER;
            B[i][j] = rand() % MAX_NUMBER;
        }
    }

    //calculo da  matriz C = A*B
    for(int i = 0; i<MATRIX_SIZE; i++){
        for(int j = 0; j<MATRIX_SIZE; j++){
            C[i][j] = 0.0f;
            for(int k = 0; k < MATRIX_SIZE; k++){
                C[i][j] += A[i][k]*B[k][j];
            }
        }
    }

}
