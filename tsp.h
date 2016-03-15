#ifndef TSP_H
#define TSP_H

#include <vector>
#include <map>
#include <set>
#include <utility> // pair
#include <time.h> // time
#include <stdlib.h> // srand, rand


/*classe que descreve o grafo*/
class Graph {
	private:
		int V; /*n�mero de vertices*/
		int total_edges; /*total de arestas*/
		int initial_vertex; /*v�rtice inicial*/
		std::map<std::pair<int, int>, int> map_edges; /*mapa de arestas*/
	public:
		Graph(int V, int initial_vertex, bool random_graph = false); /*construtor*/
		void addEdge(int v1, int v2, int weight); /*adiciona uma aresta*/
		void showGraph(); /*mostra as liga��es do grafo*/
		void generatesGraph(); /*gera um grafo rand�mico*/
		void showInfoGraph(); /*mostra informa��es sobre o grafo*/
		int existsEdge(int src, int dest); /*verifica a exist�ncia de uma aresta*/
		friend class Genetic; /*acessa os membros privados da classe*/
};

typedef std::pair<std::vector<int>, int> my_pair;


/*Ordena o vetor de pares*/
struct sort_pred {
	bool operator()(const my_pair& firstElem, const my_pair& secondElem) {
		return firstElem.second < secondElem.second;
	}
};

/*classe que descreve o algoritmo gen�tico*/
class Genetic {
	private:
		Graph* graph; /*grafo*/
		std::vector< my_pair > population; /*cada elemento � um par: vetor e custo total*/
		int size_population; /*tamanho da popula��o*/
		int real_size_population; /*tamanho real da popula��o*/
		int generations; /*quantidade de gera��es*/
		int mutation_rate; /*taxa de muta��o*/
		bool show_population; /*flag para mostrar popula��o*/
	private:
		void initialPopulation(); /*gera a popula��o inicial*/
	public:
		Genetic(Graph* graph, int amount_population, int generations, int mutation_rate, bool show_population = true); /*construtor*/
		int isValidSolution(std::vector<int>& solution); /*verifica se a solu��o � v�lida*/
		void showPopulation(); /*mostra a popula��o*/
		void crossOver(std::vector<int>& parent1, std::vector<int>& parent2); /*realiza o crossover*/
		void insertBinarySearch(std::vector<int>& child, int total_cost); /*usa busca bin�ria para inserir*/
		void run(int rank, int size); /*executa o algoritmo gen�tico*/
		int getCostBestSolution(); /*retorna o custo da melhor solu��o*/
		bool existsChromosome(const std::vector<int> & v); /*checa se existe o cromossomo*/
};

#endif
