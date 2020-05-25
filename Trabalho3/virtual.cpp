#include <iostream>
#include <fstream>
#include <vector>

//Constantes das páginas de memória virutal
const unsigned int NUM_PAGINAS = 64;
const unsigned int TAM_PAGINA  = 1024;
//gera numero da pagina com AND. bits menos significativos. ex: 0b00011111 
const unsigned int MASCARA_ENDERECO = 0xffffffff + TAM_PAGINA;
//gera numero da pagina com AND. bits mais significativos.  ex: 0b11100000
const unsigned int MASCARA_PAGINA = (0xffffffff ^ MASCARA_ENDERECO) + TAM_PAGINA*NUM_PAGINAS;

//Constanted dos quadros de páginas na memória principal
const unsigned int NUM_QUADROS = 32;    //deve ser menor que NUM_PAGINAS
const unsigned int TAM_QUADRO = TAM_PAGINA;
const unsigned int MASCARA_QUADRO = (0xffffffff ^ MASCARA_ENDERECO) + TAM_QUADRO*NUM_QUADROS;

using namespace std;

//Ajuda para verificar as máscaras, imprime elas em binário
void print_binario(unsigned int n){
    cout << "0b";
    unsigned int mascara = 0x80000000;
    for(int i = 0; i<32; i++){
        if( n & mascara ) cout << "1";
        else cout << "0";
        mascara >>= 1;
    }
}

int main(){
    //Declara vetor de bits dos quadros de páginas na memória principal
    vector<bool> bit_map;
    //Inicializado como 1 (todos os quadros livres)
    bit_map.assign(NUM_QUADROS, true);
    
    ifstream entrada;
    entrada.open("entrada.txt");
    string linha;
    while( getline(entrada,linha) ){
        cout << linha << endl;
    }
    if(!entrada.is_open()) exit(-1);

    entrada.close();

    cout << endl;
    print_binario(MASCARA_ENDERECO);
    cout << endl;
    print_binario(MASCARA_PAGINA);
    cout << endl;
    print_binario(MASCARA_QUADRO);
    cout << endl;
    return 0;
}
