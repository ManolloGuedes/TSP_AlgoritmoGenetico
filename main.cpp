#include <iostream>
#include <omp.h>
#include "tsp.h"
using namespace std;

int main() {
	srand(time(NULL)); // random numbers

	/*cria o graph1 com parâmetros: numero de vertices e vertice inicial (não faço ideia do que se trata kkk)*/
	Graph * graph1 = new Graph(5, 0);
	/*adiciona as arestas*/
	graph1->addEdge(0, 1, 1);
	graph1->addEdge(1, 0, 1);
	graph1->addEdge(0, 2, 3);
	graph1->addEdge(2, 0, 3);
	graph1->addEdge(0, 3, 4);
	graph1->addEdge(3, 0, 4);
	graph1->addEdge(0, 4, 5);
	graph1->addEdge(4, 0, 5);
	graph1->addEdge(1, 2, 1);
	graph1->addEdge(2, 1, 1);
	graph1->addEdge(1, 3, 4);
	graph1->addEdge(3, 1, 4);
	graph1->addEdge(1, 4, 8);
	graph1->addEdge(4, 1, 8);
	graph1->addEdge(2, 3, 5);
	graph1->addEdge(3, 2, 5);
	graph1->addEdge(2, 4, 1);
	graph1->addEdge(4, 2, 1);
	graph1->addEdge(3, 4, 2);
	graph1->addEdge(4, 3, 2);

	/*cria o graph2*/
	Graph * graph2 = new Graph(4, 0);
	/*adiciona as arestas*/
	graph2->addEdge(0, 1, 2);
	graph2->addEdge(1, 0, 2);
	graph2->addEdge(0, 2, 6);
	graph2->addEdge(2, 0, 6);
	graph2->addEdge(0, 3, 3);
	graph2->addEdge(3, 0, 3);
	graph2->addEdge(1, 3, 7);
	graph2->addEdge(3, 1, 7);
	graph2->addEdge(1, 2, 4);
	graph2->addEdge(2, 1, 4);
	graph2->addEdge(2, 3, 2);
	graph2->addEdge(3, 2, 2);

	/*cria o graph3*/
	Graph * graph3 = new Graph(4, 0);
	/*adiciona as arestas*/
	graph3->addEdge(0, 1, 10);
	graph3->addEdge(1, 0, 10);
	graph3->addEdge(1, 3, 20);
	graph3->addEdge(3, 1, 20);
	graph3->addEdge(0, 2, 15);
	graph3->addEdge(2, 0, 15);
	graph3->addEdge(1, 3, 25);
	graph3->addEdge(3, 1, 25);
	graph3->addEdge(2, 3, 30);
	graph3->addEdge(3, 2, 30);
	graph3->addEdge(1, 2, 35);
	graph3->addEdge(2, 1, 35);

	/*cria um grafo randomico. Parametro true é para a geração do grafo*/
	Graph * graph4 = new Graph(10, 0, true);
	graph4->showInfoGraph();

	/*parametros: grafo, tamanho da população, número de gerações e mutações*/
	/*Parâmetro opcional: mostrar ou não a população*/
	Genetic genetic(graph4, 10, 1000000, 5, true);

	const clock_t begin_time = clock(); /*recupera o tempo*/


	genetic.run(); /*roda o algoritmo genético*/


	cout << "\n\nTempo para executar o algoritmo genético: " << float(clock () - begin_time) /  CLOCKS_PER_SEC << " segundos."; /*mostra o tempo em segundos*/

	return 0;
}
