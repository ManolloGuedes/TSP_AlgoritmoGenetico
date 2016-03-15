#ifndef TSP_H
#define TSP_H

#include <vector>
#include <map>
#include <set>
#include <utility> // pair
#include <time.h> // time
#include <stdlib.h> // srand, rand


/*classe que descreve o grafo*/
class Grafo {
	private:
		int numVertices; /*número de vertices*/
		int totalArestas; /*total de arestas*/
		int verticeInicial; /*vértice inicial*/
		std::map<std::pair<int, int>, int> mapaArestas; /*mapa de arestas*/
	public:
		Grafo(int V, int initial_vertex, bool grafoRandomico = false); /*construtor*/
		void addAresta(int v1, int v2, int peso); /*adiciona uma aresta*/
		void desenhaGrafo(); /*mostra as ligações do grafo*/
		void gerarGrafo(); /*gera um grafo randômico*/
		void infoGrafo(); /*mostra informações sobre o grafo*/
		int existeAresta(int origem, int destino); /*verifica a existência de uma aresta*/
		friend class Genetic; /*acessa os membros privados da classe*/
};

typedef std::pair<std::vector<int>, int> parIndividuo;


/*Ordena o vetor de pares*/
struct ordenaPar {
	bool operator()(const parIndividuo& elemento1, const parIndividuo& elemento2) {
		return elemento1.second < elemento2.second;
	}
};

/*classe que descreve o algoritmo genético*/
class Genetic {
	private:
		Grafo* graph; /*grafo*/
		std::vector< parIndividuo > populacao; /*cada elemento é um par: vetor e custo total*/
		int tamPopulacao; /*tamanho da população*/
		int tamRealPopulacao; /*tamanho real da população*/
		int qtdGeracoes; /*quantidade de gerações*/
		int taxaMutacao; /*taxa de mutação*/
		bool mostrar_Populacao; /*flag para mostrar população*/
	private:
		void gerarPopInicial(); /*gera a população inicial*/
	public:
		Genetic(Grafo* graph, int quantidadePopulacao, int generations, int mutation_rate, bool show_population = true); /*construtor*/
		int solucaoValida(std::vector<int>& solucao); /*verifica se a solução é válida*/
		void mostrarPopulacao(); /*mostra a população*/
		void crossOver(std::vector<int>& pai1, std::vector<int>& pai2); /*realiza o crossover*/
		void insercaoBinaria(std::vector<int>& filho, int custoTotal); /*usa busca binária para inserir*/
		void executar(); /*executa o algoritmo genético*/
		int getMelhorCusto(); /*retorna o custo da melhor solução*/
		bool existeCromossomo(const std::vector<int> & cromossomo); /*checa se existe o cromossomo*/
};

#endif
