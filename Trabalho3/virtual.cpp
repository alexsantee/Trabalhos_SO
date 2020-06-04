#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <math.h>
#include <ctime>

using namespace std;

//Constantes ajustaveis:
//Numero de bits para guardar endereço dentro da pagina
const unsigned int bits_endereco = 8;
//Tamanho das memorias em número de páginas
const unsigned int NUMERO_QUADROS_PRINCIPAL = 1;
const unsigned int NUMERO_QUADROS_SECUNDARIA = 4;

//Constantes geradas automaticamente
const unsigned int NUMERO_PAGINAS_VIRTUAL = NUMERO_QUADROS_PRINCIPAL + NUMERO_QUADROS_SECUNDARIA;
//gera numero da pagina com AND. bits menos significativos. ex: 0b00011111 
const unsigned int MASCARA_ENDERECO = (1<<bits_endereco)-1;
//gera numero da pagina com AND. bits mais significativos.  ex: 0b11100000
const unsigned int MASCARA_PAGINA = ~MASCARA_ENDERECO;

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
    clock_t ultimo_uso;        //acompanha tempo de uso
};

typedef struct endereco_real endereco_real;
typedef map<unsigned int, struct endereco_real> tabela_enderecos;
typedef map<string, tabela_enderecos> tabela_processos;

void print_tabela_processos(tabela_processos &tabela_virtual){

	tabela_processos::iterator it_proc;
	//Imprime cada processo
	for(it_proc = tabela_virtual.begin(); it_proc != tabela_virtual.end(); it_proc++){
		cout << it_proc->first << ":" << endl;
		tabela_enderecos::iterator it_end;
		//Imprime cada associacao virtual->real
		for(it_end = it_proc->second.begin(); it_end != it_proc->second.end(); it_end++){
			cout << "\t" << it_end->first << " - " << 
					it_end->second.quadro << ", " <<
					it_end->second.ultimo_uso << ", " <<
					it_end->second.residencia << endl;
		}
	}
	return;
}

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
    //verifica se processo ja existe
    if(tabela_virtual.count(pid) == 1){
		cout << "Processo " << pid << "não foi criado porque já existe" << endl;
		return;
	}

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
			tabela_virtual[pid][i].ultimo_uso = clock();

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

bool realiza_RW(int endereco, string pid, tabela_processos T, bool escreve){  //Recebe as strings arg, pid, a tabela de processos e um booleano
                                                                            //true para indicar escrita e false para leitura
    //Modo mais simples de escrita, quando tudo está em RAM
    int pagina = endereco & MASCARA_PAGINA;
    pagina = pagina >> bits_endereco;
	
    if(T.count(pid) == 0){
        cout << "Acesso inválido, processo " << pid << " inexistente!" << endl;
        return true;
    }
    else{ 
        if(T[pid].count(pagina) == 0){
            cout << "Acesso inválido da pagina " << pagina << " pelo processo " << pid << "!" << endl;
            return false;
        }
    }

    if(T[pid][pagina].residencia == true){
	    //Atualiza tempo de acesso
	    T[pid][pagina].ultimo_uso = clock();

        endereco_real end = T[pid][pagina];
        if(escreve){
            cout << "Escrita na pagina " << pagina <<
	        		", correspondente ao quadro " << end.quadro <<
	        		", no endereco " << (endereco&MASCARA_ENDERECO) << endl;
        } else{
            cout << "Leitura na pagina " << pagina <<
	        		", correspondente ao quadro " << end.quadro <<
	        		", no endereco " << (endereco&MASCARA_ENDERECO) << endl;
        }
    }else{
        cout << "É necessário implementar função que libera memória principal " <<
        "e traz o processo da memória secundária, portanto necessário algoritmo " <<
        "de substituição" << endl;
    }

    return true;
    //A função retorna true se não houver necessidade de encerrar o processo, do contrário retorna false
}

int main(){
    //Declara vetor de bits dos quadros de páginas na memória principal
    vector<bool> prim_mem;
    //Inicializado como 1 (todos os quadros livres)
    prim_mem.assign(NUMERO_QUADROS_PRINCIPAL, true);

    //Declara vetor de bits dos quadros de páginas na memória secundária
    vector<bool> sec_mem;
    //Inicializando como 1 (todos os quadros livres)
    sec_mem.assign(NUMERO_QUADROS_SECUNDARIA, true);
    //NUM_QUADROS aqui deve assumir outro valor não?

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
				int n_paginas = (stoi(arg) & MASCARA_PAGINA) >> bits_endereco;
				cria_processo( pid, n_paginas, prim_mem, tabela_virtual);
                break;
            }

            case 'W': 
            {
                int endereco = extrai_endereco(arg);
                if(!realiza_RW(endereco, pid, tabela_virtual, true)){
                    encerra_processo(pid, prim_mem, tabela_virtual);
                    cout << "Processo " << pid << " encerrado!" << endl;
                }
                break;
            }
            case 'R': 
            {
                int endereco = extrai_endereco(arg);
                if(!realiza_RW(endereco, pid, tabela_virtual, false)){
                    encerra_processo(pid, prim_mem, tabela_virtual);
                    cout << "Processo " << pid << " encerrado!" << endl;
                }
                break;
            }
            case 'P': cout << "Faz o comando de instrução da CPU" << endl; break;
            case 'I': cout << "Faz o comando de instrução de I/O" << endl; break; 
        }
    }

    entrada.close();

    //Util no debugging
    cout << endl << "prim_mem:" << endl;
	for(unsigned int i = 0; i < prim_mem.size(); i++)
	    cout << prim_mem[i];
    cout << endl << "sec_mem:" << endl;
	for(unsigned int i = 0; i < sec_mem.size(); i++)
	    cout << sec_mem[i];
    cout << endl << "tabela de processos:" << endl;
	print_tabela_processos(tabela_virtual);
    cout << endl << "MASCARA_ENDERECO:" << endl;
    print_binario(MASCARA_ENDERECO);
    cout << endl << "MASCARA_PAGINA:" << endl;
    print_binario(MASCARA_PAGINA);
    cout << endl;
    return 0;
}
