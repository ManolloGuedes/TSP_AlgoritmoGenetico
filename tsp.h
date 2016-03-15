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
		int numVertices; /*n�mero de vertices*/
		int totalArestas; /*total de arestas*/
		int verticeInicial; /*v�rtice inicial*/
		std::map<std::pair<int, int>, int> mapaArestas; /*mapa de arestas*/
	public:
		Grafo(int V, int initial_vertex, bool grafoRandomico = false); /*construtor*/
		void addAresta(int v1, int v2, int peso); /*adiciona uma aresta*/
		void desenhaGrafo(); /*mostra as liga��es do grafo*/
		void gerarGrafo(); /*gera um grafo rand�mico*/
		void infoGrafo(); /*mostra informa��es sobre o grafo*/
		int existeAresta(int origem, int destino); /*verifica a exist�ncia de uma aresta*/
		friend class Genetic; /*acessa os membros privados da classe*/
};

typedef std::pair<std::vector<int>, int> parIndividuo;


/*Ordena o vetor de pares*/
struct ordenaPar {
	bool operator()(const parIndividuo& elemento1, const parIndividuo& elemento2) {
		return elemento1.second < elemento2.second;
	}
};

/*classe que descreve o algoritmo gen�tico*/
class Genetic {
	private:
		Grafo* graph; /*grafo*/
		std::vector< parIndividuo > populacao; /*cada elemento � um par: vetor e custo total*/
		int tamPopulacao; /*tamanho da popula��o*/
		int tamRealPopulacao; /*tamanho real da popula��o*/
		int qtdGeracoes; /*quantidade de gera��es*/
		int taxaMutacao; /*taxa de muta��o*/
		bool mostrar_Populacao; /*flag para mostrar popula��o*/
	private:
		void gerarPopInicial(); /*gera a popula��o inicial*/
	public:
		Genetic(Grafo* graph, int quantidadePopulacao, int generations, int mutation_rate, bool show_population = true); /*construtor*/
		int solucaoValida(std::vector<int>& solucao); /*verifica se a solu��o � v�lida*/
		void mostrarPopulacao(); /*mostra a popula��o*/
		void crossOver(std::vector<int>& pai1, std::vector<int>& pai2); /*realiza o crossover*/
		void insercaoBinaria(std::vector<int>& filho, int custoTotal); /*usa busca bin�ria para inserir*/
		void executar(); /*executa o algoritmo gen�tico*/
		int getMelhorCusto(); /*retorna o custo da melhor solu��o*/
		bool existeCromossomo(const std::vector<int> & cromossomo); /*checa se existe o cromossomo*/
};

#endif
