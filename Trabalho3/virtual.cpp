#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <math.h>
#include <ctime>

using namespace std;

//Constantes ajustaveis:
//numero de bits para armazenar cada grandeza
const unsigned int bits_tamanho = 9;
const unsigned int bits_pagina = 5;
const unsigned int bits_quadro = 3;

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
    time_t ultimo_uso;        //acompanha tempo de uso
};

typedef struct endereco_real endereco_real;
typedef map<unsigned int, struct endereco_real> tabela_enderecos;
typedef map<string, tabela_enderecos> tabela_processos;

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

void encerra_processo(string pid, vector<bool> &bit_vector, tabela_processos &tabela_virtual){

    //Libera todos os enderecos virtuais ligados ao processo
    tabela_enderecos processo = tabela_virtual[pid];
    tabela_enderecos::iterator it;
    for(it = processo.begin(); it != processo.end(); it++){
        bit_vector[it->second.quadro] = true;
    }
    //Retira o processo da lista
    tabela_virtual.erase(pid);

    return;
}

void cria_processo(string pid, int n_quadros, vector<bool> &bit_vector, tabela_processos &tabela_virtual){
	//encontra quadros vazios
	for(int i = 0; i < n_quadros; i++){
		unsigned int pos;

		//encontra um quadro vazio
		for(pos = 0; bit_vector[pos] == false && pos < bit_vector.size() ;pos++)
			;

		//ocupa quadro se disponivel
		if(bit_vector[pos] == true){
			bit_vector[pos] = false;
			cout << "Alocado quadro " << pos  << " para " << pid << endl;
			//associa quadro ao processo
            tabela_virtual[pid][i].quadro = pos;
			tabela_virtual[pid][i].residencia = true;
			tabela_virtual[pid][i].ultimo_uso = time(NULL);

		}
		//caso nao exista memoria
		else{
			cout << "Não há memória disponível para nova página, encerrando " << pid << endl;
            encerra_processo(pid, bit_vector, tabela_virtual);

		}
	}
	return;
}

int extrai_endereco(string arg){
	int n1 = arg.find('(');
	int n2 = arg.find(')');
	char aux[n2-n1];
	int tam = arg.copy(aux,n2-n1-1, n1+1);
	aux[tam] = '\0';

	//Converte string de numero para inteiro correspondente
	return stoi(aux);
}

int main(){
    //Declara vetor de bits dos quadros de páginas na memória principal
    vector<bool> bit_vector;
    //Inicializado como 1 (todos os quadros livres)
    bit_vector.assign(NUM_QUADROS, true);
   
    //Declara tabela de mapeamento virtual
    tabela_processos tabela_virtual;

	//Abre lista de comandos
    ifstream entrada;
    entrada.open("entrada.txt");
    if(!entrada.is_open()) exit(-1);

	//Enquanto houver comandos
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

		//Print de debugging
        cout << pid << " " << command << " " << arg << endl;
        switch(command.at(0)){
            case 'C': 
            {
				int n_paginas = ceil(stof(arg)/TAM_QUADRO);
				cria_processo( pid, n_paginas, bit_vector, tabela_virtual);
				break;
            }

            case 'W': 
            {
                //Modo mais simples de leitura, quando tudo está em RAM
				int endereco = extrai_endereco(arg);

                int pagina = endereco & MASCARA_PAGINA;
                pagina = pagina >> bits_tamanho;

				//FALTA VERIFICAR SE O ENDERECO ESTA NA MEMORIA DO PROCESSO!
                endereco_real end = tabela_virtual[pid][pagina];
                cout << "Escrita na pagina " << pagina <<
						", correspondente ao quadro " << end.quadro <<
						", no endereco " << (endereco&MASCARA_ENDERECO) << endl;
            }
            case 'R': cout << "Faz o comando de leitura" << endl; break;
            case 'P': cout << "Faz o comando de instrução da CPU" << endl; break;
            case 'I': cout << "Faz o comando de instrução de I/O" << endl; break; 
        }
    }

    entrada.close();

    //Util no debugging
    cout << endl << "bit_vector:" << endl;
	for(unsigned int i = 0; i < bit_vector.size(); i++)
	    cout << bit_vector[i];
    cout << endl << "MASCARA_ENDERECO:" << endl;
    print_binario(MASCARA_ENDERECO);
    cout << endl << "MASCARA_PAGINA:" << endl;
    print_binario(MASCARA_PAGINA);
    cout << endl << "MASCARA QUADRO:" << endl;
    print_binario(MASCARA_QUADRO);
    cout << endl;
    return 0;
}
