#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <math.h>

//Constantes ajustaveis:
//numero de bits para armazenar cada grandeza
const unsigned int bits_tamanho = 10;
const unsigned int bits_pagina = 6;
const unsigned int bits_quadro = 5;

//Gerados automaticamente:
//Memória Virtual
const unsigned int NUM_PAGINAS = 1 << bits_pagina;
const unsigned int TAM_PAGINA  = 1 << bits_tamanho;
//gera numero da pagina com AND. bits menos significativos. ex: 0b00011111 
const unsigned int MASCARA_ENDERECO = 0xffffffff + TAM_PAGINA;
//gera numero da pagina com AND. bits mais significativos.  ex: 0b11100000
const unsigned int MASCARA_PAGINA = (0xffffffff ^ MASCARA_ENDERECO) + TAM_PAGINA*NUM_PAGINAS;

//Memoria Real
const unsigned int NUM_QUADROS = 1 << bits_quadro;
const unsigned int TAM_QUADRO = TAM_PAGINA;
const unsigned int MASCARA_QUADRO = (0xffffffff ^ MASCARA_ENDERECO) + TAM_QUADRO*NUM_QUADROS;

/*
 *  Estrutura da tabela de endereçamento virtual:
 *  Tabela ---> PID ---> Página Virtual ---> (Quadro Real, bit residência, bit referência)
 *                                           \___________________________________________/
 *                                                      struct - endereco real
 *                       \_______________________________________________________________/
 *                                             mapa - tabela_enderecos
 *              \________________________________________________________________________/
 *                                     mapa - tabela_processos
 */

struct endereco_real{
    unsigned int quadro;    //Valor do quadro de página
    bool residencia;        //true se na memoria principal e false se na secundária
    //proximo valor depende de como selecionar as páginas que vão para o disco
    float ultimo_uso;        //acompanha tempo de uso
};

typedef struct endereco_real endereco_real;
typedef std::map<unsigned int, struct endereco_real> tabela_enderecos;
typedef std::map<std::string, tabela_enderecos> tabela_processos;

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
   
    //Declara tabela de mapeamento virtual
    tabela_processos tabela_virtual;

    ifstream entrada;
    entrada.open("entrada.txt");
    if(!entrada.is_open()) exit(-1);

    string s;
    while( getline(entrada,s, ' ') ){
        //Parsing da linha
        string pid;
        string command;
        string arg;
        pid = s;
        getline(entrada,s,' ');
        command = s;
        getline(entrada,s,'\n');
        arg = s;

        cout << pid << " " << command << " " << arg << endl;
        if(command == "C"){     //criação de processo
            int n_quadros = ceil(stof(arg)/TAM_QUADRO);
            //encontra paginas vazia
            for(int i = 0; i < n_quadros; i++){
                unsigned int pos;
                for(pos = 0; bit_map[pos] == false && pos < bit_map.size() ;pos++)
                    ;

                if(bit_map[pos] == true){ //quadro disponível
                    bit_map[pos] = false;
                    cout << "Alocado quadro " << pos << " (mentira, falta implementar)" << endl;
                    //associa quadro ao processo
                }
                else{
                    cout << "Não há memória disponível (falta implementar)";
                }
            }
        }

    }

    entrada.close();

    //Util no debugging
    cout << endl << "MASCARA_ENDERECO:" << endl;
    print_binario(MASCARA_ENDERECO);
    cout << endl << "MASCARA_PAGINA:" << endl;
    print_binario(MASCARA_PAGINA);
    cout << endl << "MASCARA QUADRO:" << endl;
    print_binario(MASCARA_QUADRO);
    cout << endl;
    return 0;
}
