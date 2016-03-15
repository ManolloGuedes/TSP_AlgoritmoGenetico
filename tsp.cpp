#include <iostream>
#include <mpi.h>
#include <algorithm> // sort, next_permutation
#include "tsp.h"
using namespace std;


Grafo::Grafo(int numVertices, int verticeInicial, bool grafoRandomico) { /*construtor do grafo*/
	if(numVertices < 1) { /*verifica se o n�mero de v�rtices � menor que 1*/
		cout << "Erro: n�mero de v�rtices <= 0\n";
		exit(1);
	}

	this->numVertices = numVertices; /*atribui o n�mero de v�rtices*/
	this->verticeInicial = verticeInicial; /*atribui v�rtice inicial*/
	this->totalArestas = 0; /*inicializa o total de arestas com 0*/

	if(grafoRandomico)
		gerarGrafo();
}


void Grafo::gerarGrafo() {
	vector<int> vec;

	/*cria o vetor*/
	for(int i = 0; i < numVertices; i++)
		vec.push_back(i);

	/*gera uma permuta��o aleat�ria*/
	random_shuffle(vec.begin(), vec.end());

	verticeInicial = vec[0]; /*atualiza vertice inicial*/

	int i, peso;
	for(i = 0; i <= numVertices; i++) {
		peso = rand() % numVertices + 1; /*peso aleat�rio no intervalo [1,V]*/

		if(i + 1 < numVertices)
			addAresta(vec[i], vec[i + 1], peso);
		else {
			/*adiciona �ltima aresta*/
			addAresta(vec[i], vec[0], peso);
			break;
		}
	}
	/*calcula o limite m�ximo de arestas do grafo*/
	int limiteArestas = numVertices * (numVertices - 1); /*um v�rtice pode ter saida para todos v�rtices, menos pra ele mesmo*/
	int tamanhoArestas = rand() % (2 * limiteArestas) + limiteArestas;

	/*adiciona outras arestas aleatoriamente*/
	for(int i = 0; i < tamanhoArestas; i++) {
		int origem = rand() % numVertices; /*origem aleat�ria*/
		int destino = rand() % numVertices; /*destino aleat�rio*/
		peso = rand() % numVertices + 1; /*peso aleat�rio no intervalo [1,numVertices]*/
		if(origem != destino) {
			addAresta(vec[origem], vec[destino], peso);
			addAresta(vec[destino], vec[origem], peso);
		}
	}
}


void Grafo::infoGrafo() {
	cout << "Mostrando informa��es do grafo:\n\n";
	cout << "Numero de v�rtices: " << numVertices;
	cout << "\nNumero de arestas: " << mapaArestas.size() << "\n";
}


void Grafo::addAresta(int origem, int destino, int peso) { /*adiciona uma aresta*/
	mapaArestas[make_pair(origem, destino)] = peso; /*adiciona uma aresta no mapa*/
}


void Grafo::desenhaGrafo() { /*mostra todas as liga��es do grafo*/
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


/*construtor gen�tico*/
Genetico::Genetico(Grafo* grafo, int tamPopulacao, int qtdGeracoes, int taxaMutacao, bool mostrar_Populacao) {
	if(tamPopulacao < 1) { /*checa se o tamanho da popula��o � menor que 1*/
		cout << "Erro: tamPopulacao < 1\n";
		exit(1);
	}
	else if(taxaMutacao < 0 || taxaMutacao > 100) { /*checa se a taxa de muta��o � menor que 0 ou maior que 100*/
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



// checa se a solu��o � v�lida, ent�o retorna o custo total do taminho ou ent�o returna -1
int Genetico::solucaoValida(vector<int>& solucao) {
	int custoTotal = 0;
	set<int> set_solucao;

	/*verifica se n�o contem repeti��o de elementos*/
	for(int i = 0; i < grafo->numVertices; i++)
		set_solucao.insert(solucao[i]);

	if(set_solucao.size() != (unsigned)grafo->numVertices)
		return -1;

	/*checa se as conex�es s�o v�lidas*/
	for(int i = 0; i < grafo->numVertices; i++) {
		if(i + 1 <  grafo->numVertices) {
			int custo = grafo->existeAresta(solucao[i], solucao[i+1]);

			/*checa se existe conex�o*/
			if(custo == -1)
				return -1;
			else
				custoTotal += custo;
		}
		else {
			int custo = grafo->existeAresta(solucao[i], solucao[0]);

			/*checa se existe conex�o*/
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
	/*checa se existe cromossomos na popula��o*/
	for(vector<pair<vector<int>, int> >::iterator it=populacao.begin(); it!=populacao.end(); ++it) {
		const vector<int>& vec = (*it).first; /*recupera o vetor*/
		if(equal(cromossomo.begin(), cromossomo.end(), vec.begin())) /*compara os vetores*/
			return true;
	}
	return false;
}


void Genetico::gerarPopInicial() { /*gera a popula��o inicial*/
	vector<int> pai;

	/*insere v�rtice inicial no pai*/
	pai.push_back(grafo->verticeInicial);

	/*cria o pai*/
	for(int i = 0; i < grafo->numVertices; i++) {
		if(i != grafo->verticeInicial)
			pai.push_back(i);
	}

	int custo_total = solucaoValida(pai);

	if(custo_total != -1) { /*checa se o pai � valido*/
		populacao.push_back(make_pair(pai, custo_total)); /*insere na popula��o*/
		tamRealPopulacao++; /*incrementa tamanho real da popula��o*/
	}

	/*faz permuta��es aleat�rias "generations" vezes*/
	for(int i = 0; i < qtdGeracoes; i++) {
		/*gera uma permuta��o aleat�ria*/
		random_shuffle(pai.begin() + 1, pai.begin() + (rand() % (grafo->numVertices - 1) + 1));

		int custo_total = solucaoValida(pai); /*checa se a solu��o � v�lida*/

		/*verifica se a permuta��o � uma solu��o v�lida e se n�o existe*/
		if(custo_total != -1 && !existeCromossomo(pai)) {
			populacao.push_back(make_pair(pai, custo_total)); /*adiciona na popula��o*/
			tamRealPopulacao++; /*incrementa popula��o real*/
		}
		if(tamRealPopulacao == tamPopulacao) /*verifica o tamanho da popula��o*/
			break;
	}

	/*verifica se a popula��o real � 0*/
	if(tamRealPopulacao == 0)
		cout << "\nPopula��o inicial vazia ;( tente executar o algoritmo novamente...";
	else
		sort(populacao.begin(), populacao.end(), ordenaPar()); /*ordena a popula��o*/
}


void Genetico::mostrarPopulacao() {
	cout << "\nMostrando solu��es...\n\n";
	for(vector<pair<vector<int>, int> >::iterator it=populacao.begin(); it!=populacao.end(); ++it) {
		const vector<int>& vec = (*it).first; /*recupera o vetor*/

		for(int i = 0; i < grafo->numVertices; i++)
			cout << vec[i] << " ";
		cout << grafo->verticeInicial;
		cout << " | Custo: " << (*it).second << "\n\n";
	}
	cout << "\nTamanho da popula��o: " << tamRealPopulacao << endl;
}


/*insere no vetor usando busca bin�ria*/
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
	A substring invertida de parent1 � colocada em parent2
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

		Filhos s�o inv�lidos: 5 aparece 2x em child1 e 3 aparece 2x em child2
		Solu��o: mapa de genes que verifica se os genes est�o ou n�o sendo usados
*/
void Genetico::crossOver(vector<int>& pai1, vector<int>& pai2) {
	vector<int> filho1, filho2;

	/*mapa de genes, verifica se j� est�o selecionados*/
	map<int, int> genes1, genes2;

	for(int i = 0; i < grafo->numVertices; i++) {
		/*Inicialmente os genes n�o est�o utilizados*/
		genes1[pai1[i]] = 0;
		genes2[pai2[i]] = 0;
	}

	/*gera pontos aleat�rios*/
	int ponto1 = rand() % (grafo->numVertices - 1) + 1;
	int ponto2 = rand() % (grafo->numVertices - ponto1) + ponto1;

	/*Ajusta os pontos se eles s�o iguais*/
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

	// at� que point1, child1 receba genes de pai1
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

	/*	aqui est� a subsequencia invertida
		child1 recebe genes de parent2 e
		child2 recebe genes de pai1 */
	for(int i = ponto2; i >= ponto1; i--) {
		if(genes1[pai2[i]] == 0) { /*se o gene n�o foi usado*/
			filho1.push_back(pai2[i]);
			genes1[pai2[i]] = 1; /*marca o gene*/
		}
		else {
			/*se o gene j� est� sendo utilizado escolhe um que n�o esteja*/
			for(map<int, int>::iterator it = genes1.begin(); it != genes1.end(); ++it) {
				if(it->second == 0) { /*checa se n�o foi usado*/
					filho1.push_back(it->first);
					genes1[it->first] = 1; /*marca como usado*/
					break; /*deixa o loop*/
				}
			}
		}

		if(genes2[pai1[i]] == 0) { /*se o gene n�o foi usado*/
			filho2.push_back(pai1[i]);
			genes2[pai1[i]] = 1; /*marca o gene*/
		}
		else {
			/*se o gene j� est� sendo utilizado escolhe um que n�o esteja*/
			for(map<int, int>::iterator it = genes2.begin(); it != genes2.end(); ++it) {
				if(it->second == 0) { /*verifica se n�o est� sendo usado*/
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

	/*muta��o*/
	int mutacao = rand() % 100 + 1; /* n�mero rand�mico no intervalo [1,100] */
	if(mutacao <= taxaMutacao) /* checa se o numero randomico <= taxa de muta��o*/
	{
		/*faz uma muta��o: mudan�a de 2 genes*/

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

	/*verifica se � uma solu��o v�lida e n�o existe na popula��o*/
	if(custoTotalFilho1 != -1 && !existeCromossomo(filho1)) {
		/*adiciona o filho na popula��o*/
		insercaoBinaria(filho1, custoTotalFilho1); /*usa busca bin�ria para inserir*/
		tamRealPopulacao++; /*incrementa popula��o real*/
	}

	/*mesmo procedimento anterior*/
	if(custoTotalFilho2 != -1 && !existeCromossomo(filho2)) {
		insercaoBinaria(filho2, custoTotalFilho2);
		tamRealPopulacao++;
	}
}


/*executa algoritmo gen�tico*/
void Genetico::executar(int rank, int size) {
	gerarPopInicial(); /*recupera popula��o inicial*/

	if(tamRealPopulacao == 0)
		return;

	for(int i = 0; i < qtdGeracoes/size; i++) {

		int  antigoTamPopulacao = tamRealPopulacao;

		/* seleciona dois pais (se existe) que participar�o
			do processo de reprodu��o */
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

			/*recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu*/
			int diff_population = tamRealPopulacao - antigoTamPopulacao;

			if(diff_population == 2) {
				if(tamRealPopulacao > tamPopulacao) {
					/*remove os dois piores pais da popula��o*/
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
		else { /*popula��o s� contem um pai*/
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
            mostrarPopulacao(); /*mostra a popula��o*/
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
