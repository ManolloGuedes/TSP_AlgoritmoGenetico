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
		int V; /*número de vertices*/
		int total_edges; /*total de arestas*/
		int initial_vertex; /*vértice inicial*/
		std::map<std::pair<int, int>, int> map_edges; /*mapa de arestas*/
	public:
		Graph(int V, int initial_vertex, bool random_graph = false); /*construtor*/
		void addEdge(int v1, int v2, int weight); /*adiciona uma aresta*/
		void showGraph(); /*mostra as ligações do grafo*/
		void generatesGraph(); /*gera um grafo randômico*/
		void showInfoGraph(); /*mostra informações sobre o grafo*/
		int existsEdge(int src, int dest); /*verifica a existência de uma aresta*/
		friend class Genetic; /*acessa os membros privados da classe*/
};

typedef std::pair<std::vector<int>, int> my_pair;


/*Ordena o vetor de pares*/
struct sort_pred {
	bool operator()(const my_pair& firstElem, const my_pair& secondElem) {
		return firstElem.second < secondElem.second;
	}
};

/*classe que descreve o algoritmo genético*/
class Genetic {
	private:
		Graph* graph; /*grafo*/
		std::vector< my_pair > population; /*cada elemento é um par: vetor e custo total*/
		int size_population; /*tamanho da população*/
		int real_size_population; /*tamanho real da população*/
		int generations; /*quantidade de gerações*/
		int mutation_rate; /*taxa de mutação*/
		bool show_population; /*flag para mostrar população*/
	private:
		void initialPopulation(); /*gera a população inicial*/
	public:
		Genetic(Graph* graph, int amount_population, int generations, int mutation_rate, bool show_population = true); /*construtor*/
		int isValidSolution(std::vector<int>& solution); /*verifica se a solução é válida*/
		void showPopulation(); /*mostra a população*/
		void crossOver(std::vector<int>& parent1, std::vector<int>& parent2); /*realiza o crossover*/
		void insertBinarySearch(std::vector<int>& child, int total_cost); /*usa busca binária para inserir*/
		void run(int rank, int size); /*executa o algoritmo genético*/
		int getCostBestSolution(); /*retorna o custo da melhor solução*/
		bool existsChromosome(const std::vector<int> & v); /*checa se existe o cromossomo*/
};

#endif
