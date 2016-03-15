#include <iostream>
#include <omp.h>
#include <algorithm> // sort, next_permutation
#include "tsp.h"
using namespace std;


Grafo::Grafo(int V, int initial_vertex, bool random_graph) { /*construtor do grafo*/
	if(V < 1) { /*verifica se o número de vértices é menor que 1*/
		cout << "Erro: número de vértices <= 0\n";
		exit(1);
	}

	this->numVertices = V; /*atribui o número de vértices*/
	this->verticeInicial = initial_vertex; /*atribui vértice inicial*/
	this->totalArestas = 0; /*inicializa o total de arestas com 0*/

	if(random_graph)
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

	int i, weight;
	for(i = 0; i <= numVertices; i++) {
		weight = rand() % numVertices + 1; /*peso aleatório no intervalo [1,V]*/

		if(i + 1 < numVertices)
			addAresta(vec[i], vec[i + 1], weight);
		else {
			/*adiciona última aresta*/
			addAresta(vec[i], vec[0], weight);
			break;
		}
	}
	/*calcula o limite máximo de arestas do grafo*/
	int limiteArestas = numVertices * (numVertices - 1); /*um vértice pode ter saida para todos vértices, menos pra ele mesmo*/
	int tamanhoArestas = rand() % (2 * limiteArestas) + limiteArestas;

	/*adiciona outras arestas aleatoriamente*/
	for(int i = 0; i < tamanhoArestas; i++) {
		int src = rand() % numVertices; /*origem aleatória*/
		int dest = rand() % numVertices; /*destino aleatório*/
		weight = rand() % numVertices + 1; /*peso aleatório no intervalo [1,V]*/
		if(src != dest) {
			addAresta(vec[src], vec[dest], weight);
			addAresta(vec[dest], vec[src], weight);
		}
	}
}


void Grafo::infoGrafo() {
	cout << "Mostrando informações do grafo:\n\n";
	cout << "Numero de vértices: " << numVertices;
	cout << "\nNumero de arestas: " << mapaArestas.size() << "\n";
}


void Grafo::addAresta(int src, int dest, int weight) { /*adiciona uma aresta*/
	mapaArestas[make_pair(src, dest)] = weight; /*adiciona uma aresta no mapa*/
}


void Grafo::desenhaGrafo() { /*mostra todas as ligações do grafo*/
	map<pair<int, int>, int>::iterator it;
	for(it = mapaArestas.begin(); it != mapaArestas.end(); ++it)
		cout << it->first.first << " ligado ao vertice " << it->first.second << " com peso " << it->second << endl;
}


int Grafo::existeAresta(int src, int dest) { /*checa se existe uma aresta*/
	map<pair<int, int>,int>::iterator it = mapaArestas.find(make_pair(src, dest));

	if(it != mapaArestas.end())
		return it->second; /*retorna o custo*/
	return -1;
}


/*construtor genético*/
Genetic::Genetic(Grafo* graph, int tamPopulacao, int qtdGeracoes, int mutation_rate, bool show_population) {
	if(tamPopulacao < 1) { /*checa se o tamanho da população é menor que 1*/
		cout << "Erro: tamPopulacao < 1\n";
		exit(1);
	}
	else if(mutation_rate < 0 || mutation_rate > 100) { /*checa se a taxa de mutação é menor que 0 ou maior que 100*/
		cout << "Erro: mutation_rate deve ser >= 0 e <= 100\n";
		exit(1);
	}
	this->graph = graph;
	this->tamPopulacao = tamPopulacao;
	this->tamRealPopulacao = 0;
	this->qtdGeracoes = qtdGeracoes;
	this->taxaMutacao = mutation_rate;
	this->mostrar_Populacao = show_population;
}



// checa se a solução é válida, então retorna o custo total do taminho ou então returna -1
int Genetic::solucaoValida(vector<int>& solution) {
	int total_cost = 0;
	set<int> set_solution;

	/*verifica se não contem repetição de elementos*/
	for(int i = 0; i < graph->numVertices; i++)
		set_solution.insert(solution[i]);

	if(set_solution.size() != (unsigned)graph->numVertices)
		return -1;

	/*checa se as conexões são válidas*/
	for(int i = 0; i < graph->numVertices; i++) {
		if(i + 1 <  graph->numVertices) {
			int cost = graph->existeAresta(solution[i], solution[i+1]);

			/*checa se existe conexão*/
			if(cost == -1)
				return -1;
			else
				total_cost += cost;
		}
		else {
			int cost = graph->existeAresta(solution[i], solution[0]);

			/*checa se existe conexão*/
			if(cost == -1)
				return -1;
			else
				total_cost += cost;
			break;
		}
	}
	return total_cost;
}


bool Genetic::existeCromossomo(const vector<int> & v) {
	/*checa se existe cromossomos na população*/
	for(vector<pair<vector<int>, int> >::iterator it=populacao.begin(); it!=populacao.end(); ++it) {
		const vector<int>& vec = (*it).first; /*recupera o vetor*/
		if(equal(v.begin(), v.end(), vec.begin())) /*compara os vetores*/
			return true;
	}
	return false;
}


void Genetic::gerarPopInicial() { /*gera a população inicial*/
	vector<int> parent;

	/*insere vértice inicial no pai*/
	parent.push_back(graph->verticeInicial);

	/*cria o pai*/
	for(int i = 0; i < graph->numVertices; i++) {
		if(i != graph->verticeInicial)
			parent.push_back(i);
	}

	int total_cost = solucaoValida(parent);

	if(total_cost != -1) { /*checa se o pai é valido*/
		populacao.push_back(make_pair(parent, total_cost)); /*insere na população*/
		tamRealPopulacao++; /*incrementa tamanho real da população*/
	}

	/*faz permutações aleatórias "qtdGeracoes" vezes*/
	for(int i = 0; i < qtdGeracoes; i++) {
		/*gera uma permutação aleatória*/
		random_shuffle(parent.begin() + 1, parent.begin() + (rand() % (graph->numVertices - 1) + 1));

		int total_cost = solucaoValida(parent); /*checa se a solução é válida*/

		/*verifica se a permutação é uma solução válida e se não existe*/
		if(total_cost != -1 && !existeCromossomo(parent)) {
			populacao.push_back(make_pair(parent, total_cost)); /*adiciona na população*/
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


void Genetic::mostrarPopulacao() {
	cout << "\nMostrando soluções...\n\n";
	for(vector<pair<vector<int>, int> >::iterator it=populacao.begin(); it!=populacao.end(); ++it) {
		const vector<int>& vec = (*it).first; /*recupera o vetor*/

		for(int i = 0; i < graph->numVertices; i++)
			cout << vec[i] << " ";
		cout << graph->verticeInicial;
		cout << " | Custo: " << (*it).second << "\n\n";
	}
	cout << "\nTamanho da população: " << tamRealPopulacao << endl;
}


/*insere no vetor usando busca binária*/
void Genetic::insercaoBinaria(vector<int>& child, int total_cost) {
	int imin = 0;
	int imax = tamRealPopulacao - 1;

	while(imax >= imin) {
		int imid = imin + (imax - imin) / 2;

		if(total_cost == populacao[imid].second) {
			populacao.insert(populacao.begin() + imid, make_pair(child, total_cost));
			return;
		}
		else if(total_cost > populacao[imid].second)
			imin = imid + 1;
		else
			imax = imid - 1;
	}
	populacao.insert(populacao.begin() + imin, make_pair(child, total_cost));
}


/*
	Realiza o crossover
	Esse crossover seleciona dois pontos aleatoriamente
	Esses pontos geram subsequencias em ambos pais
	A substring invertida de pai1 é colocada em pai2
	E vice-versa

	Exemplo:
		pai1: 1 2 3 4 5
		pai2: 1 2 4 5 3

		subsequencia em pai1: 2 3 4
		subsequencia em pai2: 2 4 5

		substring inverted in pai1: 4 3 2
		substring inverted in pai2: 5 4 2

		child1: 1 5 4 2 5
		child2: 1 4 3 2 3

		Filhos são inválidos: 5 aparece 2x em child1 e 3 aparece 2x em child2
		Solução: mapa de genes que verifica se os genes estão ou não sendo usados
*/
void Genetic::crossOver(vector<int>& pai1, vector<int>& pai2) {
	vector<int> child1, child2;

	/*mapa de genes, verifica se já estão selecionados*/
	map<int, int> genes1, genes2;

	for(int i = 0; i < graph->numVertices; i++) {
		/*Inicialmente os genes não estão utilizados*/
		genes1[pai1[i]] = 0;
		genes2[pai2[i]] = 0;
	}

	/*gera pontos aleatórios*/

	int point1 = rand() % (graph->numVertices - 1) + 1;
	int point2 = rand() % (graph->numVertices - point1) + point1;


	/*Ajusta os pontos se eles são iguais*/
	if(point1 == point2) {
		if(point1 - 1 > 1)
			point1--;
		else if(point2 + 1 < graph->numVertices)
			point2++;
		else {
			/*ponto 1 ou 2, randomico*/
			int point = rand() % 10 + 1; /*numero entre 1 e 10*/
			if(point <= 5)
				point1--;
			else
				point2++;
		}
	}

	/*gera filhos*/

	// até que point1, child1 receba genes de pai1
	// e child2 receba genes de pai2
	for(int i = 0; i < point1; i++) {
		/*adiciona genes*/
		child1.push_back(pai1[i]);
		child2.push_back(pai2[i]);
		/*marca genes*/
		genes1[pai1[i]] = 1;
		genes2[pai2[i]] = 1;
	}

	/*marca genes restantes*/
	for(int i = point2 + 1; i < graph->numVertices; i++) {
		genes1[pai1[i]] = 1;
		genes2[pai2[i]] = 1;
	}

	/*	aqui está a subsequencia invertida
		child1 recebe genes de pai2 e
		child2 recebe genes de pai1 */
	for(int i = point2; i >= point1; i--) {
		if(genes1[pai2[i]] == 0) { /*se o gene não foi usado*/
			child1.push_back(pai2[i]);
			genes1[pai2[i]] = 1; /*marca o gene*/
		}
		else {
			/*se o gene já está sendo utilizado escolhe um que não esteja*/
			for(map<int, int>::iterator it = genes1.begin(); it != genes1.end(); ++it) {
				if(it->second == 0) { /*checa se não foi usado*/
					child1.push_back(it->first);
					genes1[it->first] = 1; /*marca como usado*/
					break; /*deixa o loop*/
				}
			}
		}

		if(genes2[pai1[i]] == 0) { /*se o gene não foi usado*/
			child2.push_back(pai1[i]);
			genes2[pai1[i]] = 1; /*marca o gene*/
		}
		else {
			/*se o gene já está sendo utilizado escolhe um que não esteja*/
			for(map<int, int>::iterator it = genes2.begin(); it != genes2.end(); ++it) {
				if(it->second == 0) { /*verifica se não está sendo usado*/
					child2.push_back(it->first);
					genes2[it->first] = 1; /*marca como usado*/
					break; /*deixa o loop*/
				}
			}
		}
	}

	// genes restantes: child1 recebe genes do pai1
	// e child2 recebe genes do pai2
	for(int i = point2 + 1; i < graph->numVertices; i++)
	{
		child1.push_back(pai1[i]);
		child2.push_back(pai2[i]);
	}

	/*mutação*/
	int mutation = rand() % 100 + 1; /* número randômico no intervalo [1,100] */
	if(mutation <= taxaMutacao) /* checa se o numero randomico <= taxa de mutação*/
	{
		/*faz uma mutação: mudança de 2 genes*/

		int index_gene1, index_gene2;
		index_gene1 = rand() % (graph->numVertices - 1) + 1;
		index_gene2 = rand() % (graph->numVertices - 1) + 1;

		/* faz para child1 */
		int aux = child1[index_gene1];
		child1[index_gene1] = child1[index_gene2];
		child1[index_gene2] = aux;

		/* faz para child2 */
		aux = child2[index_gene1];
		child2[index_gene1] = child2[index_gene2];
		child2[index_gene2] = aux;
	}

	int custoTotalFilho1 = solucaoValida(child1);
	int custoTotalFilho2 = solucaoValida(child2);



	/*verifica se é uma solução válida e não existe na população*/
	if(custoTotalFilho1 != -1 && !existeCromossomo(child1)) {
		/*adiciona o filho na população*/
		insercaoBinaria(child1, custoTotalFilho1); /*usa busca binária para inserir*/
		#pragma omp critical
		{
		tamRealPopulacao++; /*incrementa população real*/
		}
	}

	/*mesmo procedimento anterior*/
	if(custoTotalFilho2 != -1 && !existeCromossomo(child2)) {

		insercaoBinaria(child2, custoTotalFilho2);
		#pragma omp critical
		{
		tamRealPopulacao++;
		}
	}
}


/*executa algoritmo genético*/
void Genetic::executar() {
	gerarPopInicial(); /*recupera população inicial*/

	if(tamRealPopulacao == 0)
		return;


// SEM THREAD
///*


	for(int i = 0; i < qtdGeracoes; i++) {
		int  old_size_population = tamRealPopulacao;


		if(tamRealPopulacao >= 2) {
			if(tamRealPopulacao == 2) {
				crossOver(populacao[0].first, populacao[1].first);
			}
			else {

				int pai1, pai2;

				do {
					pai1 = rand() % tamRealPopulacao;
					pai2 = rand() % tamRealPopulacao;
				}while(pai1 == pai2);

				crossOver(populacao[pai1].first, populacao[pai2].first);
			}

			int diff_population = tamRealPopulacao - old_size_population;

			if(diff_population == 2) {
				if(tamRealPopulacao > tamPopulacao) {

					populacao.pop_back();
					populacao.pop_back();


					tamRealPopulacao -= 2;
				}
			}
			else if(diff_population == 1) {
				if(tamRealPopulacao > tamPopulacao) {
					populacao.pop_back();
					tamRealPopulacao--;
				}
			}
		}
		else {
			crossOver(populacao[0].first, populacao[0].first);

			if(tamRealPopulacao > tamPopulacao) {
				populacao.pop_back();
				tamRealPopulacao--;
			}
		}

	}



//*/

// 2 THREADS

/*

	#pragma omp parallel num_threads(2)
	{

	int tid = omp_get_thread_num();

		if (tid == 0) {

			for(int i = 0; i < qtdGeracoes/2; i++) {
				int  old_size_population = tamRealPopulacao;

				// seleciona dois pais (se existe) que participarão do processo de reprodução

				if(tamRealPopulacao >= 2) {
					if(tamRealPopulacao == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(populacao[0].first, populacao[1].first);
					}
					else {

						// tamRealPopulacao > 2

						int pai1, pai2;

						do {

							//seleciona dois pais randomicamente

							pai1 = rand() % tamRealPopulacao;
							pai2 = rand() % tamRealPopulacao;
						}while(pai1 == pai2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(populacao[pai1].first, populacao[pai2].first);
					}

					//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

					int diff_population = tamRealPopulacao - old_size_population;

					if(diff_population == 2) {
						if(tamRealPopulacao > tamPopulacao) {

							//remove os dois piores pais da população

							#pragma omp critical
							{
								populacao.pop_back();
								populacao.pop_back();

							//decrementa apos remover os pais

							tamRealPopulacao -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(tamRealPopulacao > tamPopulacao) {
							#pragma omp critical
							{
							populacao.pop_back(); //remove o pior pai
							tamRealPopulacao--;
							}
						}
					}
				}
				else {

					//população só contem um pai
					//aplica crossover nesse pai
					#pragma omp critical
					crossOver(populacao[0].first, populacao[0].first);

					if(tamRealPopulacao > tamPopulacao) {
						#pragma omp critical
						{
						populacao.pop_back();  //remove o pior pai
						tamRealPopulacao--;
						}
					}
				}
			}

		}


		if (tid == 1) {

			for(int i = qtdGeracoes/2; i < qtdGeracoes; i++) {
				int  old_size_population = tamRealPopulacao;

				// seleciona dois pais (se existe) que participarão do processo de reprodução

				if(tamRealPopulacao >= 2) {
					if(tamRealPopulacao == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(populacao[0].first, populacao[1].first);
					}
					else {

						// tamRealPopulacao > 2

						int pai1, pai2;

						do {

							//seleciona dois pais randomicamente

							pai1 = rand() % tamRealPopulacao;
							pai2 = rand() % tamRealPopulacao;
						}while(pai1 == pai2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(populacao[pai1].first, populacao[pai2].first);
					}

					//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

					int diff_population = tamRealPopulacao - old_size_population;

					if(diff_population == 2) {
						if(tamRealPopulacao > tamPopulacao) {

							//remove os dois piores pais da população

							#pragma omp critical
							{
								populacao.pop_back();
								populacao.pop_back();

							//decrementa apos remover os pais

							tamRealPopulacao -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(tamRealPopulacao > tamPopulacao) {
							#pragma omp critical
							{
							populacao.pop_back(); //remove o pior pai
							tamRealPopulacao--;
							}
						}
					}
				}
				else {

					//população só contem um pai
					//aplica crossover nesse pai
				    #pragma omp critical
					crossOver(populacao[0].first, populacao[0].first);

					if(tamRealPopulacao > tamPopulacao) {
						#pragma omp critical
						{
						populacao.pop_back();  //remove o pior pai
						tamRealPopulacao--;
						}
					}
				}
			}

		}

	} //fim da paralelização


*/

// 4 THREADS

/*
	#pragma omp parallel num_threads(3)
	{

	int tid = omp_get_thread_num();

		if (tid == 0) {

			for(int i = 0; i < qtdGeracoes/4; i++) {
				int  old_size_population = tamRealPopulacao;

				// seleciona dois pais (se existe) que participarão do processo de reprodução

				if(tamRealPopulacao >= 2) {
					if(tamRealPopulacao == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(populacao[0].first, populacao[1].first);
					}
					else {

						// tamRealPopulacao > 2

						int pai1, pai2;

						do {

							//seleciona dois pais randomicamente

							pai1 = rand() % tamRealPopulacao;
							pai2 = rand() % tamRealPopulacao;
						}while(pai1 == pai2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(populacao[pai1].first, populacao[pai2].first);
					}

					//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

					int diff_population = tamRealPopulacao - old_size_population;

					if(diff_population == 2) {
						if(tamRealPopulacao > tamPopulacao) {

							//remove os dois piores pais da população

							#pragma omp critical
							{
								populacao.pop_back();
								populacao.pop_back();

							//decrementa apos remover os pais

							tamRealPopulacao -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(tamRealPopulacao > tamPopulacao) {
							#pragma omp critical
							{
							populacao.pop_back(); //remove o pior pai
							tamRealPopulacao--;
							}
						}
					}
				}
				else {

					//população só contem um pai
					//aplica crossover nesse pai
					#pragma omp critical
					crossOver(populacao[0].first, populacao[0].first);

					if(tamRealPopulacao > tamPopulacao) {
						#pragma omp critical
						{
						populacao.pop_back();  //remove o pior pai
						tamRealPopulacao--;
						}
					}
				}
			}

		}


		if (tid == 1) {

			for(int i = qtdGeracoes/4; i < 2*(qtdGeracoes/4); i++) {
				int  old_size_population = tamRealPopulacao;

				// seleciona dois pais (se existe) que participarão do processo de reprodução

				if(tamRealPopulacao >= 2) {
					if(tamRealPopulacao == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(populacao[0].first, populacao[1].first);
					}
					else {

						// tamRealPopulacao > 2

						int pai1, pai2;

						do {

							//seleciona dois pais randomicamente

							pai1 = rand() % tamRealPopulacao;
							pai2 = rand() % tamRealPopulacao;
						}while(pai1 == pai2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(populacao[pai1].first, populacao[pai2].first);
					}

					//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

					int diff_population = tamRealPopulacao - old_size_population;

					if(diff_population == 2) {
						if(tamRealPopulacao > tamPopulacao) {

							//remove os dois piores pais da população

							#pragma omp critical
							{
								populacao.pop_back();
								populacao.pop_back();

							//decrementa apos remover os pais

							tamRealPopulacao -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(tamRealPopulacao > tamPopulacao) {
							#pragma omp critical
							{
							populacao.pop_back(); //remove o pior pai
							tamRealPopulacao--;
							}
						}
					}
				}
				else {

					//população só contem um pai
					//aplica crossover nesse pai
					#pragma omp critical
					crossOver(populacao[0].first, populacao[0].first);

					if(tamRealPopulacao > tamPopulacao) {
						#pragma omp critical
						{
						populacao.pop_back();  //remove o pior pai
						tamRealPopulacao--;
						}
					}
				}
			}

		}


		if (tid == 2) {

			for(int i = 2*(qtdGeracoes/4); i < 3*(qtdGeracoes/4); i++) {
				int  old_size_population = tamRealPopulacao;

				// seleciona dois pais (se existe) que participarão do processo de reprodução

				if(tamRealPopulacao >= 2) {
					if(tamRealPopulacao == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(populacao[0].first, populacao[1].first);
					}
					else {

						// tamRealPopulacao > 2

						int pai1, pai2;

						do {

							//seleciona dois pais randomicamente

							pai1 = rand() % tamRealPopulacao;
							pai2 = rand() % tamRealPopulacao;
						}while(pai1 == pai2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(populacao[pai1].first, populacao[pai2].first);
					}

					//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

					int diff_population = tamRealPopulacao - old_size_population;

					if(diff_population == 2) {
						if(tamRealPopulacao > tamPopulacao) {

							//remove os dois piores pais da população

							#pragma omp critical
							{
								populacao.pop_back();
								populacao.pop_back();

							//decrementa apos remover os pais

							tamRealPopulacao -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(tamRealPopulacao > tamPopulacao) {
							#pragma omp critical
							{
							populacao.pop_back(); //remove o pior pai
							tamRealPopulacao--;
							}
						}
					}
				}
				else {

					//população só contem um pai
					//aplica crossover nesse pai
					#pragma omp critical
					crossOver(populacao[0].first, populacao[0].first);

					if(tamRealPopulacao > tamPopulacao) {
						#pragma omp critical
						{
						populacao.pop_back();  //remove o pior pai
						tamRealPopulacao--;
						}
					}
				}
			}

		}


		if (tid == 3) {

			for(int i = 3*(qtdGeracoes/4); i < qtdGeracoes; i++) {
				int  old_size_population = tamRealPopulacao;

				// seleciona dois pais (se existe) que participarão do processo de reprodução

				if(tamRealPopulacao >= 2) {
					if(tamRealPopulacao == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(populacao[0].first, populacao[1].first);
					}
					else {

						// tamRealPopulacao > 2

						int pai1, pai2;

						do {

							//seleciona dois pais randomicamente

							pai1 = rand() % tamRealPopulacao;
							pai2 = rand() % tamRealPopulacao;
						}while(pai1 == pai2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(populacao[pai1].first, populacao[pai2].first);
					}

					//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

					int diff_population = tamRealPopulacao - old_size_population;

					if(diff_population == 2) {
						if(tamRealPopulacao > tamPopulacao) {

							//remove os dois piores pais da população

							#pragma omp critical
							{
								populacao.pop_back();
								populacao.pop_back();

							//decrementa apos remover os pais

							tamRealPopulacao -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(tamRealPopulacao > tamPopulacao) {
							#pragma omp critical
							{
							populacao.pop_back(); //remove o pior pai
							tamRealPopulacao--;
							}
						}
					}
				}
				else {

					//população só contem um pai
					//aplica crossover nesse pai
					#pragma omp critical
					crossOver(populacao[0].first, populacao[0].first);

					if(tamRealPopulacao > tamPopulacao) {
						#pragma omp critical
						{
						populacao.pop_back();  //remove o pior pai
						tamRealPopulacao--;
						}
					}
				}
			}

		}

	} //fim da paralelização



*/



// 8 THREADS
/*

#pragma omp parallel num_threads(3)
{

int tid = omp_get_thread_num();

	if (tid == 0) {

		for(int i = 0; i < qtdGeracoes/8; i++) {
			int  old_size_population = tamRealPopulacao;

			// seleciona dois pais (se existe) que participarão do processo de reprodução

			if(tamRealPopulacao >= 2) {
				if(tamRealPopulacao == 2) {

					//aplica crossover nos pais
					#pragma omp critical
					crossOver(populacao[0].first, populacao[1].first);
				}
				else {

					// tamRealPopulacao > 2

					int pai1, pai2;

					do {

						//seleciona dois pais randomicamente

						pai1 = rand() % tamRealPopulacao;
						pai2 = rand() % tamRealPopulacao;
					}while(pai1 == pai2);

					//aplica crossover nos dois pais
					#pragma omp critical
					crossOver(populacao[pai1].first, populacao[pai2].first);
				}

				//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

				int diff_population = tamRealPopulacao - old_size_population;

				if(diff_population == 2) {
					if(tamRealPopulacao > tamPopulacao) {

						//remove os dois piores pais da população

						#pragma omp critical
						{
							populacao.pop_back();
							populacao.pop_back();

						//decrementa apos remover os pais

						tamRealPopulacao -= 2;

						}
					}
				}
				else if(diff_population == 1) {
					if(tamRealPopulacao > tamPopulacao) {
						#pragma omp critical
						{
						populacao.pop_back(); //remove o pior pai
						tamRealPopulacao--;
						}
					}
				}
			}
			else {

				//população só contem um pai
				//aplica crossover nesse pai
				#pragma omp critical
				crossOver(populacao[0].first, populacao[0].first);

				if(tamRealPopulacao > tamPopulacao) {
					#pragma omp critical
					{
					populacao.pop_back();  //remove o pior pai
					tamRealPopulacao--;
					}
				}
			}
		}

	}


	if (tid == 1) {

		for(int i = qtdGeracoes/8; i < 2*(qtdGeracoes/8); i++) {
			int  old_size_population = tamRealPopulacao;

			// seleciona dois pais (se existe) que participarão do processo de reprodução

			if(tamRealPopulacao >= 2) {
				if(tamRealPopulacao == 2) {

					//aplica crossover nos pais
					#pragma omp critical
					crossOver(populacao[0].first, populacao[1].first);
				}
				else {

					// tamRealPopulacao > 2

					int pai1, pai2;

					do {

						//seleciona dois pais randomicamente

						pai1 = rand() % tamRealPopulacao;
						pai2 = rand() % tamRealPopulacao;
					}while(pai1 == pai2);

					//aplica crossover nos dois pais
					#pragma omp critical
					crossOver(populacao[pai1].first, populacao[pai2].first);
				}

				//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

				int diff_population = tamRealPopulacao - old_size_population;

				if(diff_population == 2) {
					if(tamRealPopulacao > tamPopulacao) {

						//remove os dois piores pais da população

						#pragma omp critical
						{
							populacao.pop_back();
							populacao.pop_back();

						//decrementa apos remover os pais

						tamRealPopulacao -= 2;

						}
					}
				}
				else if(diff_population == 1) {
					if(tamRealPopulacao > tamPopulacao) {
						#pragma omp critical
						{
						populacao.pop_back(); //remove o pior pai
						tamRealPopulacao--;
						}
					}
				}
			}
			else {

				//população só contem um pai
				//aplica crossover nesse pai
				#pragma omp critical
				crossOver(populacao[0].first, populacao[0].first);

				if(tamRealPopulacao > tamPopulacao) {
					#pragma omp critical
					{
					populacao.pop_back();  //remove o pior pai
					tamRealPopulacao--;
					}
				}
			}
		}

	}


	if (tid == 2) {

		for(int i = 2*(qtdGeracoes/8); i < 3*(qtdGeracoes/8); i++) {
			int  old_size_population = tamRealPopulacao;

			// seleciona dois pais (se existe) que participarão do processo de reprodução

			if(tamRealPopulacao >= 2) {
				if(tamRealPopulacao == 2) {

					//aplica crossover nos pais
					#pragma omp critical
					crossOver(populacao[0].first, populacao[1].first);
				}
				else {

					// tamRealPopulacao > 2

					int pai1, pai2;

					do {

						//seleciona dois pais randomicamente

						pai1 = rand() % tamRealPopulacao;
						pai2 = rand() % tamRealPopulacao;
					}while(pai1 == pai2);

					//aplica crossover nos dois pais
					#pragma omp critical
					crossOver(populacao[pai1].first, populacao[pai2].first);
				}

				//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

				int diff_population = tamRealPopulacao - old_size_population;

				if(diff_population == 2) {
					if(tamRealPopulacao > tamPopulacao) {

						//remove os dois piores pais da população

						#pragma omp critical
						{
							populacao.pop_back();
							populacao.pop_back();

						//decrementa apos remover os pais

						tamRealPopulacao -= 2;

						}
					}
				}
				else if(diff_population == 1) {
					if(tamRealPopulacao > tamPopulacao) {
						#pragma omp critical
						{
						populacao.pop_back(); //remove o pior pai
						tamRealPopulacao--;
						}
					}
				}
			}
			else {

				//população só contem um pai
				//aplica crossover nesse pai
				#pragma omp critical
				crossOver(populacao[0].first, populacao[0].first);

				if(tamRealPopulacao > tamPopulacao) {
					#pragma omp critical
					{
					populacao.pop_back();  //remove o pior pai
					tamRealPopulacao--;
					}
				}
			}
		}

	}


	if (tid == 3) {

		for(int i = 3*(qtdGeracoes/8); i < 4*(qtdGeracoes/8); i++) {
			int  old_size_population = tamRealPopulacao;

			// seleciona dois pais (se existe) que participarão do processo de reprodução

			if(tamRealPopulacao >= 2) {
				if(tamRealPopulacao == 2) {

					//aplica crossover nos pais
					#pragma omp critical
					crossOver(populacao[0].first, populacao[1].first);
				}
				else {

					// tamRealPopulacao > 2

					int pai1, pai2;

					do {

						//seleciona dois pais randomicamente

						pai1 = rand() % tamRealPopulacao;
						pai2 = rand() % tamRealPopulacao;
					}while(pai1 == pai2);

					//aplica crossover nos dois pais
					#pragma omp critical
					crossOver(populacao[pai1].first, populacao[pai2].first);
				}

				//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

				int diff_population = tamRealPopulacao - old_size_population;

				if(diff_population == 2) {
					if(tamRealPopulacao > tamPopulacao) {

						//remove os dois piores pais da população

						#pragma omp critical
						{
							populacao.pop_back();
							populacao.pop_back();

						//decrementa apos remover os pais

						tamRealPopulacao -= 2;

						}
					}
				}
				else if(diff_population == 1) {
					if(tamRealPopulacao > tamPopulacao) {
						#pragma omp critical
						{
						populacao.pop_back(); //remove o pior pai
						tamRealPopulacao--;
						}
					}
				}
			}
			else {

				//população só contem um pai
				//aplica crossover nesse pai
				#pragma omp critical
				crossOver(populacao[0].first, populacao[0].first);

				if(tamRealPopulacao > tamPopulacao) {
					#pragma omp critical
					{
					populacao.pop_back();  //remove o pior pai
					tamRealPopulacao--;
					}
				}
			}
		}

	}

	if (tid == 4) {

			for(int i = 4*(qtdGeracoes/8); i < 5*(qtdGeracoes/8); i++) {
				int  old_size_population = tamRealPopulacao;

				// seleciona dois pais (se existe) que participarão do processo de reprodução

				if(tamRealPopulacao >= 2) {
					if(tamRealPopulacao == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(populacao[0].first, populacao[1].first);
					}
					else {

						// tamRealPopulacao > 2

						int pai1, pai2;

						do {

							//seleciona dois pais randomicamente

							pai1 = rand() % tamRealPopulacao;
							pai2 = rand() % tamRealPopulacao;
						}while(pai1 == pai2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(populacao[pai1].first, populacao[pai2].first);
					}

					//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

					int diff_population = tamRealPopulacao - old_size_population;

					if(diff_population == 2) {
						if(tamRealPopulacao > tamPopulacao) {

							//remove os dois piores pais da população

							#pragma omp critical
							{
								populacao.pop_back();
								populacao.pop_back();

							//decrementa apos remover os pais

							tamRealPopulacao -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(tamRealPopulacao > tamPopulacao) {
							#pragma omp critical
							{
							populacao.pop_back(); //remove o pior pai
							tamRealPopulacao--;
							}
						}
					}
				}
				else {

					//população só contem um pai
					//aplica crossover nesse pai
					#pragma omp critical
					crossOver(populacao[0].first, populacao[0].first);

					if(tamRealPopulacao > tamPopulacao) {
						#pragma omp critical
						{
						populacao.pop_back();  //remove o pior pai
						tamRealPopulacao--;
						}
					}
				}
			}

		}

	if (tid == 5) {

			for(int i = 5*(qtdGeracoes/8); i < 6*(qtdGeracoes/8); i++) {
				int  old_size_population = tamRealPopulacao;

				// seleciona dois pais (se existe) que participarão do processo de reprodução

				if(tamRealPopulacao >= 2) {
					if(tamRealPopulacao == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(populacao[0].first, populacao[1].first);
					}
					else {

						// tamRealPopulacao > 2

						int pai1, pai2;

						do {

							//seleciona dois pais randomicamente

							pai1 = rand() % tamRealPopulacao;
							pai2 = rand() % tamRealPopulacao;
						}while(pai1 == pai2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(populacao[pai1].first, populacao[pai2].first);
					}

					//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

					int diff_population = tamRealPopulacao - old_size_population;

					if(diff_population == 2) {
						if(tamRealPopulacao > tamPopulacao) {

							//remove os dois piores pais da população

							#pragma omp critical
							{
								populacao.pop_back();
								populacao.pop_back();

							//decrementa apos remover os pais

							tamRealPopulacao -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(tamRealPopulacao > tamPopulacao) {
							#pragma omp critical
							{
							populacao.pop_back(); //remove o pior pai
							tamRealPopulacao--;
							}
						}
					}
				}
				else {

					//população só contem um pai
					//aplica crossover nesse pai
					#pragma omp critical
					crossOver(populacao[0].first, populacao[0].first);

					if(tamRealPopulacao > tamPopulacao) {
						#pragma omp critical
						{
						populacao.pop_back();  //remove o pior pai
						tamRealPopulacao--;
						}
					}
				}
			}

		}
	if (tid == 6) {

			for(int i = 6*(qtdGeracoes/8); i < 7*(qtdGeracoes/8); i++) {
				int  old_size_population = tamRealPopulacao;

				// seleciona dois pais (se existe) que participarão do processo de reprodução

				if(tamRealPopulacao >= 2) {
					if(tamRealPopulacao == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(populacao[0].first, populacao[1].first);
					}
					else {

						// tamRealPopulacao > 2

						int pai1, pai2;

						do {

							//seleciona dois pais randomicamente

							pai1 = rand() % tamRealPopulacao;
							pai2 = rand() % tamRealPopulacao;
						}while(pai1 == pai2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(populacao[pai1].first, populacao[pai2].first);
					}

					//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

					int diff_population = tamRealPopulacao - old_size_population;

					if(diff_population == 2) {
						if(tamRealPopulacao > tamPopulacao) {

							//remove os dois piores pais da população

							#pragma omp critical
							{
								populacao.pop_back();
								populacao.pop_back();

							//decrementa apos remover os pais

							tamRealPopulacao -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(tamRealPopulacao > tamPopulacao) {
							#pragma omp critical
							{
							populacao.pop_back(); //remove o pior pai
							tamRealPopulacao--;
							}
						}
					}
				}
				else {

					//população só contem um pai
					//aplica crossover nesse pai
					#pragma omp critical
					crossOver(populacao[0].first, populacao[0].first);

					if(tamRealPopulacao > tamPopulacao) {
						#pragma omp critical
						{
						populacao.pop_back();  //remove o pior pai
						tamRealPopulacao--;
						}
					}
				}
			}

		}
	if (tid == 7) {

			for(int i = 7*(qtdGeracoes/8); i < qtdGeracoes; i++) {
				int  old_size_population = tamRealPopulacao;

				// seleciona dois pais (se existe) que participarão do processo de reprodução

				if(tamRealPopulacao >= 2) {
					if(tamRealPopulacao == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(populacao[0].first, populacao[1].first);
					}
					else {

						// tamRealPopulacao > 2

						int pai1, pai2;

						do {

							//seleciona dois pais randomicamente

							pai1 = rand() % tamRealPopulacao;
							pai2 = rand() % tamRealPopulacao;
						}while(pai1 == pai2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(populacao[pai1].first, populacao[pai2].first);
					}

					//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

					int diff_population = tamRealPopulacao - old_size_population;

					if(diff_population == 2) {
						if(tamRealPopulacao > tamPopulacao) {

							//remove os dois piores pais da população

							#pragma omp critical
							{
								populacao.pop_back();
								populacao.pop_back();

							//decrementa apos remover os pais

							tamRealPopulacao -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(tamRealPopulacao > tamPopulacao) {
							#pragma omp critical
							{
							populacao.pop_back(); //remove o pior pai
							tamRealPopulacao--;
							}
						}
					}
				}
				else {

					//população só contem um pai
					//aplica crossover nesse pai
					#pragma omp critical
					crossOver(populacao[0].first, populacao[0].first);

					if(tamRealPopulacao > tamPopulacao) {
						#pragma omp critical
						{
						populacao.pop_back();  //remove o pior pai
						tamRealPopulacao--;
						}
					}
				}
			}

		}

} //fim da paralelização




*/






// 16 THREADS

/*

#pragma omp parallel num_threads(3)
{

	int tid = omp_get_thread_num();

		if (tid == 0) {

			for(int i = 0; i < qtdGeracoes/16; i++) {
				int  old_size_population = tamRealPopulacao;

				// seleciona dois pais (se existe) que participarão do processo de reprodução

				if(tamRealPopulacao >= 2) {
					if(tamRealPopulacao == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(populacao[0].first, populacao[1].first);
					}
					else {

						// tamRealPopulacao > 2

						int pai1, pai2;

						do {

							//seleciona dois pais randomicamente

							pai1 = rand() % tamRealPopulacao;
							pai2 = rand() % tamRealPopulacao;
						}while(pai1 == pai2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(populacao[pai1].first, populacao[pai2].first);
					}

					//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

					int diff_population = tamRealPopulacao - old_size_population;

					if(diff_population == 2) {
						if(tamRealPopulacao > tamPopulacao) {

							//remove os dois piores pais da população

							#pragma omp critical
							{
								populacao.pop_back();
								populacao.pop_back();

							//decrementa apos remover os pais

							tamRealPopulacao -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(tamRealPopulacao > tamPopulacao) {
							#pragma omp critical
							{
							populacao.pop_back(); //remove o pior pai
							tamRealPopulacao--;
							}
						}
					}
				}
				else {

					//população só contem um pai
					//aplica crossover nesse pai
					#pragma omp critical
					crossOver(populacao[0].first, populacao[0].first);

					if(tamRealPopulacao > tamPopulacao) {
						#pragma omp critical
						{
						populacao.pop_back();  //remove o pior pai
						tamRealPopulacao--;
						}
					}
				}
			}

		}


		if (tid == 1) {

			for(int i = qtdGeracoes/16; i < 2*(qtdGeracoes/16); i++) {
				int  old_size_population = tamRealPopulacao;

				// seleciona dois pais (se existe) que participarão do processo de reprodução

				if(tamRealPopulacao >= 2) {
					if(tamRealPopulacao == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(populacao[0].first, populacao[1].first);
					}
					else {

						// tamRealPopulacao > 2

						int pai1, pai2;

						do {

							//seleciona dois pais randomicamente

							pai1 = rand() % tamRealPopulacao;
							pai2 = rand() % tamRealPopulacao;
						}while(pai1 == pai2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(populacao[pai1].first, populacao[pai2].first);
					}

					//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

					int diff_population = tamRealPopulacao - old_size_population;

					if(diff_population == 2) {
						if(tamRealPopulacao > tamPopulacao) {

							//remove os dois piores pais da população

							#pragma omp critical
							{
								populacao.pop_back();
								populacao.pop_back();

							//decrementa apos remover os pais

							tamRealPopulacao -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(tamRealPopulacao > tamPopulacao) {
							#pragma omp critical
							{
							populacao.pop_back(); //remove o pior pai
							tamRealPopulacao--;
							}
						}
					}
				}
				else {

					//população só contem um pai
					//aplica crossover nesse pai
					#pragma omp critical
					crossOver(populacao[0].first, populacao[0].first);

					if(tamRealPopulacao > tamPopulacao) {
						#pragma omp critical
						{
						populacao.pop_back();  //remove o pior pai
						tamRealPopulacao--;
						}
					}
				}
			}

		}


		if (tid == 2) {

			for(int i = 2*(qtdGeracoes/16); i < 3*(qtdGeracoes/16); i++) {
				int  old_size_population = tamRealPopulacao;

				// seleciona dois pais (se existe) que participarão do processo de reprodução

				if(tamRealPopulacao >= 2) {
					if(tamRealPopulacao == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(populacao[0].first, populacao[1].first);
					}
					else {

						// tamRealPopulacao > 2

						int pai1, pai2;

						do {

							//seleciona dois pais randomicamente

							pai1 = rand() % tamRealPopulacao;
							pai2 = rand() % tamRealPopulacao;
						}while(pai1 == pai2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(populacao[pai1].first, populacao[pai2].first);
					}

					//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

					int diff_population = tamRealPopulacao - old_size_population;

					if(diff_population == 2) {
						if(tamRealPopulacao > tamPopulacao) {

							//remove os dois piores pais da população

							#pragma omp critical
							{
								populacao.pop_back();
								populacao.pop_back();

							//decrementa apos remover os pais

							tamRealPopulacao -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(tamRealPopulacao > tamPopulacao) {
							#pragma omp critical
							{
							populacao.pop_back(); //remove o pior pai
							tamRealPopulacao--;
							}
						}
					}
				}
				else {

					//população só contem um pai
					//aplica crossover nesse pai
					#pragma omp critical
					crossOver(populacao[0].first, populacao[0].first);

					if(tamRealPopulacao > tamPopulacao) {
						#pragma omp critical
						{
						populacao.pop_back();  //remove o pior pai
						tamRealPopulacao--;
						}
					}
				}
			}

		}


		if (tid == 3) {

			for(int i = 3*(qtdGeracoes/16); i < 4*(qtdGeracoes/16); i++) {
				int  old_size_population = tamRealPopulacao;

				// seleciona dois pais (se existe) que participarão do processo de reprodução

				if(tamRealPopulacao >= 2) {
					if(tamRealPopulacao == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(populacao[0].first, populacao[1].first);
					}
					else {

						// tamRealPopulacao > 2

						int pai1, pai2;

						do {

							//seleciona dois pais randomicamente

							pai1 = rand() % tamRealPopulacao;
							pai2 = rand() % tamRealPopulacao;
						}while(pai1 == pai2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(populacao[pai1].first, populacao[pai2].first);
					}

					//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

					int diff_population = tamRealPopulacao - old_size_population;

					if(diff_population == 2) {
						if(tamRealPopulacao > tamPopulacao) {

							//remove os dois piores pais da população

							#pragma omp critical
							{
								populacao.pop_back();
								populacao.pop_back();

							//decrementa apos remover os pais

							tamRealPopulacao -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(tamRealPopulacao > tamPopulacao) {
							#pragma omp critical
							{
							populacao.pop_back(); //remove o pior pai
							tamRealPopulacao--;
							}
						}
					}
				}
				else {

					//população só contem um pai
					//aplica crossover nesse pai
					#pragma omp critical
					crossOver(populacao[0].first, populacao[0].first);

					if(tamRealPopulacao > tamPopulacao) {
						#pragma omp critical
						{
						populacao.pop_back();  //remove o pior pai
						tamRealPopulacao--;
						}
					}
				}
			}

		}

		if (tid == 4) {

				for(int i = 4*(qtdGeracoes/16); i < 5*(qtdGeracoes/16); i++) {
					int  old_size_population = tamRealPopulacao;

					// seleciona dois pais (se existe) que participarão do processo de reprodução

					if(tamRealPopulacao >= 2) {
						if(tamRealPopulacao == 2) {

							//aplica crossover nos pais
							#pragma omp critical
							crossOver(populacao[0].first, populacao[1].first);
						}
						else {

							// tamRealPopulacao > 2

							int pai1, pai2;

							do {

								//seleciona dois pais randomicamente

								pai1 = rand() % tamRealPopulacao;
								pai2 = rand() % tamRealPopulacao;
							}while(pai1 == pai2);

							//aplica crossover nos dois pais
							#pragma omp critical
							crossOver(populacao[pai1].first, populacao[pai2].first);
						}

						//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

						int diff_population = tamRealPopulacao - old_size_population;

						if(diff_population == 2) {
							if(tamRealPopulacao > tamPopulacao) {

								//remove os dois piores pais da população

								#pragma omp critical
								{
									populacao.pop_back();
									populacao.pop_back();

								//decrementa apos remover os pais

								tamRealPopulacao -= 2;

								}
							}
						}
						else if(diff_population == 1) {
							if(tamRealPopulacao > tamPopulacao) {
								#pragma omp critical
								{
								populacao.pop_back(); //remove o pior pai
								tamRealPopulacao--;
								}
							}
						}
					}
					else {

						//população só contem um pai
						//aplica crossover nesse pai
						#pragma omp critical
						crossOver(populacao[0].first, populacao[0].first);

						if(tamRealPopulacao > tamPopulacao) {
							#pragma omp critical
							{
							populacao.pop_back();  //remove o pior pai
							tamRealPopulacao--;
							}
						}
					}
				}

			}

		if (tid == 5) {

				for(int i = 5*(qtdGeracoes/16); i < 6*(qtdGeracoes/16); i++) {
					int  old_size_population = tamRealPopulacao;

					// seleciona dois pais (se existe) que participarão do processo de reprodução

					if(tamRealPopulacao >= 2) {
						if(tamRealPopulacao == 2) {

							//aplica crossover nos pais
							#pragma omp critical
							crossOver(populacao[0].first, populacao[1].first);
						}
						else {

							// tamRealPopulacao > 2

							int pai1, pai2;

							do {

								//seleciona dois pais randomicamente

								pai1 = rand() % tamRealPopulacao;
								pai2 = rand() % tamRealPopulacao;
							}while(pai1 == pai2);

							//aplica crossover nos dois pais
							#pragma omp critical
							crossOver(populacao[pai1].first, populacao[pai2].first);
						}

						//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

						int diff_population = tamRealPopulacao - old_size_population;

						if(diff_population == 2) {
							if(tamRealPopulacao > tamPopulacao) {

								//remove os dois piores pais da população

								#pragma omp critical
								{
									populacao.pop_back();
									populacao.pop_back();

								//decrementa apos remover os pais

								tamRealPopulacao -= 2;

								}
							}
						}
						else if(diff_population == 1) {
							if(tamRealPopulacao > tamPopulacao) {
								#pragma omp critical
								{
								populacao.pop_back(); //remove o pior pai
								tamRealPopulacao--;
								}
							}
						}
					}
					else {

						//população só contem um pai
						//aplica crossover nesse pai
						#pragma omp critical
						crossOver(populacao[0].first, populacao[0].first);

						if(tamRealPopulacao > tamPopulacao) {
							#pragma omp critical
							{
							populacao.pop_back();  //remove o pior pai
							tamRealPopulacao--;
							}
						}
					}
				}

			}
		if (tid == 6) {

				for(int i = 6*(qtdGeracoes/16); i < 7*(qtdGeracoes/16); i++) {
					int  old_size_population = tamRealPopulacao;

					// seleciona dois pais (se existe) que participarão do processo de reprodução

					if(tamRealPopulacao >= 2) {
						if(tamRealPopulacao == 2) {

							//aplica crossover nos pais
							#pragma omp critical
							crossOver(populacao[0].first, populacao[1].first);
						}
						else {

							// tamRealPopulacao > 2

							int pai1, pai2;

							do {

								//seleciona dois pais randomicamente

								pai1 = rand() % tamRealPopulacao;
								pai2 = rand() % tamRealPopulacao;
							}while(pai1 == pai2);

							//aplica crossover nos dois pais
							#pragma omp critical
							crossOver(populacao[pai1].first, populacao[pai2].first);
						}

						//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

						int diff_population = tamRealPopulacao - old_size_population;

						if(diff_population == 2) {
							if(tamRealPopulacao > tamPopulacao) {

								//remove os dois piores pais da população

								#pragma omp critical
								{
									populacao.pop_back();
									populacao.pop_back();

								//decrementa apos remover os pais

								tamRealPopulacao -= 2;

								}
							}
						}
						else if(diff_population == 1) {
							if(tamRealPopulacao > tamPopulacao) {
								#pragma omp critical
								{
								populacao.pop_back(); //remove o pior pai
								tamRealPopulacao--;
								}
							}
						}
					}
					else {

						//população só contem um pai
						//aplica crossover nesse pai
						#pragma omp critical
						crossOver(populacao[0].first, populacao[0].first);

						if(tamRealPopulacao > tamPopulacao) {
							#pragma omp critical
							{
							populacao.pop_back();  //remove o pior pai
							tamRealPopulacao--;
							}
						}
					}
				}

			}
		if (tid == 7) {

				for(int i = 7*(qtdGeracoes/16); i < 8*(qtdGeracoes/16); i++) {
					int  old_size_population = tamRealPopulacao;

					// seleciona dois pais (se existe) que participarão do processo de reprodução

					if(tamRealPopulacao >= 2) {
						if(tamRealPopulacao == 2) {

							//aplica crossover nos pais
							#pragma omp critical
							crossOver(populacao[0].first, populacao[1].first);
						}
						else {

							// tamRealPopulacao > 2

							int pai1, pai2;

							do {

								//seleciona dois pais randomicamente

								pai1 = rand() % tamRealPopulacao;
								pai2 = rand() % tamRealPopulacao;
							}while(pai1 == pai2);

							//aplica crossover nos dois pais
							#pragma omp critical
							crossOver(populacao[pai1].first, populacao[pai2].first);
						}

						//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

						int diff_population = tamRealPopulacao - old_size_population;

						if(diff_population == 2) {
							if(tamRealPopulacao > tamPopulacao) {

								//remove os dois piores pais da população

								#pragma omp critical
								{
									populacao.pop_back();
									populacao.pop_back();

								//decrementa apos remover os pais

								tamRealPopulacao -= 2;

								}
							}
						}
						else if(diff_population == 1) {
							if(tamRealPopulacao > tamPopulacao) {
								#pragma omp critical
								{
								populacao.pop_back(); //remove o pior pai
								tamRealPopulacao--;
								}
							}
						}
					}
					else {

						//população só contem um pai
						//aplica crossover nesse pai
						#pragma omp critical
						crossOver(populacao[0].first, populacao[0].first);

						if(tamRealPopulacao > tamPopulacao) {
							#pragma omp critical
							{
							populacao.pop_back();  //remove o pior pai
							tamRealPopulacao--;
							}
						}
					}
				}

			}

		if (tid == 8) {

						for(int i = 8*(qtdGeracoes/16); i < 9*(qtdGeracoes/16); i++) {
							int  old_size_population = tamRealPopulacao;

							// seleciona dois pais (se existe) que participarão do processo de reprodução

							if(tamRealPopulacao >= 2) {
								if(tamRealPopulacao == 2) {

									//aplica crossover nos pais
									#pragma omp critical
									crossOver(populacao[0].first, populacao[1].first);
								}
								else {

									// tamRealPopulacao > 2

									int pai1, pai2;

									do {

										//seleciona dois pais randomicamente

										pai1 = rand() % tamRealPopulacao;
										pai2 = rand() % tamRealPopulacao;
									}while(pai1 == pai2);

									//aplica crossover nos dois pais
									#pragma omp critical
									crossOver(populacao[pai1].first, populacao[pai2].first);
								}

								//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

								int diff_population = tamRealPopulacao - old_size_population;

								if(diff_population == 2) {
									if(tamRealPopulacao > tamPopulacao) {

										//remove os dois piores pais da população

										#pragma omp critical
										{
											populacao.pop_back();
											populacao.pop_back();

										//decrementa apos remover os pais

										tamRealPopulacao -= 2;

										}
									}
								}
								else if(diff_population == 1) {
									if(tamRealPopulacao > tamPopulacao) {
										#pragma omp critical
										{
										populacao.pop_back(); //remove o pior pai
										tamRealPopulacao--;
										}
									}
								}
							}
							else {

								//população só contem um pai
								//aplica crossover nesse pai
								#pragma omp critical
								crossOver(populacao[0].first, populacao[0].first);

								if(tamRealPopulacao > tamPopulacao) {
									#pragma omp critical
									{
									populacao.pop_back();  //remove o pior pai
									tamRealPopulacao--;
									}
								}
							}
						}

					}
		if (tid == 9) {

						for(int i = 9*(qtdGeracoes/16); i < 10*(qtdGeracoes/16); i++) {
							int  old_size_population = tamRealPopulacao;

							// seleciona dois pais (se existe) que participarão do processo de reprodução

							if(tamRealPopulacao >= 2) {
								if(tamRealPopulacao == 2) {

									//aplica crossover nos pais
									#pragma omp critical
									crossOver(populacao[0].first, populacao[1].first);
								}
								else {

									// tamRealPopulacao > 2

									int pai1, pai2;

									do {

										//seleciona dois pais randomicamente

										pai1 = rand() % tamRealPopulacao;
										pai2 = rand() % tamRealPopulacao;
									}while(pai1 == pai2);

									//aplica crossover nos dois pais
									#pragma omp critical
									crossOver(populacao[pai1].first, populacao[pai2].first);
								}

								//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

								int diff_population = tamRealPopulacao - old_size_population;

								if(diff_population == 2) {
									if(tamRealPopulacao > tamPopulacao) {

										//remove os dois piores pais da população

										#pragma omp critical
										{
											populacao.pop_back();
											populacao.pop_back();

										//decrementa apos remover os pais

										tamRealPopulacao -= 2;

										}
									}
								}
								else if(diff_population == 1) {
									if(tamRealPopulacao > tamPopulacao) {
										#pragma omp critical
										{
										populacao.pop_back(); //remove o pior pai
										tamRealPopulacao--;
										}
									}
								}
							}
							else {

								//população só contem um pai
								//aplica crossover nesse pai
								#pragma omp critical
								crossOver(populacao[0].first, populacao[0].first);

								if(tamRealPopulacao > tamPopulacao) {
									#pragma omp critical
									{
									populacao.pop_back();  //remove o pior pai
									tamRealPopulacao--;
									}
								}
							}
						}

					}
		if (tid == 10) {

						for(int i = 10*(qtdGeracoes/16); i < 11*(qtdGeracoes/16); i++) {
							int  old_size_population = tamRealPopulacao;

							// seleciona dois pais (se existe) que participarão do processo de reprodução

							if(tamRealPopulacao >= 2) {
								if(tamRealPopulacao == 2) {

									//aplica crossover nos pais
									#pragma omp critical
									crossOver(populacao[0].first, populacao[1].first);
								}
								else {

									// tamRealPopulacao > 2

									int pai1, pai2;

									do {

										//seleciona dois pais randomicamente

										pai1 = rand() % tamRealPopulacao;
										pai2 = rand() % tamRealPopulacao;
									}while(pai1 == pai2);

									//aplica crossover nos dois pais
									#pragma omp critical
									crossOver(populacao[pai1].first, populacao[pai2].first);
								}

								//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

								int diff_population = tamRealPopulacao - old_size_population;

								if(diff_population == 2) {
									if(tamRealPopulacao > tamPopulacao) {

										//remove os dois piores pais da população

										#pragma omp critical
										{
											populacao.pop_back();
											populacao.pop_back();

										//decrementa apos remover os pais

										tamRealPopulacao -= 2;

										}
									}
								}
								else if(diff_population == 1) {
									if(tamRealPopulacao > tamPopulacao) {
										#pragma omp critical
										{
										populacao.pop_back(); //remove o pior pai
										tamRealPopulacao--;
										}
									}
								}
							}
							else {

								//população só contem um pai
								//aplica crossover nesse pai
								#pragma omp critical
								crossOver(populacao[0].first, populacao[0].first);

								if(tamRealPopulacao > tamPopulacao) {
									#pragma omp critical
									{
									populacao.pop_back();  //remove o pior pai
									tamRealPopulacao--;
									}
								}
							}
						}

					}

		if (tid == 11) {

						for(int i = 11*(qtdGeracoes/16); i < 12*(qtdGeracoes/16); i++) {
							int  old_size_population = tamRealPopulacao;

							// seleciona dois pais (se existe) que participarão do processo de reprodução

							if(tamRealPopulacao >= 2) {
								if(tamRealPopulacao == 2) {

									//aplica crossover nos pais
									#pragma omp critical
									crossOver(populacao[0].first, populacao[1].first);
								}
								else {

									// tamRealPopulacao > 2

									int pai1, pai2;

									do {

										//seleciona dois pais randomicamente

										pai1 = rand() % tamRealPopulacao;
										pai2 = rand() % tamRealPopulacao;
									}while(pai1 == pai2);

									//aplica crossover nos dois pais
									#pragma omp critical
									crossOver(populacao[pai1].first, populacao[pai2].first);
								}

								//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

								int diff_population = tamRealPopulacao - old_size_population;

								if(diff_population == 2) {
									if(tamRealPopulacao > tamPopulacao) {

										//remove os dois piores pais da população

										#pragma omp critical
										{
											populacao.pop_back();
											populacao.pop_back();

										//decrementa apos remover os pais

										tamRealPopulacao -= 2;

										}
									}
								}
								else if(diff_population == 1) {
									if(tamRealPopulacao > tamPopulacao) {
										#pragma omp critical
										{
										populacao.pop_back(); //remove o pior pai
										tamRealPopulacao--;
										}
									}
								}
							}
							else {

								//população só contem um pai
								//aplica crossover nesse pai
								#pragma omp critical
								crossOver(populacao[0].first, populacao[0].first);

								if(tamRealPopulacao > tamPopulacao) {
									#pragma omp critical
									{
									populacao.pop_back();  //remove o pior pai
									tamRealPopulacao--;
									}
								}
							}
						}

					}
		if (tid == 12) {

						for(int i = 12*(qtdGeracoes/16); i < 13*(qtdGeracoes/16); i++) {
							int  old_size_population = tamRealPopulacao;

							// seleciona dois pais (se existe) que participarão do processo de reprodução

							if(tamRealPopulacao >= 2) {
								if(tamRealPopulacao == 2) {

									//aplica crossover nos pais
									#pragma omp critical
									crossOver(populacao[0].first, populacao[1].first);
								}
								else {

									// tamRealPopulacao > 2

									int pai1, pai2;

									do {

										//seleciona dois pais randomicamente

										pai1 = rand() % tamRealPopulacao;
										pai2 = rand() % tamRealPopulacao;
									}while(pai1 == pai2);

									//aplica crossover nos dois pais
									#pragma omp critical
									crossOver(populacao[pai1].first, populacao[pai2].first);
								}

								//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

								int diff_population = tamRealPopulacao - old_size_population;

								if(diff_population == 2) {
									if(tamRealPopulacao > tamPopulacao) {

										//remove os dois piores pais da população

										#pragma omp critical
										{
											populacao.pop_back();
											populacao.pop_back();

										//decrementa apos remover os pais

										tamRealPopulacao -= 2;

										}
									}
								}
								else if(diff_population == 1) {
									if(tamRealPopulacao > tamPopulacao) {
										#pragma omp critical
										{
										populacao.pop_back(); //remove o pior pai
										tamRealPopulacao--;
										}
									}
								}
							}
							else {

								//população só contem um pai
								//aplica crossover nesse pai
								#pragma omp critical
								crossOver(populacao[0].first, populacao[0].first);

								if(tamRealPopulacao > tamPopulacao) {
									#pragma omp critical
									{
									populacao.pop_back();  //remove o pior pai
									tamRealPopulacao--;
									}
								}
							}
						}

					}
		if (tid == 14) {

						for(int i = 14*(qtdGeracoes/16); i < 15*(qtdGeracoes/16); i++) {
							int  old_size_population = tamRealPopulacao;

							// seleciona dois pais (se existe) que participarão do processo de reprodução

							if(tamRealPopulacao >= 2) {
								if(tamRealPopulacao == 2) {

									//aplica crossover nos pais
									#pragma omp critical
									crossOver(populacao[0].first, populacao[1].first);
								}
								else {

									// tamRealPopulacao > 2

									int pai1, pai2;

									do {

										//seleciona dois pais randomicamente

										pai1 = rand() % tamRealPopulacao;
										pai2 = rand() % tamRealPopulacao;
									}while(pai1 == pai2);

									//aplica crossover nos dois pais
									#pragma omp critical
									crossOver(populacao[pai1].first, populacao[pai2].first);
								}

								//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

								int diff_population = tamRealPopulacao - old_size_population;

								if(diff_population == 2) {
									if(tamRealPopulacao > tamPopulacao) {

										//remove os dois piores pais da população

										#pragma omp critical
										{
											populacao.pop_back();
											populacao.pop_back();

										//decrementa apos remover os pais

										tamRealPopulacao -= 2;

										}
									}
								}
								else if(diff_population == 1) {
									if(tamRealPopulacao > tamPopulacao) {
										#pragma omp critical
										{
										populacao.pop_back(); //remove o pior pai
										tamRealPopulacao--;
										}
									}
								}
							}
							else {

								//população só contem um pai
								//aplica crossover nesse pai
								#pragma omp critical
								crossOver(populacao[0].first, populacao[0].first);

								if(tamRealPopulacao > tamPopulacao) {
									#pragma omp critical
									{
									populacao.pop_back();  //remove o pior pai
									tamRealPopulacao--;
									}
								}
							}
						}

					}
		if (tid == 15) {

						for(int i = 15*(qtdGeracoes/16); i < qtdGeracoes; i++) {
							int  old_size_population = tamRealPopulacao;

							// seleciona dois pais (se existe) que participarão do processo de reprodução

							if(tamRealPopulacao >= 2) {
								if(tamRealPopulacao == 2) {

									//aplica crossover nos pais
									#pragma omp critical
									crossOver(populacao[0].first, populacao[1].first);
								}
								else {

									// tamRealPopulacao > 2

									int pai1, pai2;

									do {

										//seleciona dois pais randomicamente

										pai1 = rand() % tamRealPopulacao;
										pai2 = rand() % tamRealPopulacao;
									}while(pai1 == pai2);

									//aplica crossover nos dois pais
									#pragma omp critical
									crossOver(populacao[pai1].first, populacao[pai2].first);
								}

								//recupera a diferença entre os tamanhos de população antigo e atual pra ver se a mesma cresceu

								int diff_population = tamRealPopulacao - old_size_population;

								if(diff_population == 2) {
									if(tamRealPopulacao > tamPopulacao) {

										//remove os dois piores pais da população

										#pragma omp critical
										{
											populacao.pop_back();
											populacao.pop_back();

										//decrementa apos remover os pais

										tamRealPopulacao -= 2;

										}
									}
								}
								else if(diff_population == 1) {
									if(tamRealPopulacao > tamPopulacao) {
										#pragma omp critical
										{
										populacao.pop_back(); //remove o pior pai
										tamRealPopulacao--;
										}
									}
								}
							}
							else {

								//população só contem um pai
								//aplica crossover nesse pai
								#pragma omp critical
								crossOver(populacao[0].first, populacao[0].first);

								if(tamRealPopulacao > tamPopulacao) {
									#pragma omp critical
									{
									populacao.pop_back();  //remove o pior pai
									tamRealPopulacao--;
									}
								}
							}
						}

					}

	} //fim da paralelização



*/






	if(mostrar_Populacao == true)
		mostrarPopulacao(); /*mostra a população*/

	cout << "\nMelhor solucao: ";
	const vector<int>& vec = populacao[0].first;
	for(int i = 0; i < graph->numVertices; i++)
		cout << vec[i] << " ";
	cout << graph->verticeInicial;
	cout << " | Custo: " << populacao[0].second;
}


int Genetic::getMelhorCusto()
{
	if(tamRealPopulacao > 0)
		return populacao[0].second;
	return -1;
}
