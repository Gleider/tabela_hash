#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct nodo{
	char dado[101];
	struct nodo *prox;
}typedef Nodo;

struct hash{
	int qtd, tamanho;
	Nodo **vetor;
}typedef Hash;

//Declaração das funções
Hash* novaHash(int tam);
int liberaHash(Hash *oldH);
unsigned int rotation(char string[101], int tam);
void insere(Hash* ha, char palavra[101], int tipo, FILE *fpW, int rehash);
void busca(Hash* ha, char palavra[101], int tipo, FILE *fpW, int delete);
void *rehash(Hash* ha, int t, FILE *fpW);
unsigned int hashPrimaria(Hash* ha, unsigned int k, int i);
unsigned int hashSecundaria(Hash* ha, unsigned int k);
int hashQuadratica(Hash* ha, unsigned int k, int i);
unsigned int hashDuplo(Hash* ha, unsigned int k, int i);

//função principal
int main( int argc, char *argv[])  {

    char funcao[10], string[110], stringAux[101];
    int aux=0, m, i, tipo;
    Hash *ha = novaHash(500);
    
   if( argc == 4 ) {
      printf("\n------------------------\n\n%s Começando... \n", argv[0]);
      
      	FILE *fpR; //ponteiro para arquivo de entrada
      	if((fpR = fopen(argv[1], "r"))==NULL)
      		printf("Erro ao abrir arquivo de leitura! \n");
      		
    	FILE *fpW; // ponteiro para arquivo de saida
      	if((fpW = fopen(argv[2], "w"))==NULL)
     		printf("Erro ao abrir ou criar arquivo de escrita! \n");
     	
     	if(strcmp(argv[3], "-encadeamento") == 0)
			tipo = 0;
		else if(strcmp(argv[3], "-linear") == 0)
			tipo = 1;
		else if(strcmp(argv[3], "-quadratica") == 0)
			tipo = 2;
		else if(strcmp(argv[3], "-hash_duplo") == 0)
			tipo = 3;
		else
			tipo = 4;
			
     	if(tipo != 4){	
			printf("\n---------------------\n\nPrograma utilizando %s. \n", argv[3]);
			
			while (!feof(fpR)){ // percorre até o fim do arquivo pegando as entradas, aqui deve ser tratado o uso das funções
					fgets(string, sizeof(string), fpR);
					
					for(m=0;string[m]!=' ';m++){
						funcao[m] = string[m];
					}
					funcao[m] = '\0';
					m+=2;
					i=0;
					
					while(string[m] != '"'){
						stringAux[i] = string[m];
						m++;
						i++;
					}
					stringAux[i] = '\0';
				
					if (strcmp(funcao, "INSERT") == 0){
						fprintf(fpW, "%s %s %u ", funcao, stringAux, rotation(string, strlen(string)));
						insere(ha, stringAux, tipo, fpW, 0);	//'0' no último argumento indica que não é uma rehash
		
					}
				    else if(strcmp(funcao, "GET") == 0){
					    fprintf(fpW, "%s %s %u ", funcao, stringAux, rotation(string, strlen(string)));
					    busca(ha, stringAux, tipo, fpW, 0);     //'0' no último argumento indica que será feita a busca e não será deletado
					    
				   }
				    else if(strcmp(funcao, "DELETE") == 0){
					    fprintf(fpW, "%s %s %u ", funcao, stringAux, rotation(string, strlen(string)));
					    busca(ha, stringAux, tipo, fpW, 1);		//'1' no último argumento indica que será feita a busca e será deletado
					}
					else{
						 printf("\n----------------------\n\nArquivo escrito.\n");
					}
					strcpy(funcao,"");
					strcpy(string,"");
					strcpy(stringAux,"");
					aux++;
					
					ha = rehash(ha, tipo, fpW);
			}
		}
		else{
			printf("Tipo inválido!\n");
		}
	}
	else if( argc > 4 ) {
		printf("Muitos argumentos para o inicio do programa! \n");
	}
	else {
		printf("Faltam argumentos para o inicio do programa! \n");
	}
	
	liberaHash(ha);
	printf("\n----------------------\n\nFim do programa.\n\n");
	exit(0);
}

//Função pra criação da tabela HASH
Hash* novaHash(int tam){
	printf("\n------------------------\n\nHash Criada.\n");
	Hash *newH =(Hash*)calloc(1, sizeof(Hash));
	if (!newH){
		printf ("** Erro: Memoria Insuficiente para criar a Hash **\n");
        return NULL;
    }
    else{
		newH->tamanho = tam;
		newH->qtd = 0;
		newH->vetor = (Nodo**)calloc(tam, sizeof(Nodo*));
		
		if(!newH->vetor){
			printf ("** Erro: Memoria Insuficiente para criar o vetor da Hash **\n");
			return NULL;
		}
		else{
			int i=0;
			for(i=0; i<newH->tamanho; i++){
				newH->vetor[i] = NULL;
			}
		}
	}
	
	return newH;
}

//Função para eliminar a tabela Hash
int liberaHash(Hash *oldH){
	if(oldH == NULL)
		return 0;
	else{
		int i;
		for(i=0; i<oldH->tamanho; i++){
			if(oldH->vetor[i]!= NULL){
				free(oldH->vetor[i]);
			}
		}
		free(oldH->vetor);
		free(oldH);
		return 1;
	}
}

//função de geração de um inteiro para armazenar string na Hash
unsigned rotation(char *string, int tam){  
	char *p = string;
	unsigned h = 0;
	int i;
	
	for(i=0;i<tam;i++){
		if(p[i] != ' ')
			h = (h << 4) ^ (h >> 28) ^ p[i];
	}
	
	return h;
}

//inserir string
void insere(Hash* ha, char palavra[101], int tipo, FILE *fpW, int rehash){
	int pos, t=0, i=0;
	unsigned int k;
	
	k = rotation(palavra, strlen(palavra));
	while(t==0){
		switch(tipo){
			case 0: pos = hashPrimaria(ha, k, 0); break;	// tipo encadeamento
			case 1: pos = hashPrimaria(ha, k, i); break; // tipo linear
			case 2: pos = hashQuadratica(ha, k, i); break;	//tipo quadratico
			case 3: pos = hashDuplo(ha, k, i); break;	// tipo hash_duplo
		}
		
		if(tipo != 0){			//Caso não seja encadeamento
			if(i == 0 && rehash == 0)
				fprintf(fpW, "%d ", pos);		//escrita do indice do vetor mapeado do código hash para o arquivo
		
			if(ha->vetor[pos] == NULL){		//se a posição estiver vazia na tabela
				struct nodo *nova = (Nodo*)calloc(1, sizeof(Nodo));
				strcpy(nova->dado, palavra);
				ha->vetor[pos] = nova;		//insere o novo valor na hash
				
				ha->qtd++;	//aumenta a quantidade de elementas na hash
				t = 1;		//faz finalizar o laço while
				if(rehash == 0)	//caso não esteja fazendo rehash, vai escrever no arquivo
					fprintf(fpW, "%d %d SUCESS\n", pos, i); 	//indice em que posição a chave foi inserida e número de colisões
				
			} else{		//se a posição não estiver vazia na tabela
				if(strcmp(palavra, ha->vetor[pos]->dado)==0){
					if(rehash == 0) //caso não esteja fazendo rehash, vai escrever no arquivo
						fprintf(fpW, "%d %d FAIL\n", pos, i);   //indice em que posição a chave foi inserida e número de colisões
					t = 1;		//faz finalizar o laço while
				}
				i++;	//incrementa 1 no cálculo da hash
			}
		}
		else{ //caso seja encadeamento
			struct nodo *nova = (Nodo*)calloc(1, sizeof(Nodo));
			strcpy(nova->dado, palavra);
			nova->prox = NULL;
		
			if(rehash == 0)		//caso não esteja fazendo rehash
				fprintf(fpW, "%d ", pos);		//escrita do indice do vetor mapeado do código hash para o arquivo
				
			if(ha->vetor[pos] == NULL){			//se a posição estiver vazia
				ha->vetor[pos] = nova;
				ha->qtd++;
				t = 1;		//faz parar o laço while
				if(rehash == 0)		//caso não esteja fazendo rehash
					fprintf(fpW, "%d %d SUCESS\n", pos, i);		//indice em que posição a chave foi inserida e número de colisões
				
			} else{			//caso tenha um elemento na posição
				
				struct nodo *ref = (Nodo*)calloc(1, sizeof(Nodo));
				ref = ha->vetor[pos];	//faz referência a posição inicial da lista
				int encad = 0, erro = 0;
				
				while(ha->vetor[pos]->prox != NULL){	//percorre a lista até encontrar null	
					ha->vetor[pos] = ha->vetor[pos]->prox;
					encad++;		//incrementa o número de elementos encadeados
					if(strcmp(palavra, ha->vetor[pos]->dado)==0){	//caso já tenha o elemento na lista
						erro = 1;
						break;
					}
					
				}
				
				if(erro == 0){	//caso não tenha o elemento na lista
					ha->vetor[pos] = nova;
					ha->qtd++;	
					fprintf(fpW, "%d %d SUCESS\n", pos, encad); 	//indice em que a chave foi inserida e número de colisões
					
				} else{		//caso tenha um elemento na lista
					if(rehash == 0)	//se não estiver fazendo rehash
						fprintf(fpW, "%d %d FAIL\n", pos, encad);	//indice em que posição a chave foi inserida e número de colisões
					
				}
				t = 1;		//finaliza o laço while
				ha->vetor[pos] = ref;	//volta a referência a posição inicial da lista
				ref = NULL;
				free(ref);
			}
		}
	}	
}

//buscar string e/ou deletar
void busca(Hash* ha, char palavra[101], int tipo, FILE *fpW, int delete){
	int pos, t=0, i=0;
	unsigned int k;
	
	k = rotation(palavra, strlen(palavra));
	while(t==0){
		switch(tipo){
			case 0: pos = hashPrimaria(ha, k, 0); break;	// tipo encadeamento
			case 1: pos = hashPrimaria(ha, k, i); break; // tipo linear
			case 2: pos = hashQuadratica(ha, k, i); break;	//tipo quadratico
			case 3: pos = hashDuplo(ha, k, i); break;	// tipo hash_duplo
		}
		if(tipo != 0){
			if(i==0)
				fprintf(fpW, "%d ", pos);		//indice do vetor mapeado do código hash
		
			if(ha->vetor[pos] == NULL){		//se a posição estiver vazia na tabela
				fprintf(fpW, "%d %d FAIL\n", pos, i);		//indice em que posição a chave foi inserida e número de colisões
				t = 1;
				
			} else{		//se a posição não estiver vazia na tabela
				if(strcmp(palavra, ha->vetor[pos]->dado)==0){	//caso encontre o dado procurado
					if(delete == 1)		//caso a função seja para deletar
						ha->vetor[pos] = NULL;
					fprintf(fpW, "%d %d SUCESS\n", pos, i); 	//indice em que posição a chave foi inserida e número de colisões
					t = 1;		//finaliza o laço while
				}
				i++;  	//incrementa 1 no cálculo da hash
			}
		}
		else{ //caso seja encadeamento
			struct nodo *ref = (Nodo*)calloc(1, sizeof(Nodo));
			struct nodo *ant = (Nodo*)calloc(1, sizeof(Nodo));
			fprintf(fpW, "%d ", pos);		//indice do vetor mapeado do código hash
			int encad = 0;
			
			do{
				 
				if(ha->vetor[pos] == NULL){		//caso a posição esteja vazia
					fprintf(fpW, "%d %d FAIL\n", pos, encad);		//indice em que posição a chave foi inserida e número de colisões
					i=1;		//vai finalizar o laço
				}
				
				else{							//caso a posição não esteja vazia
				
					ref = ha->vetor[pos];			//referência a primeira posição do encadeamento
					ant = ha->vetor[pos];			//posição anterior no encadeamento
					
					if(strcmp(palavra, ha->vetor[pos]->dado)==0){		//caso a palavra esteja na posição
						if(delete == 1){	//caso a função seja para deletar
							struct nodo *aux = (Nodo*)calloc(1, sizeof(Nodo));
							if(encad == 0){		//se estiver na primeira poção da lista
								aux = ha->vetor[pos];
								ha->vetor[pos] = ha->vetor[pos]->prox;
								
							} else{		//se não estiver na primeira posição da lista
								aux = ha->vetor[pos];
								ant->prox = ha->vetor[pos]->prox;		//deleta e reorganiza os elementos na lista
								aux = NULL;
								free(aux);
							}
						}
						fprintf(fpW, "%d %d SUCESS\n", pos, encad);		//indice em que posição a chave foi inserida e número de colisões
						i=1;
					} else{				//caso a palavra não esteja na posição
						ha->vetor[pos] = ha->vetor[pos]->prox;		//percorre a lista
						if(encad != 0)	//isso é feito para que o ant sempre fique uma posição atrás da posição atual
							ant = ant->prox;
							
						encad++;		//incrementa a posição da lista
					}
				}
			} while(i != 1);	//o laço se repete até i ser igual a 1
			ha->vetor[pos] = ref;	//hash recupera a referência a primeira posição
			ref = NULL;
			free(ref);
			ant = NULL;
			free(ant);
			t = 1;		//finaliza o laço while
			
		}	
	}
}

//Função de dispersão: Hash primária
unsigned int hashPrimaria(Hash* ha, unsigned int k, int i){
	unsigned int h1;
	h1 = (k+i) % ha->tamanho;
	return h1;
}

//Função de dispersão: Hash secundária
unsigned int hashSecundaria(Hash* ha, unsigned int k){
	unsigned int h2;
	h2 = 1 + (k % (ha->tamanho - 1));
	
	return h2;
}

//Função de dispersão: Hash quadrática
int hashQuadratica(Hash* ha, unsigned int k, int i){
	int hk;
	int c1=3, c2=5;
	hk = (k + c1*i + c2*i*i) % ha->tamanho;
	
	return hk;
}

//Função de dispersão: Hash duplo
unsigned int hashDuplo(Hash* ha, unsigned int k, int i){
	unsigned int hd;
	
	hd = (hashPrimaria(ha, k, 0) + i*hashSecundaria(ha, k)) % ha->tamanho;

	return hd;
}

//Verifica carga, e faz rehash caso necessário
void *rehash(Hash* ha, int t, FILE *fpW){
	float factor = 0.75;
	
	if(((float)ha->qtd/ha->tamanho) >= factor){
		int i;
		Hash* rehash;
		rehash = novaHash(ha->tamanho*2);
		
		for(i=0;i<ha->tamanho;i++){
			if(ha->vetor[i] != NULL){
				insere(rehash, ha->vetor[i]->dado, t, fpW, 1); 		// '1' no último parâmetro indica que é uma rehash
			}
		}
		liberaHash(ha);
		return rehash;
	}
	return ha;
}

