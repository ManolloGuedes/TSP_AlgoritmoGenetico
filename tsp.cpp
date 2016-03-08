#include <iostream>
#include <omp.h>
#include <algorithm> // sort, next_permutation
#include "tsp.h"
using namespace std;


Graph::Graph(int V, int initial_vertex, bool random_graph) { /*construtor do grafo*/
	if(V < 1) { /*verifica se o n�mero de v�rtices � menor que 1*/
		cout << "Erro: n�mero de v�rtices <= 0\n";
		exit(1);
	}

	this->V = V; /*atribui o n�mero de v�rtices*/
	this->initial_vertex = initial_vertex; /*atribui v�rtice inicial*/
	this->total_edges = 0; /*inicializa o total de arestas com 0*/

	if(random_graph)
		generatesGraph();
}


void Graph::generatesGraph() {
	vector<int> vec;

	/*cria o vetor*/
	for(int i = 0; i < V; i++)
		vec.push_back(i);

	/*gera uma permuta��o aleat�ria*/
	random_shuffle(vec.begin(), vec.end());

	initial_vertex = vec[0]; /*atualiza vertice inicial*/

	int i, weight;
	for(i = 0; i <= V; i++) {
		weight = rand() % V + 1; /*peso aleat�rio no intervalo [1,V]*/

		if(i + 1 < V)
			addEdge(vec[i], vec[i + 1], weight);
		else {
			/*adiciona �ltima aresta*/
			addEdge(vec[i], vec[0], weight);
			break;
		}
	}
	/*calcula o limite m�ximo de arestas do grafo*/
	int limit_edges = V * (V - 1); /*um v�rtice pode ter saida para todos v�rtices, menos pra ele mesmo*/
	int size_edges = rand() % (2 * limit_edges) + limit_edges;

	/*adiciona outras arestas aleatoriamente*/
	for(int i = 0; i < size_edges; i++) {
		int src = rand() % V; /*origem aleat�ria*/
		int dest = rand() % V; /*destino aleat�rio*/
		weight = rand() % V + 1; /*peso aleat�rio no intervalo [1,V]*/
		if(src != dest) {
			addEdge(vec[src], vec[dest], weight);
			addEdge(vec[dest], vec[src], weight);
		}
	}
}


void Graph::showInfoGraph() {
	cout << "Mostrando informa��es do grafo:\n\n";
	cout << "Numero de v�rtices: " << V;
	cout << "\nNumero de arestas: " << map_edges.size() << "\n";
}


void Graph::addEdge(int src, int dest, int weight) { /*adiciona uma aresta*/
	map_edges[make_pair(src, dest)] = weight; /*adiciona uma aresta no mapa*/
}


void Graph::showGraph() { /*mostra todas as liga��es do grafo*/
	map<pair<int, int>, int>::iterator it;
	for(it = map_edges.begin(); it != map_edges.end(); ++it)
		cout << it->first.first << " ligado ao vertice " << it->first.second << " com peso " << it->second << endl;
}


int Graph::existsEdge(int src, int dest) { /*checa se existe uma aresta*/
	map<pair<int, int>,int>::iterator it = map_edges.find(make_pair(src, dest));

	if(it != map_edges.end())
		return it->second; /*retorna o custo*/
	return -1;
}


/*construtor gen�tico*/
Genetic::Genetic(Graph* graph, int size_population, int generations, int mutation_rate, bool show_population) {
	if(size_population < 1) { /*checa se o tamanho da popula��o � menor que 1*/
		cout << "Erro: size_population < 1\n";
		exit(1);
	}
	else if(mutation_rate < 0 || mutation_rate > 100) { /*checa se a taxa de muta��o � menor que 0 ou maior que 100*/
		cout << "Erro: mutation_rate deve ser >= 0 e <= 100\n";
		exit(1);
	}
	this->graph = graph;
	this->size_population = size_population;
	this->real_size_population = 0;
	this->generations = generations;
	this->mutation_rate = mutation_rate;
	this->show_population = show_population;
}



// checa se a solu��o � v�lida, ent�o retorna o custo total do taminho ou ent�o returna -1
int Genetic::isValidSolution(vector<int>& solution) {
	int total_cost = 0;
	set<int> set_solution;

	/*verifica se n�o contem repeti��o de elementos*/
	for(int i = 0; i < graph->V; i++)
		set_solution.insert(solution[i]);

	if(set_solution.size() != (unsigned)graph->V)
		return -1;

	/*checa se as conex�es s�o v�lidas*/
	for(int i = 0; i < graph->V; i++) {
		if(i + 1 <  graph->V) {
			int cost = graph->existsEdge(solution[i], solution[i+1]);

			/*checa se existe conex�o*/
			if(cost == -1)
				return -1;
			else
				total_cost += cost;
		}
		else {
			int cost = graph->existsEdge(solution[i], solution[0]);

			/*checa se existe conex�o*/
			if(cost == -1)
				return -1;
			else
				total_cost += cost;
			break;
		}
	}
	return total_cost;
}


bool Genetic::existsChromosome(const vector<int> & v) {
	/*checa se existe cromossomos na popula��o*/
	for(vector<pair<vector<int>, int> >::iterator it=population.begin(); it!=population.end(); ++it) {
		const vector<int>& vec = (*it).first; /*recupera o vetor*/
		if(equal(v.begin(), v.end(), vec.begin())) /*compara os vetores*/
			return true;
	}
	return false;
}


void Genetic::initialPopulation() { /*gera a popula��o inicial*/
	vector<int> parent;

	/*insere v�rtice inicial no pai*/
	parent.push_back(graph->initial_vertex);

	/*cria o pai*/
	for(int i = 0; i < graph->V; i++) {
		if(i != graph->initial_vertex)
			parent.push_back(i);
	}

	int total_cost = isValidSolution(parent);

	if(total_cost != -1) { /*checa se o pai � valido*/
		population.push_back(make_pair(parent, total_cost)); /*insere na popula��o*/
		real_size_population++; /*incrementa tamanho real da popula��o*/
	}

	/*faz permuta��es aleat�rias "generations" vezes*/
	for(int i = 0; i < generations; i++) {
		/*gera uma permuta��o aleat�ria*/
		random_shuffle(parent.begin() + 1, parent.begin() + (rand() % (graph->V - 1) + 1));

		int total_cost = isValidSolution(parent); /*checa se a solu��o � v�lida*/

		/*verifica se a permuta��o � uma solu��o v�lida e se n�o existe*/
		if(total_cost != -1 && !existsChromosome(parent)) {
			population.push_back(make_pair(parent, total_cost)); /*adiciona na popula��o*/
			real_size_population++; /*incrementa popula��o real*/
		}
		if(real_size_population == size_population) /*verifica o tamanho da popula��o*/
			break;
	}

	/*verifica se a popula��o real � 0*/
	if(real_size_population == 0)
		cout << "\nPopula��o inicial vazia ;( tente executar o algoritmo novamente...";
	else
		sort(population.begin(), population.end(), sort_pred()); /*ordena a popula��o*/
}


void Genetic::showPopulation() {
	cout << "\nMostrando solu��es...\n\n";
	for(vector<pair<vector<int>, int> >::iterator it=population.begin(); it!=population.end(); ++it) {
		const vector<int>& vec = (*it).first; /*recupera o vetor*/

		for(int i = 0; i < graph->V; i++)
			cout << vec[i] << " ";
		cout << graph->initial_vertex;
		cout << " | Custo: " << (*it).second << "\n\n";
	}
	cout << "\nTamanho da popula��o: " << real_size_population << endl;
}


/*insere no vetor usando busca bin�ria*/
void Genetic::insertBinarySearch(vector<int>& child, int total_cost) {
	int imin = 0;
	int imax = real_size_population - 1;

	while(imax >= imin) {
		int imid = imin + (imax - imin) / 2;

		if(total_cost == population[imid].second) {
			population.insert(population.begin() + imid, make_pair(child, total_cost));
			return;
		}
		else if(total_cost > population[imid].second)
			imin = imid + 1;
		else
			imax = imid - 1;
	}
	population.insert(population.begin() + imin, make_pair(child, total_cost));
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
void Genetic::crossOver(vector<int>& parent1, vector<int>& parent2) {
	vector<int> child1, child2;

	/*mapa de genes, verifica se j� est�o selecionados*/
	map<int, int> genes1, genes2;

	for(int i = 0; i < graph->V; i++) {
		/*Inicialmente os genes n�o est�o utilizados*/
		genes1[parent1[i]] = 0;
		genes2[parent2[i]] = 0;
	}

	/*gera pontos aleat�rios*/

	int point1 = rand() % (graph->V - 1) + 1;
	int point2 = rand() % (graph->V - point1) + point1;


	/*Ajusta os pontos se eles s�o iguais*/
	if(point1 == point2) {
		if(point1 - 1 > 1)
			point1--;
		else if(point2 + 1 < graph->V)
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

	// at� que point1, child1 receba genes de parent1
	// e child2 receba genes de parent2
	for(int i = 0; i < point1; i++) {
		/*adiciona genes*/
		child1.push_back(parent1[i]);
		child2.push_back(parent2[i]);
		/*marca genes*/
		genes1[parent1[i]] = 1;
		genes2[parent2[i]] = 1;
	}

	/*marca genes restantes*/
	for(int i = point2 + 1; i < graph->V; i++) {
		genes1[parent1[i]] = 1;
		genes2[parent2[i]] = 1;
	}

	/*	aqui est� a subsequencia invertida
		child1 recebe genes de parent2 e
		child2 recebe genes de parent1 */
	for(int i = point2; i >= point1; i--) {
		if(genes1[parent2[i]] == 0) { /*se o gene n�o foi usado*/
			child1.push_back(parent2[i]);
			genes1[parent2[i]] = 1; /*marca o gene*/
		}
		else {
			/*se o gene j� est� sendo utilizado escolhe um que n�o esteja*/
			for(map<int, int>::iterator it = genes1.begin(); it != genes1.end(); ++it) {
				if(it->second == 0) { /*checa se n�o foi usado*/
					child1.push_back(it->first);
					genes1[it->first] = 1; /*marca como usado*/
					break; /*deixa o loop*/
				}
			}
		}

		if(genes2[parent1[i]] == 0) { /*se o gene n�o foi usado*/
			child2.push_back(parent1[i]);
			genes2[parent1[i]] = 1; /*marca o gene*/
		}
		else {
			/*se o gene j� est� sendo utilizado escolhe um que n�o esteja*/
			for(map<int, int>::iterator it = genes2.begin(); it != genes2.end(); ++it) {
				if(it->second == 0) { /*verifica se n�o est� sendo usado*/
					child2.push_back(it->first);
					genes2[it->first] = 1; /*marca como usado*/
					break; /*deixa o loop*/
				}
			}
		}
	}

	// genes restantes: child1 recebe genes do parent1
	// e child2 recebe genes do parent2
	for(int i = point2 + 1; i < graph->V; i++)
	{
		child1.push_back(parent1[i]);
		child2.push_back(parent2[i]);
	}

	/*muta��o*/
	int mutation = rand() % 100 + 1; /* n�mero rand�mico no intervalo [1,100] */
	if(mutation <= mutation_rate) /* checa se o numero randomico <= taxa de muta��o*/
	{
		/*faz uma muta��o: mudan�a de 2 genes*/

		int index_gene1, index_gene2;
		index_gene1 = rand() % (graph->V - 1) + 1;
		index_gene2 = rand() % (graph->V - 1) + 1;

		/* faz para child1 */
		int aux = child1[index_gene1];
		child1[index_gene1] = child1[index_gene2];
		child1[index_gene2] = aux;

		/* faz para child2 */
		aux = child2[index_gene1];
		child2[index_gene1] = child2[index_gene2];
		child2[index_gene2] = aux;
	}

	int total_cost_child1 = isValidSolution(child1);
	int total_cost_child2 = isValidSolution(child2);



	/*verifica se � uma solu��o v�lida e n�o existe na popula��o*/
	if(total_cost_child1 != -1 && !existsChromosome(child1)) {
		/*adiciona o filho na popula��o*/
		insertBinarySearch(child1, total_cost_child1); /*usa busca bin�ria para inserir*/
		#pragma omp critical
		{
		real_size_population++; /*incrementa popula��o real*/
		}
	}

	/*mesmo procedimento anterior*/
	if(total_cost_child2 != -1 && !existsChromosome(child2)) {

		insertBinarySearch(child2, total_cost_child2);
		#pragma omp critical
		{
		real_size_population++;
		}
	}
}


/*executa algoritmo gen�tico*/
void Genetic::run() {
	initialPopulation(); /*recupera popula��o inicial*/

	if(real_size_population == 0)
		return;


// SEM THREAD
/*


	for(int i = 0; i < generations; i++) {
		int  old_size_population = real_size_population;


		if(real_size_population >= 2) {
			if(real_size_population == 2) {
				crossOver(population[0].first, population[1].first);
			}
			else {

				int parent1, parent2;

				do {
					parent1 = rand() % real_size_population;
					parent2 = rand() % real_size_population;
				}while(parent1 == parent2);

				crossOver(population[parent1].first, population[parent2].first);
			}

			int diff_population = real_size_population - old_size_population;

			if(diff_population == 2) {
				if(real_size_population > size_population) {

					population.pop_back();
					population.pop_back();


					real_size_population -= 2;
				}
			}
			else if(diff_population == 1) {
				if(real_size_population > size_population) {
					population.pop_back();
					real_size_population--;
				}
			}
		}
		else {
			crossOver(population[0].first, population[0].first);

			if(real_size_population > size_population) {
				population.pop_back();
				real_size_population--;
			}
		}

	}



*/

// 2 THREADS

/*

	#pragma omp parallel num_threads(2)
	{

	int tid = omp_get_thread_num();

		if (tid == 0) {

			for(int i = 0; i < generations/2; i++) {
				int  old_size_population = real_size_population;

				// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

				if(real_size_population >= 2) {
					if(real_size_population == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(population[0].first, population[1].first);
					}
					else {

						// real_size_population > 2

						int parent1, parent2;

						do {

							//seleciona dois pais randomicamente

							parent1 = rand() % real_size_population;
							parent2 = rand() % real_size_population;
						}while(parent1 == parent2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(population[parent1].first, population[parent2].first);
					}

					//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

					int diff_population = real_size_population - old_size_population;

					if(diff_population == 2) {
						if(real_size_population > size_population) {

							//remove os dois piores pais da popula��o

							#pragma omp critical
							{
								population.pop_back();
								population.pop_back();

							//decrementa apos remover os pais

							real_size_population -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(real_size_population > size_population) {
							#pragma omp critical
							{
							population.pop_back(); //remove o pior pai
							real_size_population--;
							}
						}
					}
				}
				else {

					//popula��o s� contem um pai
					//aplica crossover nesse pai
					#pragma omp critical
					crossOver(population[0].first, population[0].first);

					if(real_size_population > size_population) {
						#pragma omp critical
						{
						population.pop_back();  //remove o pior pai
						real_size_population--;
						}
					}
				}
			}

		}


		if (tid == 1) {

			for(int i = generations/2; i < generations; i++) {
				int  old_size_population = real_size_population;

				// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

				if(real_size_population >= 2) {
					if(real_size_population == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(population[0].first, population[1].first);
					}
					else {

						// real_size_population > 2

						int parent1, parent2;

						do {

							//seleciona dois pais randomicamente

							parent1 = rand() % real_size_population;
							parent2 = rand() % real_size_population;
						}while(parent1 == parent2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(population[parent1].first, population[parent2].first);
					}

					//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

					int diff_population = real_size_population - old_size_population;

					if(diff_population == 2) {
						if(real_size_population > size_population) {

							//remove os dois piores pais da popula��o

							#pragma omp critical
							{
								population.pop_back();
								population.pop_back();

							//decrementa apos remover os pais

							real_size_population -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(real_size_population > size_population) {
							#pragma omp critical
							{
							population.pop_back(); //remove o pior pai
							real_size_population--;
							}
						}
					}
				}
				else {

					//popula��o s� contem um pai
					//aplica crossover nesse pai
				    #pragma omp critical
					crossOver(population[0].first, population[0].first);

					if(real_size_population > size_population) {
						#pragma omp critical
						{
						population.pop_back();  //remove o pior pai
						real_size_population--;
						}
					}
				}
			}

		}

	} //fim da paraleliza��o


*/

// 4 THREADS

/*
	#pragma omp parallel num_threads(3)
	{

	int tid = omp_get_thread_num();

		if (tid == 0) {

			for(int i = 0; i < generations/4; i++) {
				int  old_size_population = real_size_population;

				// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

				if(real_size_population >= 2) {
					if(real_size_population == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(population[0].first, population[1].first);
					}
					else {

						// real_size_population > 2

						int parent1, parent2;

						do {

							//seleciona dois pais randomicamente

							parent1 = rand() % real_size_population;
							parent2 = rand() % real_size_population;
						}while(parent1 == parent2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(population[parent1].first, population[parent2].first);
					}

					//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

					int diff_population = real_size_population - old_size_population;

					if(diff_population == 2) {
						if(real_size_population > size_population) {

							//remove os dois piores pais da popula��o

							#pragma omp critical
							{
								population.pop_back();
								population.pop_back();

							//decrementa apos remover os pais

							real_size_population -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(real_size_population > size_population) {
							#pragma omp critical
							{
							population.pop_back(); //remove o pior pai
							real_size_population--;
							}
						}
					}
				}
				else {

					//popula��o s� contem um pai
					//aplica crossover nesse pai
					#pragma omp critical
					crossOver(population[0].first, population[0].first);

					if(real_size_population > size_population) {
						#pragma omp critical
						{
						population.pop_back();  //remove o pior pai
						real_size_population--;
						}
					}
				}
			}

		}


		if (tid == 1) {

			for(int i = generations/4; i < 2*(generations/4); i++) {
				int  old_size_population = real_size_population;

				// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

				if(real_size_population >= 2) {
					if(real_size_population == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(population[0].first, population[1].first);
					}
					else {

						// real_size_population > 2

						int parent1, parent2;

						do {

							//seleciona dois pais randomicamente

							parent1 = rand() % real_size_population;
							parent2 = rand() % real_size_population;
						}while(parent1 == parent2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(population[parent1].first, population[parent2].first);
					}

					//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

					int diff_population = real_size_population - old_size_population;

					if(diff_population == 2) {
						if(real_size_population > size_population) {

							//remove os dois piores pais da popula��o

							#pragma omp critical
							{
								population.pop_back();
								population.pop_back();

							//decrementa apos remover os pais

							real_size_population -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(real_size_population > size_population) {
							#pragma omp critical
							{
							population.pop_back(); //remove o pior pai
							real_size_population--;
							}
						}
					}
				}
				else {

					//popula��o s� contem um pai
					//aplica crossover nesse pai
					#pragma omp critical
					crossOver(population[0].first, population[0].first);

					if(real_size_population > size_population) {
						#pragma omp critical
						{
						population.pop_back();  //remove o pior pai
						real_size_population--;
						}
					}
				}
			}

		}


		if (tid == 2) {

			for(int i = 2*(generations/4); i < 3*(generations/4); i++) {
				int  old_size_population = real_size_population;

				// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

				if(real_size_population >= 2) {
					if(real_size_population == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(population[0].first, population[1].first);
					}
					else {

						// real_size_population > 2

						int parent1, parent2;

						do {

							//seleciona dois pais randomicamente

							parent1 = rand() % real_size_population;
							parent2 = rand() % real_size_population;
						}while(parent1 == parent2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(population[parent1].first, population[parent2].first);
					}

					//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

					int diff_population = real_size_population - old_size_population;

					if(diff_population == 2) {
						if(real_size_population > size_population) {

							//remove os dois piores pais da popula��o

							#pragma omp critical
							{
								population.pop_back();
								population.pop_back();

							//decrementa apos remover os pais

							real_size_population -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(real_size_population > size_population) {
							#pragma omp critical
							{
							population.pop_back(); //remove o pior pai
							real_size_population--;
							}
						}
					}
				}
				else {

					//popula��o s� contem um pai
					//aplica crossover nesse pai
					#pragma omp critical
					crossOver(population[0].first, population[0].first);

					if(real_size_population > size_population) {
						#pragma omp critical
						{
						population.pop_back();  //remove o pior pai
						real_size_population--;
						}
					}
				}
			}

		}


		if (tid == 3) {

			for(int i = 3*(generations/4); i < generations; i++) {
				int  old_size_population = real_size_population;

				// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

				if(real_size_population >= 2) {
					if(real_size_population == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(population[0].first, population[1].first);
					}
					else {

						// real_size_population > 2

						int parent1, parent2;

						do {

							//seleciona dois pais randomicamente

							parent1 = rand() % real_size_population;
							parent2 = rand() % real_size_population;
						}while(parent1 == parent2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(population[parent1].first, population[parent2].first);
					}

					//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

					int diff_population = real_size_population - old_size_population;

					if(diff_population == 2) {
						if(real_size_population > size_population) {

							//remove os dois piores pais da popula��o

							#pragma omp critical
							{
								population.pop_back();
								population.pop_back();

							//decrementa apos remover os pais

							real_size_population -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(real_size_population > size_population) {
							#pragma omp critical
							{
							population.pop_back(); //remove o pior pai
							real_size_population--;
							}
						}
					}
				}
				else {

					//popula��o s� contem um pai
					//aplica crossover nesse pai
					#pragma omp critical
					crossOver(population[0].first, population[0].first);

					if(real_size_population > size_population) {
						#pragma omp critical
						{
						population.pop_back();  //remove o pior pai
						real_size_population--;
						}
					}
				}
			}

		}

	} //fim da paraleliza��o



*/



// 8 THREADS
/*

#pragma omp parallel num_threads(3)
{

int tid = omp_get_thread_num();

	if (tid == 0) {

		for(int i = 0; i < generations/8; i++) {
			int  old_size_population = real_size_population;

			// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

			if(real_size_population >= 2) {
				if(real_size_population == 2) {

					//aplica crossover nos pais
					#pragma omp critical
					crossOver(population[0].first, population[1].first);
				}
				else {

					// real_size_population > 2

					int parent1, parent2;

					do {

						//seleciona dois pais randomicamente

						parent1 = rand() % real_size_population;
						parent2 = rand() % real_size_population;
					}while(parent1 == parent2);

					//aplica crossover nos dois pais
					#pragma omp critical
					crossOver(population[parent1].first, population[parent2].first);
				}

				//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

				int diff_population = real_size_population - old_size_population;

				if(diff_population == 2) {
					if(real_size_population > size_population) {

						//remove os dois piores pais da popula��o

						#pragma omp critical
						{
							population.pop_back();
							population.pop_back();

						//decrementa apos remover os pais

						real_size_population -= 2;

						}
					}
				}
				else if(diff_population == 1) {
					if(real_size_population > size_population) {
						#pragma omp critical
						{
						population.pop_back(); //remove o pior pai
						real_size_population--;
						}
					}
				}
			}
			else {

				//popula��o s� contem um pai
				//aplica crossover nesse pai
				#pragma omp critical
				crossOver(population[0].first, population[0].first);

				if(real_size_population > size_population) {
					#pragma omp critical
					{
					population.pop_back();  //remove o pior pai
					real_size_population--;
					}
				}
			}
		}

	}


	if (tid == 1) {

		for(int i = generations/8; i < 2*(generations/8); i++) {
			int  old_size_population = real_size_population;

			// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

			if(real_size_population >= 2) {
				if(real_size_population == 2) {

					//aplica crossover nos pais
					#pragma omp critical
					crossOver(population[0].first, population[1].first);
				}
				else {

					// real_size_population > 2

					int parent1, parent2;

					do {

						//seleciona dois pais randomicamente

						parent1 = rand() % real_size_population;
						parent2 = rand() % real_size_population;
					}while(parent1 == parent2);

					//aplica crossover nos dois pais
					#pragma omp critical
					crossOver(population[parent1].first, population[parent2].first);
				}

				//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

				int diff_population = real_size_population - old_size_population;

				if(diff_population == 2) {
					if(real_size_population > size_population) {

						//remove os dois piores pais da popula��o

						#pragma omp critical
						{
							population.pop_back();
							population.pop_back();

						//decrementa apos remover os pais

						real_size_population -= 2;

						}
					}
				}
				else if(diff_population == 1) {
					if(real_size_population > size_population) {
						#pragma omp critical
						{
						population.pop_back(); //remove o pior pai
						real_size_population--;
						}
					}
				}
			}
			else {

				//popula��o s� contem um pai
				//aplica crossover nesse pai
				#pragma omp critical
				crossOver(population[0].first, population[0].first);

				if(real_size_population > size_population) {
					#pragma omp critical
					{
					population.pop_back();  //remove o pior pai
					real_size_population--;
					}
				}
			}
		}

	}


	if (tid == 2) {

		for(int i = 2*(generations/8); i < 3*(generations/8); i++) {
			int  old_size_population = real_size_population;

			// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

			if(real_size_population >= 2) {
				if(real_size_population == 2) {

					//aplica crossover nos pais
					#pragma omp critical
					crossOver(population[0].first, population[1].first);
				}
				else {

					// real_size_population > 2

					int parent1, parent2;

					do {

						//seleciona dois pais randomicamente

						parent1 = rand() % real_size_population;
						parent2 = rand() % real_size_population;
					}while(parent1 == parent2);

					//aplica crossover nos dois pais
					#pragma omp critical
					crossOver(population[parent1].first, population[parent2].first);
				}

				//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

				int diff_population = real_size_population - old_size_population;

				if(diff_population == 2) {
					if(real_size_population > size_population) {

						//remove os dois piores pais da popula��o

						#pragma omp critical
						{
							population.pop_back();
							population.pop_back();

						//decrementa apos remover os pais

						real_size_population -= 2;

						}
					}
				}
				else if(diff_population == 1) {
					if(real_size_population > size_population) {
						#pragma omp critical
						{
						population.pop_back(); //remove o pior pai
						real_size_population--;
						}
					}
				}
			}
			else {

				//popula��o s� contem um pai
				//aplica crossover nesse pai
				#pragma omp critical
				crossOver(population[0].first, population[0].first);

				if(real_size_population > size_population) {
					#pragma omp critical
					{
					population.pop_back();  //remove o pior pai
					real_size_population--;
					}
				}
			}
		}

	}


	if (tid == 3) {

		for(int i = 3*(generations/8); i < 4*(generations/8); i++) {
			int  old_size_population = real_size_population;

			// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

			if(real_size_population >= 2) {
				if(real_size_population == 2) {

					//aplica crossover nos pais
					#pragma omp critical
					crossOver(population[0].first, population[1].first);
				}
				else {

					// real_size_population > 2

					int parent1, parent2;

					do {

						//seleciona dois pais randomicamente

						parent1 = rand() % real_size_population;
						parent2 = rand() % real_size_population;
					}while(parent1 == parent2);

					//aplica crossover nos dois pais
					#pragma omp critical
					crossOver(population[parent1].first, population[parent2].first);
				}

				//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

				int diff_population = real_size_population - old_size_population;

				if(diff_population == 2) {
					if(real_size_population > size_population) {

						//remove os dois piores pais da popula��o

						#pragma omp critical
						{
							population.pop_back();
							population.pop_back();

						//decrementa apos remover os pais

						real_size_population -= 2;

						}
					}
				}
				else if(diff_population == 1) {
					if(real_size_population > size_population) {
						#pragma omp critical
						{
						population.pop_back(); //remove o pior pai
						real_size_population--;
						}
					}
				}
			}
			else {

				//popula��o s� contem um pai
				//aplica crossover nesse pai
				#pragma omp critical
				crossOver(population[0].first, population[0].first);

				if(real_size_population > size_population) {
					#pragma omp critical
					{
					population.pop_back();  //remove o pior pai
					real_size_population--;
					}
				}
			}
		}

	}

	if (tid == 4) {

			for(int i = 4*(generations/8); i < 5*(generations/8); i++) {
				int  old_size_population = real_size_population;

				// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

				if(real_size_population >= 2) {
					if(real_size_population == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(population[0].first, population[1].first);
					}
					else {

						// real_size_population > 2

						int parent1, parent2;

						do {

							//seleciona dois pais randomicamente

							parent1 = rand() % real_size_population;
							parent2 = rand() % real_size_population;
						}while(parent1 == parent2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(population[parent1].first, population[parent2].first);
					}

					//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

					int diff_population = real_size_population - old_size_population;

					if(diff_population == 2) {
						if(real_size_population > size_population) {

							//remove os dois piores pais da popula��o

							#pragma omp critical
							{
								population.pop_back();
								population.pop_back();

							//decrementa apos remover os pais

							real_size_population -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(real_size_population > size_population) {
							#pragma omp critical
							{
							population.pop_back(); //remove o pior pai
							real_size_population--;
							}
						}
					}
				}
				else {

					//popula��o s� contem um pai
					//aplica crossover nesse pai
					#pragma omp critical
					crossOver(population[0].first, population[0].first);

					if(real_size_population > size_population) {
						#pragma omp critical
						{
						population.pop_back();  //remove o pior pai
						real_size_population--;
						}
					}
				}
			}

		}

	if (tid == 5) {

			for(int i = 5*(generations/8); i < 6*(generations/8); i++) {
				int  old_size_population = real_size_population;

				// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

				if(real_size_population >= 2) {
					if(real_size_population == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(population[0].first, population[1].first);
					}
					else {

						// real_size_population > 2

						int parent1, parent2;

						do {

							//seleciona dois pais randomicamente

							parent1 = rand() % real_size_population;
							parent2 = rand() % real_size_population;
						}while(parent1 == parent2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(population[parent1].first, population[parent2].first);
					}

					//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

					int diff_population = real_size_population - old_size_population;

					if(diff_population == 2) {
						if(real_size_population > size_population) {

							//remove os dois piores pais da popula��o

							#pragma omp critical
							{
								population.pop_back();
								population.pop_back();

							//decrementa apos remover os pais

							real_size_population -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(real_size_population > size_population) {
							#pragma omp critical
							{
							population.pop_back(); //remove o pior pai
							real_size_population--;
							}
						}
					}
				}
				else {

					//popula��o s� contem um pai
					//aplica crossover nesse pai
					#pragma omp critical
					crossOver(population[0].first, population[0].first);

					if(real_size_population > size_population) {
						#pragma omp critical
						{
						population.pop_back();  //remove o pior pai
						real_size_population--;
						}
					}
				}
			}

		}
	if (tid == 6) {

			for(int i = 6*(generations/8); i < 7*(generations/8); i++) {
				int  old_size_population = real_size_population;

				// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

				if(real_size_population >= 2) {
					if(real_size_population == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(population[0].first, population[1].first);
					}
					else {

						// real_size_population > 2

						int parent1, parent2;

						do {

							//seleciona dois pais randomicamente

							parent1 = rand() % real_size_population;
							parent2 = rand() % real_size_population;
						}while(parent1 == parent2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(population[parent1].first, population[parent2].first);
					}

					//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

					int diff_population = real_size_population - old_size_population;

					if(diff_population == 2) {
						if(real_size_population > size_population) {

							//remove os dois piores pais da popula��o

							#pragma omp critical
							{
								population.pop_back();
								population.pop_back();

							//decrementa apos remover os pais

							real_size_population -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(real_size_population > size_population) {
							#pragma omp critical
							{
							population.pop_back(); //remove o pior pai
							real_size_population--;
							}
						}
					}
				}
				else {

					//popula��o s� contem um pai
					//aplica crossover nesse pai
					#pragma omp critical
					crossOver(population[0].first, population[0].first);

					if(real_size_population > size_population) {
						#pragma omp critical
						{
						population.pop_back();  //remove o pior pai
						real_size_population--;
						}
					}
				}
			}

		}
	if (tid == 7) {

			for(int i = 7*(generations/8); i < generations; i++) {
				int  old_size_population = real_size_population;

				// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

				if(real_size_population >= 2) {
					if(real_size_population == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(population[0].first, population[1].first);
					}
					else {

						// real_size_population > 2

						int parent1, parent2;

						do {

							//seleciona dois pais randomicamente

							parent1 = rand() % real_size_population;
							parent2 = rand() % real_size_population;
						}while(parent1 == parent2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(population[parent1].first, population[parent2].first);
					}

					//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

					int diff_population = real_size_population - old_size_population;

					if(diff_population == 2) {
						if(real_size_population > size_population) {

							//remove os dois piores pais da popula��o

							#pragma omp critical
							{
								population.pop_back();
								population.pop_back();

							//decrementa apos remover os pais

							real_size_population -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(real_size_population > size_population) {
							#pragma omp critical
							{
							population.pop_back(); //remove o pior pai
							real_size_population--;
							}
						}
					}
				}
				else {

					//popula��o s� contem um pai
					//aplica crossover nesse pai
					#pragma omp critical
					crossOver(population[0].first, population[0].first);

					if(real_size_population > size_population) {
						#pragma omp critical
						{
						population.pop_back();  //remove o pior pai
						real_size_population--;
						}
					}
				}
			}

		}

} //fim da paraleliza��o




*/






// 16 THREADS

/*

#pragma omp parallel num_threads(3)
{

	int tid = omp_get_thread_num();

		if (tid == 0) {

			for(int i = 0; i < generations/16; i++) {
				int  old_size_population = real_size_population;

				// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

				if(real_size_population >= 2) {
					if(real_size_population == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(population[0].first, population[1].first);
					}
					else {

						// real_size_population > 2

						int parent1, parent2;

						do {

							//seleciona dois pais randomicamente

							parent1 = rand() % real_size_population;
							parent2 = rand() % real_size_population;
						}while(parent1 == parent2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(population[parent1].first, population[parent2].first);
					}

					//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

					int diff_population = real_size_population - old_size_population;

					if(diff_population == 2) {
						if(real_size_population > size_population) {

							//remove os dois piores pais da popula��o

							#pragma omp critical
							{
								population.pop_back();
								population.pop_back();

							//decrementa apos remover os pais

							real_size_population -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(real_size_population > size_population) {
							#pragma omp critical
							{
							population.pop_back(); //remove o pior pai
							real_size_population--;
							}
						}
					}
				}
				else {

					//popula��o s� contem um pai
					//aplica crossover nesse pai
					#pragma omp critical
					crossOver(population[0].first, population[0].first);

					if(real_size_population > size_population) {
						#pragma omp critical
						{
						population.pop_back();  //remove o pior pai
						real_size_population--;
						}
					}
				}
			}

		}


		if (tid == 1) {

			for(int i = generations/16; i < 2*(generations/16); i++) {
				int  old_size_population = real_size_population;

				// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

				if(real_size_population >= 2) {
					if(real_size_population == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(population[0].first, population[1].first);
					}
					else {

						// real_size_population > 2

						int parent1, parent2;

						do {

							//seleciona dois pais randomicamente

							parent1 = rand() % real_size_population;
							parent2 = rand() % real_size_population;
						}while(parent1 == parent2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(population[parent1].first, population[parent2].first);
					}

					//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

					int diff_population = real_size_population - old_size_population;

					if(diff_population == 2) {
						if(real_size_population > size_population) {

							//remove os dois piores pais da popula��o

							#pragma omp critical
							{
								population.pop_back();
								population.pop_back();

							//decrementa apos remover os pais

							real_size_population -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(real_size_population > size_population) {
							#pragma omp critical
							{
							population.pop_back(); //remove o pior pai
							real_size_population--;
							}
						}
					}
				}
				else {

					//popula��o s� contem um pai
					//aplica crossover nesse pai
					#pragma omp critical
					crossOver(population[0].first, population[0].first);

					if(real_size_population > size_population) {
						#pragma omp critical
						{
						population.pop_back();  //remove o pior pai
						real_size_population--;
						}
					}
				}
			}

		}


		if (tid == 2) {

			for(int i = 2*(generations/16); i < 3*(generations/16); i++) {
				int  old_size_population = real_size_population;

				// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

				if(real_size_population >= 2) {
					if(real_size_population == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(population[0].first, population[1].first);
					}
					else {

						// real_size_population > 2

						int parent1, parent2;

						do {

							//seleciona dois pais randomicamente

							parent1 = rand() % real_size_population;
							parent2 = rand() % real_size_population;
						}while(parent1 == parent2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(population[parent1].first, population[parent2].first);
					}

					//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

					int diff_population = real_size_population - old_size_population;

					if(diff_population == 2) {
						if(real_size_population > size_population) {

							//remove os dois piores pais da popula��o

							#pragma omp critical
							{
								population.pop_back();
								population.pop_back();

							//decrementa apos remover os pais

							real_size_population -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(real_size_population > size_population) {
							#pragma omp critical
							{
							population.pop_back(); //remove o pior pai
							real_size_population--;
							}
						}
					}
				}
				else {

					//popula��o s� contem um pai
					//aplica crossover nesse pai
					#pragma omp critical
					crossOver(population[0].first, population[0].first);

					if(real_size_population > size_population) {
						#pragma omp critical
						{
						population.pop_back();  //remove o pior pai
						real_size_population--;
						}
					}
				}
			}

		}


		if (tid == 3) {

			for(int i = 3*(generations/16); i < 4*(generations/16); i++) {
				int  old_size_population = real_size_population;

				// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

				if(real_size_population >= 2) {
					if(real_size_population == 2) {

						//aplica crossover nos pais
						#pragma omp critical
						crossOver(population[0].first, population[1].first);
					}
					else {

						// real_size_population > 2

						int parent1, parent2;

						do {

							//seleciona dois pais randomicamente

							parent1 = rand() % real_size_population;
							parent2 = rand() % real_size_population;
						}while(parent1 == parent2);

						//aplica crossover nos dois pais
						#pragma omp critical
						crossOver(population[parent1].first, population[parent2].first);
					}

					//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

					int diff_population = real_size_population - old_size_population;

					if(diff_population == 2) {
						if(real_size_population > size_population) {

							//remove os dois piores pais da popula��o

							#pragma omp critical
							{
								population.pop_back();
								population.pop_back();

							//decrementa apos remover os pais

							real_size_population -= 2;

							}
						}
					}
					else if(diff_population == 1) {
						if(real_size_population > size_population) {
							#pragma omp critical
							{
							population.pop_back(); //remove o pior pai
							real_size_population--;
							}
						}
					}
				}
				else {

					//popula��o s� contem um pai
					//aplica crossover nesse pai
					#pragma omp critical
					crossOver(population[0].first, population[0].first);

					if(real_size_population > size_population) {
						#pragma omp critical
						{
						population.pop_back();  //remove o pior pai
						real_size_population--;
						}
					}
				}
			}

		}

		if (tid == 4) {

				for(int i = 4*(generations/16); i < 5*(generations/16); i++) {
					int  old_size_population = real_size_population;

					// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

					if(real_size_population >= 2) {
						if(real_size_population == 2) {

							//aplica crossover nos pais
							#pragma omp critical
							crossOver(population[0].first, population[1].first);
						}
						else {

							// real_size_population > 2

							int parent1, parent2;

							do {

								//seleciona dois pais randomicamente

								parent1 = rand() % real_size_population;
								parent2 = rand() % real_size_population;
							}while(parent1 == parent2);

							//aplica crossover nos dois pais
							#pragma omp critical
							crossOver(population[parent1].first, population[parent2].first);
						}

						//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

						int diff_population = real_size_population - old_size_population;

						if(diff_population == 2) {
							if(real_size_population > size_population) {

								//remove os dois piores pais da popula��o

								#pragma omp critical
								{
									population.pop_back();
									population.pop_back();

								//decrementa apos remover os pais

								real_size_population -= 2;

								}
							}
						}
						else if(diff_population == 1) {
							if(real_size_population > size_population) {
								#pragma omp critical
								{
								population.pop_back(); //remove o pior pai
								real_size_population--;
								}
							}
						}
					}
					else {

						//popula��o s� contem um pai
						//aplica crossover nesse pai
						#pragma omp critical
						crossOver(population[0].first, population[0].first);

						if(real_size_population > size_population) {
							#pragma omp critical
							{
							population.pop_back();  //remove o pior pai
							real_size_population--;
							}
						}
					}
				}

			}

		if (tid == 5) {

				for(int i = 5*(generations/16); i < 6*(generations/16); i++) {
					int  old_size_population = real_size_population;

					// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

					if(real_size_population >= 2) {
						if(real_size_population == 2) {

							//aplica crossover nos pais
							#pragma omp critical
							crossOver(population[0].first, population[1].first);
						}
						else {

							// real_size_population > 2

							int parent1, parent2;

							do {

								//seleciona dois pais randomicamente

								parent1 = rand() % real_size_population;
								parent2 = rand() % real_size_population;
							}while(parent1 == parent2);

							//aplica crossover nos dois pais
							#pragma omp critical
							crossOver(population[parent1].first, population[parent2].first);
						}

						//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

						int diff_population = real_size_population - old_size_population;

						if(diff_population == 2) {
							if(real_size_population > size_population) {

								//remove os dois piores pais da popula��o

								#pragma omp critical
								{
									population.pop_back();
									population.pop_back();

								//decrementa apos remover os pais

								real_size_population -= 2;

								}
							}
						}
						else if(diff_population == 1) {
							if(real_size_population > size_population) {
								#pragma omp critical
								{
								population.pop_back(); //remove o pior pai
								real_size_population--;
								}
							}
						}
					}
					else {

						//popula��o s� contem um pai
						//aplica crossover nesse pai
						#pragma omp critical
						crossOver(population[0].first, population[0].first);

						if(real_size_population > size_population) {
							#pragma omp critical
							{
							population.pop_back();  //remove o pior pai
							real_size_population--;
							}
						}
					}
				}

			}
		if (tid == 6) {

				for(int i = 6*(generations/16); i < 7*(generations/16); i++) {
					int  old_size_population = real_size_population;

					// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

					if(real_size_population >= 2) {
						if(real_size_population == 2) {

							//aplica crossover nos pais
							#pragma omp critical
							crossOver(population[0].first, population[1].first);
						}
						else {

							// real_size_population > 2

							int parent1, parent2;

							do {

								//seleciona dois pais randomicamente

								parent1 = rand() % real_size_population;
								parent2 = rand() % real_size_population;
							}while(parent1 == parent2);

							//aplica crossover nos dois pais
							#pragma omp critical
							crossOver(population[parent1].first, population[parent2].first);
						}

						//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

						int diff_population = real_size_population - old_size_population;

						if(diff_population == 2) {
							if(real_size_population > size_population) {

								//remove os dois piores pais da popula��o

								#pragma omp critical
								{
									population.pop_back();
									population.pop_back();

								//decrementa apos remover os pais

								real_size_population -= 2;

								}
							}
						}
						else if(diff_population == 1) {
							if(real_size_population > size_population) {
								#pragma omp critical
								{
								population.pop_back(); //remove o pior pai
								real_size_population--;
								}
							}
						}
					}
					else {

						//popula��o s� contem um pai
						//aplica crossover nesse pai
						#pragma omp critical
						crossOver(population[0].first, population[0].first);

						if(real_size_population > size_population) {
							#pragma omp critical
							{
							population.pop_back();  //remove o pior pai
							real_size_population--;
							}
						}
					}
				}

			}
		if (tid == 7) {

				for(int i = 7*(generations/16); i < 8*(generations/16); i++) {
					int  old_size_population = real_size_population;

					// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

					if(real_size_population >= 2) {
						if(real_size_population == 2) {

							//aplica crossover nos pais
							#pragma omp critical
							crossOver(population[0].first, population[1].first);
						}
						else {

							// real_size_population > 2

							int parent1, parent2;

							do {

								//seleciona dois pais randomicamente

								parent1 = rand() % real_size_population;
								parent2 = rand() % real_size_population;
							}while(parent1 == parent2);

							//aplica crossover nos dois pais
							#pragma omp critical
							crossOver(population[parent1].first, population[parent2].first);
						}

						//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

						int diff_population = real_size_population - old_size_population;

						if(diff_population == 2) {
							if(real_size_population > size_population) {

								//remove os dois piores pais da popula��o

								#pragma omp critical
								{
									population.pop_back();
									population.pop_back();

								//decrementa apos remover os pais

								real_size_population -= 2;

								}
							}
						}
						else if(diff_population == 1) {
							if(real_size_population > size_population) {
								#pragma omp critical
								{
								population.pop_back(); //remove o pior pai
								real_size_population--;
								}
							}
						}
					}
					else {

						//popula��o s� contem um pai
						//aplica crossover nesse pai
						#pragma omp critical
						crossOver(population[0].first, population[0].first);

						if(real_size_population > size_population) {
							#pragma omp critical
							{
							population.pop_back();  //remove o pior pai
							real_size_population--;
							}
						}
					}
				}

			}

		if (tid == 8) {

						for(int i = 8*(generations/16); i < 9*(generations/16); i++) {
							int  old_size_population = real_size_population;

							// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

							if(real_size_population >= 2) {
								if(real_size_population == 2) {

									//aplica crossover nos pais
									#pragma omp critical
									crossOver(population[0].first, population[1].first);
								}
								else {

									// real_size_population > 2

									int parent1, parent2;

									do {

										//seleciona dois pais randomicamente

										parent1 = rand() % real_size_population;
										parent2 = rand() % real_size_population;
									}while(parent1 == parent2);

									//aplica crossover nos dois pais
									#pragma omp critical
									crossOver(population[parent1].first, population[parent2].first);
								}

								//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

								int diff_population = real_size_population - old_size_population;

								if(diff_population == 2) {
									if(real_size_population > size_population) {

										//remove os dois piores pais da popula��o

										#pragma omp critical
										{
											population.pop_back();
											population.pop_back();

										//decrementa apos remover os pais

										real_size_population -= 2;

										}
									}
								}
								else if(diff_population == 1) {
									if(real_size_population > size_population) {
										#pragma omp critical
										{
										population.pop_back(); //remove o pior pai
										real_size_population--;
										}
									}
								}
							}
							else {

								//popula��o s� contem um pai
								//aplica crossover nesse pai
								#pragma omp critical
								crossOver(population[0].first, population[0].first);

								if(real_size_population > size_population) {
									#pragma omp critical
									{
									population.pop_back();  //remove o pior pai
									real_size_population--;
									}
								}
							}
						}

					}
		if (tid == 9) {

						for(int i = 9*(generations/16); i < 10*(generations/16); i++) {
							int  old_size_population = real_size_population;

							// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

							if(real_size_population >= 2) {
								if(real_size_population == 2) {

									//aplica crossover nos pais
									#pragma omp critical
									crossOver(population[0].first, population[1].first);
								}
								else {

									// real_size_population > 2

									int parent1, parent2;

									do {

										//seleciona dois pais randomicamente

										parent1 = rand() % real_size_population;
										parent2 = rand() % real_size_population;
									}while(parent1 == parent2);

									//aplica crossover nos dois pais
									#pragma omp critical
									crossOver(population[parent1].first, population[parent2].first);
								}

								//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

								int diff_population = real_size_population - old_size_population;

								if(diff_population == 2) {
									if(real_size_population > size_population) {

										//remove os dois piores pais da popula��o

										#pragma omp critical
										{
											population.pop_back();
											population.pop_back();

										//decrementa apos remover os pais

										real_size_population -= 2;

										}
									}
								}
								else if(diff_population == 1) {
									if(real_size_population > size_population) {
										#pragma omp critical
										{
										population.pop_back(); //remove o pior pai
										real_size_population--;
										}
									}
								}
							}
							else {

								//popula��o s� contem um pai
								//aplica crossover nesse pai
								#pragma omp critical
								crossOver(population[0].first, population[0].first);

								if(real_size_population > size_population) {
									#pragma omp critical
									{
									population.pop_back();  //remove o pior pai
									real_size_population--;
									}
								}
							}
						}

					}
		if (tid == 10) {

						for(int i = 10*(generations/16); i < 11*(generations/16); i++) {
							int  old_size_population = real_size_population;

							// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

							if(real_size_population >= 2) {
								if(real_size_population == 2) {

									//aplica crossover nos pais
									#pragma omp critical
									crossOver(population[0].first, population[1].first);
								}
								else {

									// real_size_population > 2

									int parent1, parent2;

									do {

										//seleciona dois pais randomicamente

										parent1 = rand() % real_size_population;
										parent2 = rand() % real_size_population;
									}while(parent1 == parent2);

									//aplica crossover nos dois pais
									#pragma omp critical
									crossOver(population[parent1].first, population[parent2].first);
								}

								//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

								int diff_population = real_size_population - old_size_population;

								if(diff_population == 2) {
									if(real_size_population > size_population) {

										//remove os dois piores pais da popula��o

										#pragma omp critical
										{
											population.pop_back();
											population.pop_back();

										//decrementa apos remover os pais

										real_size_population -= 2;

										}
									}
								}
								else if(diff_population == 1) {
									if(real_size_population > size_population) {
										#pragma omp critical
										{
										population.pop_back(); //remove o pior pai
										real_size_population--;
										}
									}
								}
							}
							else {

								//popula��o s� contem um pai
								//aplica crossover nesse pai
								#pragma omp critical
								crossOver(population[0].first, population[0].first);

								if(real_size_population > size_population) {
									#pragma omp critical
									{
									population.pop_back();  //remove o pior pai
									real_size_population--;
									}
								}
							}
						}

					}

		if (tid == 11) {

						for(int i = 11*(generations/16); i < 12*(generations/16); i++) {
							int  old_size_population = real_size_population;

							// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

							if(real_size_population >= 2) {
								if(real_size_population == 2) {

									//aplica crossover nos pais
									#pragma omp critical
									crossOver(population[0].first, population[1].first);
								}
								else {

									// real_size_population > 2

									int parent1, parent2;

									do {

										//seleciona dois pais randomicamente

										parent1 = rand() % real_size_population;
										parent2 = rand() % real_size_population;
									}while(parent1 == parent2);

									//aplica crossover nos dois pais
									#pragma omp critical
									crossOver(population[parent1].first, population[parent2].first);
								}

								//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

								int diff_population = real_size_population - old_size_population;

								if(diff_population == 2) {
									if(real_size_population > size_population) {

										//remove os dois piores pais da popula��o

										#pragma omp critical
										{
											population.pop_back();
											population.pop_back();

										//decrementa apos remover os pais

										real_size_population -= 2;

										}
									}
								}
								else if(diff_population == 1) {
									if(real_size_population > size_population) {
										#pragma omp critical
										{
										population.pop_back(); //remove o pior pai
										real_size_population--;
										}
									}
								}
							}
							else {

								//popula��o s� contem um pai
								//aplica crossover nesse pai
								#pragma omp critical
								crossOver(population[0].first, population[0].first);

								if(real_size_population > size_population) {
									#pragma omp critical
									{
									population.pop_back();  //remove o pior pai
									real_size_population--;
									}
								}
							}
						}

					}
		if (tid == 12) {

						for(int i = 12*(generations/16); i < 13*(generations/16); i++) {
							int  old_size_population = real_size_population;

							// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

							if(real_size_population >= 2) {
								if(real_size_population == 2) {

									//aplica crossover nos pais
									#pragma omp critical
									crossOver(population[0].first, population[1].first);
								}
								else {

									// real_size_population > 2

									int parent1, parent2;

									do {

										//seleciona dois pais randomicamente

										parent1 = rand() % real_size_population;
										parent2 = rand() % real_size_population;
									}while(parent1 == parent2);

									//aplica crossover nos dois pais
									#pragma omp critical
									crossOver(population[parent1].first, population[parent2].first);
								}

								//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

								int diff_population = real_size_population - old_size_population;

								if(diff_population == 2) {
									if(real_size_population > size_population) {

										//remove os dois piores pais da popula��o

										#pragma omp critical
										{
											population.pop_back();
											population.pop_back();

										//decrementa apos remover os pais

										real_size_population -= 2;

										}
									}
								}
								else if(diff_population == 1) {
									if(real_size_population > size_population) {
										#pragma omp critical
										{
										population.pop_back(); //remove o pior pai
										real_size_population--;
										}
									}
								}
							}
							else {

								//popula��o s� contem um pai
								//aplica crossover nesse pai
								#pragma omp critical
								crossOver(population[0].first, population[0].first);

								if(real_size_population > size_population) {
									#pragma omp critical
									{
									population.pop_back();  //remove o pior pai
									real_size_population--;
									}
								}
							}
						}

					}
		if (tid == 14) {

						for(int i = 14*(generations/16); i < 15*(generations/16); i++) {
							int  old_size_population = real_size_population;

							// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

							if(real_size_population >= 2) {
								if(real_size_population == 2) {

									//aplica crossover nos pais
									#pragma omp critical
									crossOver(population[0].first, population[1].first);
								}
								else {

									// real_size_population > 2

									int parent1, parent2;

									do {

										//seleciona dois pais randomicamente

										parent1 = rand() % real_size_population;
										parent2 = rand() % real_size_population;
									}while(parent1 == parent2);

									//aplica crossover nos dois pais
									#pragma omp critical
									crossOver(population[parent1].first, population[parent2].first);
								}

								//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

								int diff_population = real_size_population - old_size_population;

								if(diff_population == 2) {
									if(real_size_population > size_population) {

										//remove os dois piores pais da popula��o

										#pragma omp critical
										{
											population.pop_back();
											population.pop_back();

										//decrementa apos remover os pais

										real_size_population -= 2;

										}
									}
								}
								else if(diff_population == 1) {
									if(real_size_population > size_population) {
										#pragma omp critical
										{
										population.pop_back(); //remove o pior pai
										real_size_population--;
										}
									}
								}
							}
							else {

								//popula��o s� contem um pai
								//aplica crossover nesse pai
								#pragma omp critical
								crossOver(population[0].first, population[0].first);

								if(real_size_population > size_population) {
									#pragma omp critical
									{
									population.pop_back();  //remove o pior pai
									real_size_population--;
									}
								}
							}
						}

					}
		if (tid == 15) {

						for(int i = 15*(generations/16); i < generations; i++) {
							int  old_size_population = real_size_population;

							// seleciona dois pais (se existe) que participar�o do processo de reprodu��o

							if(real_size_population >= 2) {
								if(real_size_population == 2) {

									//aplica crossover nos pais
									#pragma omp critical
									crossOver(population[0].first, population[1].first);
								}
								else {

									// real_size_population > 2

									int parent1, parent2;

									do {

										//seleciona dois pais randomicamente

										parent1 = rand() % real_size_population;
										parent2 = rand() % real_size_population;
									}while(parent1 == parent2);

									//aplica crossover nos dois pais
									#pragma omp critical
									crossOver(population[parent1].first, population[parent2].first);
								}

								//recupera a diferen�a entre os tamanhos de popula��o antigo e atual pra ver se a mesma cresceu

								int diff_population = real_size_population - old_size_population;

								if(diff_population == 2) {
									if(real_size_population > size_population) {

										//remove os dois piores pais da popula��o

										#pragma omp critical
										{
											population.pop_back();
											population.pop_back();

										//decrementa apos remover os pais

										real_size_population -= 2;

										}
									}
								}
								else if(diff_population == 1) {
									if(real_size_population > size_population) {
										#pragma omp critical
										{
										population.pop_back(); //remove o pior pai
										real_size_population--;
										}
									}
								}
							}
							else {

								//popula��o s� contem um pai
								//aplica crossover nesse pai
								#pragma omp critical
								crossOver(population[0].first, population[0].first);

								if(real_size_population > size_population) {
									#pragma omp critical
									{
									population.pop_back();  //remove o pior pai
									real_size_population--;
									}
								}
							}
						}

					}

	} //fim da paraleliza��o



*/






	if(show_population == true)
		showPopulation(); /*mostra a popula��o*/

	cout << "\nMelhor solucao: ";
	const vector<int>& vec = population[0].first;
	for(int i = 0; i < graph->V; i++)
		cout << vec[i] << " ";
	cout << graph->initial_vertex;
	cout << " | Custo: " << population[0].second;
}


int Genetic::getCostBestSolution()
{
	if(real_size_population > 0)
		return population[0].second;
	return -1;
}
