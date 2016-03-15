#include <iostream>
#include <mpi.h>
#include <algorithm> // sort, next_permutation
#include "tsp.h"
using namespace std;


Grafo::Grafo(int numVertices, int verticeInicial, bool grafoRandomico) { /*construtor do grafo*/
	if(numVertices < 1) { /*verifica se o número de vértices é menor que 1*/
		cout << "Erro: número de vértices <= 0\n";
		exit(1);
	}

	this->numVertices = numVertices; /*atribui o número de vértices*/
	this->verticeInicial = verticeInicial; /*atribui vértice inicial*/
	this->totalArestas = 0; /*inicializa o total de arestas com 0*/

	if(grafoRandomico)
		gerarGrafo();
}


void Grafo::gerarGrafo() {
	vector<int> vec;

	/*cria o vetor*/
	for(int i = 0; i < numVertices; i++)
		vec.push_back(i);

	/*gera uma permutação aleatória*/
	random_shuffle(vec.begin(), vec.end());

	verticeInicial = vec[0]; /*atualiza vertice inicial*/

	int i, peso;
	for(i = 0; i <= numVertices; i++) {
		peso = rand() % numVertices + 1; /*peso aleatório no intervalo [1,V]*/

		if(i + 1 < numVertices)
			addAresta(vec[i], vec[i + 1], peso);
		else {
			/*adiciona última aresta*/
			addAresta(vec[i], vec[0], peso);
			break;
		}
	}
	/*calcula o limite máximo de arestas do grafo*/
	int limiteArestas = numVertices * (numVertices - 1); /*um vértice pode ter saida para todos vértices, menos pra ele mesmo*/
	int tamanhoArestas = rand() % (2 * limiteArestas) + limiteArestas;

	/*adiciona outras arestas aleatoriamente*/
	for(int i = 0; i < tamanhoArestas; i++) {
		int origem = rand() % numVertices; /*origem aleatória*/
		int destino = rand() % numVertices; /*destino aleatório*/
		peso = rand() % numVertices + 1; /*peso aleatório no intervalo [1,numVertices]*/
		if(origem != destino) {
			addAresta(vec[origem], vec[destino], peso);
			addAresta(vec[destino], vec[origem], peso);
		}
	}
}


void Grafo::infoGrafo() {
	cout << "Mostrando informações do grafo:\n\n";
	cout << "Numero de vértices: " << numVertices;
	cout << "\nNumero de arestas: " << mapaArestas.size() << "\n";
}


void Grafo::addAresta(int origem, int destino, int peso) { /*adiciona uma aresta*/
	mapaArestas[make_pair(origem, destino)] = peso; /*adiciona uma aresta no mapa*/
}


void Grafo::desenhaGrafo() { /*mostra todas as ligações do grafo*/
	map<pair<int, int>, int>::iterator it;
	for(it = mapaArestas.begin(); it != mapaArestas.end(); ++it)
		cout << it->first.first << " ligado ao vertice " << it->first.second << " com peso " << it->second << endl;
}


int Grafo::existeAresta(int origem, int destino) { /*checa se existe uma aresta*/
	map<pair<int, int>,int>::iterator it = mapaArestas.find(make_pair(origem, destino));

	if(it != mapaArestas.end())
		return it->second; /*retorna o custo*/
	return -1;
}


/*construtor genético*/
Genetico::Genetico(Grafo* grafo, int tamPopulacao, int qtdGeracoes, int taxaMutacao, bool mostrar_Populacao) {
	if(tamPopulacao < 1) { /*checa se o tamanho da população é menor que 1*/
		cout << "Erro: tamPopulacao < 1\n";
		exit(1);
	}
	else if(taxaMutacao < 0 || taxaMutacao > 100) { /*checa se a taxa de mutação é menor que 0 ou maior que 100*/
		cout << "Erro: taxaMutacao deve ser >= 0 e <= 100\n";
		exit(1);
	}
	this->grafo = grafo;
	this->tamPopulacao = tamPopulacao;
	this->tamRealPopulacao = 0;
	this->qtdGeracoes = qtdGeracoes;
	this->taxaMutacao = taxaMutacao;
	this->mostrarPopulacao = mostrar_Populacao;
}



// checa se a solução é válida, então retorna o custo total do taminho ou então returna -1
int Genetico::solucaoValida(vector<int>& solucao) {
	int custoTotal = 0;
	set<int> set_solucao;

	/*verifica se não contem repetição de elementos*/
	for(int i = 0; i < grafo->numVertices; i++)
		set_solucao.insert(solucao[i]);

	if(set_solucao.size() != (unsigned)grafo->numVertices)
		return -1;

	/*checa se as conexões são válidas*/
	for(int i = 0; i < grafo->numVertices; i++) {
		if(i + 1 <  grafo->numVertices) {
			int custo = grafo->existeAresta(solucao[i], solucao[i+1]);

			/*checa se existe conexão*/
			if(custo == -1)
				return -1;
			else
				custoTotal += custo;
		}
		else {
			int custo = grafo->existeAresta(solucao[i], solucao[0]);

			/*checa se existe conexão*/
			if(custo == -1)
				return -1;
			else
				custoTotal += custo;
			break;
		}
	}
	return custoTotal;
}


bool Genetico::existeCromossomo(const vector<int> & cromossomo) {
	/*checa se existe cromossomos na população*/
	for(vector<pair<vector<int>, int> >::iterator it=populacao.begin(); it!=populacao.end(); ++it) {
		const vector<int>& vec = (*it).first; /*recupera o vetor*/
		if(equal(cromossomo.begin(), cromossomo.end(), vec.begin())) /*compara os vetores*/
			return true;
	}
	return false;
}


void Genetico::gerarPopInicial() { /*gera a população inicial*/
	vector<int> pai;

	/*insere vértice inicial no pai*/
	pai.push_back(grafo->verticeInicial);

	/*cria o pai*/
	for(int i = 0; i < grafo->numVertices; i++) {
		if(i != grafo->verticeInicial)
			pai.push_back(i);
	}

	int custo_total = solucaoValida(pai);

	if(custo_total != -1) { /*checa se o pai é valido*/
		populacao.push_back(make_pair(pai, custo_total)); /*insere na população*/
		tamRealPopulacao++; /*incrementa tamanho real da população*/
	}

	/*faz permutações aleatórias "generations" vezes*/
	for(int i = 0; i < qtdGeracoes; i++) {
		/*gera uma permutação aleatória*/
		random_shuffle(pai.begin() + 1, pai.begin() + (rand() % (grafo->numVertices - 1) + 1));

		int custo_total = solucaoValida(pai); /*checa se a solução é válida*/

		/*verifica se a permutação é uma solução válida e se não existe*/
		if(custo_total != -1 && !existeCromossomo(pai)) {
			populacao.push_back(make_pair(pai, custo_total)); /*adiciona na população*/
			tamRealPopulacao++; /*incrementa população real*/
		}
		if(tamRealPopulacao == tamPopulacao) /*verifica o tamanho da população*/
			break;
	}

	/*verifica se a população real é 0*/
	if(tamRealPopulacao == 0)
		cout << "\nPopulação inicial vazia ;( tente executar o algoritmo novamente...";
	else
		sort(populacao.begin(), populacao.end(), ordenaPar()); /*ordena a população*/
}


void Genetico::mostrarPopulacao() {
	cout << "\nMostrando soluções...\n\n";
	for(vector<pair<vector<int>, int> >::iterator it=populacao.begin(); it!=populacao.end(); ++it) {
		const vector<int>& vec = (*it).first; /*recupera o vetor*/

		for(int i = 0; i < grafo->numVertices; i++)
			cout << vec[i] << " ";
		cout << grafo->verticeInicial;
		cout << " | Custo: " << (*it).second << "\n\n";
	}
	cout << "\nTamanho da população: " << tamRealPopulacao << endl;
}


/*insere no vetor usando busca binária*/
void Genetico::insercaoBinaria(vector<int>& filho, int custoTotal) {
	int imin = 0;
	int imax = tamRealPopulacao - 1;

	while(imax >= imin) {
		int imid = imin + (imax - imin) / 2;

		if(custoTotal == populacao[imid].second) {
			populacao.insert(populacao.begin() + imid, make_pair(filho, custoTotal));
			return;
		}
		else if(custoTotal > populacao[imid].second)
			imin = imid + 1;
		else
			imax = imid - 1;
	}
	populacao.insert(populacao.begin() + imin, make_pair(filho, custoTotal));
}


/*
	Realiza o crossover
	Esse crossover seleciona dois pontos aleatoriamente
	Esses pontos geram subsequencias em ambos pais
	A substring invertida de parent1 é colocada em parent2
	E vice-versa

	Exemplo:
		parent1: 1 2 3 4 5
		parent2: 1 2 4 5 3

		subsequencia em parent1: 2 3 4
		subsequencia em parent2: 2 4 5

		substring inverted in parent1: 4 3 2
		substring inverted in parent2: 5 4 2

		child1: 1 5 4 2 5
		child2: 1 4 3 2 3

		Filhos são inválidos: 5 aparece 2x em child1 e 3 aparece 2x em child2
		Solução: mapa de genes que verifica se os genes estão ou não sendo usados
*/
void Genetico::crossOver(vector<int>& pai1, vector<int>& pai2) {
	vector<int> filho1, filho2;

	/*mapa de genes, verifica se já estão selecionados*/
	map<int, int> genes1, genes2;

	for(int i = 0; i < grafo->numVertices; i++) {
		/*Inicialmente os genes não estão utilizados*/
		genes1[pai1[i]] = 0;
		genes2[pai2[i]] = 0;
	}

	/*gera pontos aleatórios*/
	int ponto1 = rand() % (grafo->numVertices - 1) + 1;
	int ponto2 = rand() % (grafo->numVertices - ponto1) + ponto1;

	/*Ajusta os pontos se eles são iguais*/
	if(ponto1 == ponto2) {
		if(ponto1 - 1 > 1)
			ponto1--;
		else if(ponto2 + 1 < grafo->numVertices)
			ponto2++;
		else {
			/*ponto 1 ou 2, randomico*/
			int point = rand() % 10 + 1; /*numero entre 1 e 10*/
			if(point <= 5)
				ponto1--;
			else
				ponto2++;
		}
	}

	/*gera filhos*/

	// até que point1, child1 receba genes de pai1
	// e filho2 receba genes de parent2
	for(int i = 0; i < ponto1; i++) {
		/*adiciona genes*/
		filho1.push_back(pai1[i]);
		filho2.push_back(pai2[i]);
		/*marca genes*/
		genes1[pai1[i]] = 1;
		genes2[pai2[i]] = 1;
	}

	/*marca genes restantes*/
	for(int i = ponto2 + 1; i < grafo->numVertices; i++) {
		genes1[pai1[i]] = 1;
		genes2[pai2[i]] = 1;
	}

	/*	aqui está a subsequencia invertida
		child1 recebe genes de parent2 e
		child2 recebe genes de pai1 */
	for(int i = ponto2; i >= ponto1; i--) {
		if(genes1[pai2[i]] == 0) { /*se o gene não foi usado*/
			filho1.push_back(pai2[i]);
			genes1[pai2[i]] = 1; /*marca o gene*/
		}
		else {
			/*se o gene já está sendo utilizado escolhe um que não esteja*/
			for(map<int, int>::iterator it = genes1.begin(); it != genes1.end(); ++it) {
				if(it->second == 0) { /*checa se não foi usado*/
					filho1.push_back(it->first);
					genes1[it->first] = 1; /*marca como usado*/
					break; /*deixa o loop*/
				}
			}
		}

		if(genes2[pai1[i]] == 0) { /*se o gene não foi usado*/
			filho2.push_back(pai1[i]);
			genes2[pai1[i]] = 1; /*marca o gene*/
		}
		else {
			/*se o gene já está sendo utilizado escolhe um que não esteja*/
			for(map<int, int>::iterator it = genes2.begin(); it != genes2.end(); ++it) {
				if(it->second == 0) { /*verifica se não está sendo usado*/
					filho2.push_back(it->first);
					genes2[it->first] = 1; /*marca como usado*/
					break; /*deixa o loop*/
				}
			}
		}
	}

	// genes restantes: filho1 recebe genes do pai1
	// e child2 recebe genes do parent2
	for(int i = ponto2 + 1; i < grafo->numVertices; i++)
	{
		filho1.push_back(pai1[i]);
		filho2.push_back(pai2[i]);
	}

	/*mutação*/
	int mutacao = rand() % 100 + 1; /* número randômico no intervalo [1,100] */
	if(mutacao <= taxaMutacao) /* checa se o numero randomico <= taxa de mutação*/
	{
		/*faz uma mutação: mudança de 2 genes*/

		int index_gene1, index_gene2;
		index_gene1 = rand() % (grafo->numVertices - 1) + 1;
		index_gene2 = rand() % (grafo->numVertices - 1) + 1;

		/* faz para filho1 */
		int aux = filho1[index_gene1];
		filho1[index_gene1] = filho1[index_gene2];
		filho1[index_gene2] = aux;

		/* faz para filho2 */
		aux = filho2[index_gene1];
		filho2[index_gene1] = filho2[index_gene2];
		filho2[index_gene2] = aux;
	}

	int custoTotalFilho1 = solucaoValida(filho1);
	int custoTotalFilho2 = solucaoValida(filho2);

	/*verifica se é uma solução válida e não existe na população*/
	if(custoTotalFilho1 != -1 && !existeCromossomo(filho1)) {
		/*adiciona o filho na população*/
		insercaoBinaria(filho1, custoTotalFilho1); /*usa busca binária para inserir*/
		tamRealPopulacao++; /*incrementa população real*/
	}

	/*mesmo procedimento anterior*/
	if(custoTotalFilho2 != -1 && !existeCromossomo(filho2)) {
		insercaoBinaria(filho2, custoTotalFilho2);
		tamRealPopulacao++;
	}
}


/*executa algoritmo genético*/
void Genetico::executar(int rank, int size) {
	gerarPopInicial(); /*recupera população inicial*/

	if(tamRealPopulacao == 0)
		return;

	for(int i = 0; i < qtdGeracoes/size; i++) {

		int  antigoTamPopulacao = tamRealPopulacao;

		/* seleciona dois pais (se existe) que participarão
			do processo de reprodução */
		if(tamRealPopulacao >= 2) {
			if(tamRealPopulacao == 2) {
				/*aplica crossover nos pais*/
				crossOver(populacao[0].first, populacao[1].first);
			}
			else {
				/* real_size_population > 2*/

				int pai1, pai2;

				do {
					/* seleciona dois pais randomicamente */
					pai1 = rand() % tamRealPopulacao;
					pai2 = rand() % tamRealPopulacao;
				}while(pai1 == pai2);

				/*aplica crossover nos dois pais*/
				crossOver(populacao[pai1].first, populacao[pai2].first);
			}

			/*recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu*/
			int diff_population = tamRealPopulacao - antigoTamPopulacao;

			if(diff_population == 2) {
				if(tamRealPopulacao > tamPopulacao) {
					/*remove os dois piores pais da população*/
					populacao.pop_back();
					populacao.pop_back();

					/*decrementa apos remover os pais*/
					tamRealPopulacao -= 2;
				}
			}
			else if(diff_population == 1) {
				if(tamRealPopulacao > tamPopulacao) {
					populacao.pop_back(); /*remove o pior pai*/
					tamRealPopulacao--;
				}
			}
		}
		else { /*população só contem um pai*/
			/*aplica crossover nesse pai*/
			crossOver(populacao[0].first, populacao[0].first);

			if(tamRealPopulacao > tamPopulacao) {
				populacao.pop_back(); /*remove o pior pai*/
				tamRealPopulacao--;
			}
		}
	}

    int custo;
    const vector<int>& vec = populacao[0].first;

    //reduz para o processo 0 o menor custo de todos os processos
    MPI_Reduce(&populacao[0].second,&custo,1,MPI_INT,MPI_MIN,0,MPI_COMM_WORLD);

    if(rank == 0) {
        if(mostrarPopulacao == true)
            mostrarPopulacao(); /*mostra a população*/
    	cout << "\nMelhor solucao: ";
    	for(int i = 0; i < grafo->numVertices; i++)
    		cout << vec[i] << " ";
    	cout << vec[0];
    	cout << " | Custo: " << custo;
    }
}


int Genetico::getMelhorCusto()
{
	if(tamRealPopulacao > 0)
		return populacao[0].second;
	return -1;
}
