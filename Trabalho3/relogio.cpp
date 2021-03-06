#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <math.h>
#include <ctime>
#include <climits>

using namespace std;

//Constantes ajustaveis:
//Numero de bits para guardar endereço dentro da pagina
const unsigned int bits_endereco = 8;
//Tamanho das memorias em número de páginas
const unsigned int NUMERO_QUADROS_PRINCIPAL = 4;
const unsigned int NUMERO_QUADROS_SECUNDARIA = 10;

//Constantes geradas automaticamente
const unsigned int NUMERO_PAGINAS_VIRTUAL = NUMERO_QUADROS_PRINCIPAL + NUMERO_QUADROS_SECUNDARIA;
//gera numero da pagina com AND. bits menos significativos. ex: 0b00011111 
const unsigned int MASCARA_ENDERECO = (1<<bits_endereco)-1;
//gera numero da pagina com AND. bits mais significativos.  ex: 0b11100000
const unsigned int MASCARA_PAGINA = ~MASCARA_ENDERECO;
const unsigned int TAM_PAGINA = pow(2,bits_endereco);

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
    //proximos valores dependem de como selecionar as páginas que vão para o disco
    bool referencia;        //acompanha uso
    struct endereco_real* next;
    struct endereco_real* prev;

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
					it_end->second.referencia << ", " <<
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

void print_encadeada(endereco_real* relogio){
    if (relogio == NULL){
        return;
    }
    unsigned int first = relogio->quadro;
    do{
        cout << relogio->quadro << " -->" << endl;
        relogio = relogio->next;
    } while(relogio->quadro != first);
    return;
}

void encerra_processo(string pid, vector<bool> &bit_vector_prim,
        vector<bool> &bit_vector_sec, tabela_processos &tabela_virtual,
        endereco_real* &relogio){
    //verifica se processo existe
    if(tabela_virtual.count(pid) == 0){
		cout << "Processo " << pid << "não foi encerrado porque não existe" << endl;
		return;
	}

    //Libera todos os enderecos virtuais ligados ao processo
    tabela_enderecos::iterator it;
    for(it = tabela_virtual[pid].begin(); it != tabela_virtual[pid].end(); it++){
        if(it->second.residencia == true){
            it->second.next->prev = it->second.prev;
            it->second.prev->next = it->second.next;
            if (relogio->quadro == it->second.quadro){
                if(it->second.next == &it->second){  //Ultimo elemento da lista
                    relogio = NULL;
                }
                else{
                    relogio = it->second.next;
                }
            }
            bit_vector_prim[it->second.quadro] = true;
        }
        else{
            bit_vector_sec[it->second.quadro] = true;
        }
    }

    //Retira o processo da lista
    tabela_virtual.erase(pid);
    cout << "Processo " << pid << " encerrado" << endl;

    return;
}

void cria_processo(string pid, int n_quadros, vector<bool> &bit_vector_prim,
        vector<bool> &bit_vector_sec, tabela_processos &tabela_virtual,
        endereco_real* &relogio ){
    //verifica se processo ja existe
    if(tabela_virtual.count(pid) == 1){
		cout << "Processo " << pid << "não foi criado porque já existe" << endl;
		return;
	}

	//encontra quadros vazios
	for(int i = 0; i < n_quadros; i++){
		unsigned int pos;
        bool primaria;

		//busca um quadro vazio na memória primária
        primaria = false;
		for(pos = 0; pos < bit_vector_prim.size() ;pos++){
            if (bit_vector_prim[pos] == true){
                primaria = true;
                break;
            }
        }

        //se não encontrado, busca um quadro vazio na memória secundária
        if (!primaria){
            for(pos = 0; bit_vector_sec[pos] == false && pos < bit_vector_sec.size() ;pos++)
                ;
        }

        //se não encontrado, encerra o processo
        if(!primaria && bit_vector_sec[pos] == false){
            cout << "Não há memória disponível para nova página" << endl;
            encerra_processo(pid, bit_vector_prim, bit_vector_sec, tabela_virtual, relogio);
            return;
        }

		//ocupa quadro
        tabela_virtual[pid][i].quadro = pos;
        if (primaria){
        	bit_vector_prim[pos] = false;
            tabela_virtual[pid][i].residencia = true;
            
            //Insere página na lista encadeada circular
            if (relogio != NULL){
                tabela_virtual[pid][i].next = relogio;
                tabela_virtual[pid][i].prev = relogio->prev;
                relogio->prev->next = &tabela_virtual[pid][i];
                relogio->prev = &tabela_virtual[pid][i];
            }
            else{
                relogio = &tabela_virtual[pid][i];
                tabela_virtual[pid][i].next = relogio;
                tabela_virtual[pid][i].prev = relogio;
            }

            cout << "Alocado quadro " << pos  << " para " << pid << " na memória primária" << endl;
        }
        else{
            bit_vector_sec[pos] = false;
            tabela_virtual[pid][i].residencia = false;
            cout << "Alocado quadro " << pos  << " para " << pid << " na memória secundária" << endl;
        }
        tabela_virtual[pid][i].referencia = true;

	}
	return;
}

void substitui_pagina(string pid, int pag, tabela_processos &T, endereco_real* &relogio){
    endereco_real* removida;
    unsigned int permutador;

    //Função responsável por implementar algoritmo de substituição relogio
    while( relogio->referencia != 0 ){
        relogio->referencia = 0;
        relogio = relogio->next;
    }
    removida = relogio;

    cout << "Quadro " << removida->quadro << " na memória primária permutado com " <<
            T[pid][pag].quadro << " na secundária" << endl;
    //Permuta as paginas
    permutador = removida->quadro;
    removida->quadro = T[pid][pag].quadro;
    T[pid][pag].quadro = permutador;
    removida->residencia = false;
    T[pid][pag].residencia = true;
    T[pid][pag].referencia = true;

    //Encadeia a nova página
    removida->next->prev = &T[pid][pag];
    removida->prev->next = &T[pid][pag];
    T[pid][pag].next = removida->next;
    T[pid][pag].prev = removida->prev;
    relogio = &T[pid][pag];

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

int realiza_RW(int endereco, string pid, tabela_processos T, bool escreve){  //Recebe as strings arg, pid, a tabela de processos e um booleano
                                                                                                                    //true para indicar escrita e false para leitura
    //Modo mais simples de escrita, quando tudo está em RAM
    int pagina = endereco & MASCARA_PAGINA;
    pagina = pagina >> bits_endereco;
	
    if(T.count(pid) == 0){
        cout << "Acesso inválido, processo " << pid << " inexistente!" << endl;
        return -1;
    }
    else{ 
        if(T[pid].count(pagina) == 0){
            cout << "Acesso inválido da pagina " << pagina << " pelo processo " << pid << "!" << endl;
            return -2;
        }
    }

    if(T[pid][pagina].residencia == true){
	    T[pid][pagina].referencia = true;

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
        /*cout << "É necessário implementar função que libera memória principal " <<
        "e traz o processo da memória secundária, portanto necessário algoritmo " <<
        "de substituição" << endl;*/

        return pagina;
    }

    return -1;
    //A função retorna true se não houver necessidade de encerrar o processo, do contrário retorna false
}

bool hdtoram(string pid, int pagina, tabela_processos &T, vector<bool> &prim, vector<bool> &sec, endereco_real* &relogio){
    for(int i = 0; i < prim.size(); i++){
        if (prim[i] == true){
            prim[i] = false;
            sec[T[pid][pagina].quadro] = true;
            T[pid][pagina].quadro = i;
            T[pid][pagina].residencia = true;
            T[pid][pagina].referencia = true;

            T[pid][pagina].next = relogio;
            T[pid][pagina].prev = relogio->prev;
            relogio->prev->next = &T[pid][pagina];
            relogio->prev = &T[pid][pagina];

            cout << "Página "<< pagina << " do processo " << pid << " alocado em RAM no quadro " << i << "!" << endl;
            return true;
        }
    }

    return false;
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
    endereco_real* ponteiro_relogio = NULL;

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
				int n_paginas = ceil(stoi(arg)/float(TAM_PAGINA));
				cria_processo( pid, n_paginas, prim_mem, sec_mem, tabela_virtual, ponteiro_relogio);
                break;
            }

            case 'W': 
            {
                int endereco = extrai_endereco(arg);
                int pagina = realiza_RW(endereco, pid, tabela_virtual, true);
                if(pagina == -2)
                    encerra_processo(pid, prim_mem, sec_mem, tabela_virtual, ponteiro_relogio);
                else if (pagina >= 0)
                {
                    if(!hdtoram(pid, pagina, tabela_virtual, prim_mem, sec_mem, ponteiro_relogio)){
                        substitui_pagina(pid, pagina, tabela_virtual, ponteiro_relogio);
                    }
                    realiza_RW(endereco, pid, tabela_virtual, true);
                }
                break;
            }
            case 'R': 
            {
                int endereco = extrai_endereco(arg);
                int pagina = realiza_RW(endereco, pid, tabela_virtual, false);
                if(pagina == -2)
                    encerra_processo(pid, prim_mem, sec_mem, tabela_virtual, ponteiro_relogio);
                else if (pagina >= 0)
                {
                    if(!hdtoram(pid, pagina, tabela_virtual, prim_mem, sec_mem, ponteiro_relogio)){
                        substitui_pagina(pid, pagina, tabela_virtual, ponteiro_relogio);
                    }
                    realiza_RW(endereco, pid, tabela_virtual, false);
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
    print_encadeada(ponteiro_relogio);
    cout << endl;
    return 0;
}
