#include <iostream>
#include <omp.h>
#include "tsp.h"
using namespace std;

int main() {
	srand(time(NULL)); // random numbers

	/*cria o graph1 com parâmetros: numero de vertices e vertice inicial (não faço ideia do que se trata kkk)*/
	Grafo * grafo1 = new Grafo(5, 0);
	/*adiciona as arestas*/
	grafo1->addAresta(0, 1, 1);
	grafo1->addAresta(1, 0, 1);
	grafo1->addAresta(0, 2, 3);
	grafo1->addAresta(2, 0, 3);
	grafo1->addAresta(0, 3, 4);
	grafo1->addAresta(3, 0, 4);
	grafo1->addAresta(0, 4, 5);
	grafo1->addAresta(4, 0, 5);
	grafo1->addAresta(1, 2, 1);
	grafo1->addAresta(2, 1, 1);
	grafo1->addAresta(1, 3, 4);
	grafo1->addAresta(3, 1, 4);
	grafo1->addAresta(1, 4, 8);
	grafo1->addAresta(4, 1, 8);
	grafo1->addAresta(2, 3, 5);
	grafo1->addAresta(3, 2, 5);
	grafo1->addAresta(2, 4, 1);
	grafo1->addAresta(4, 2, 1);
	grafo1->addAresta(3, 4, 2);
	grafo1->addAresta(4, 3, 2);

	/*cria o graph2*/
	Grafo * grafo2 = new Grafo(4, 0);
	/*adiciona as arestas*/
	grafo2->addAresta(0, 1, 2);
	grafo2->addAresta(1, 0, 2);
	grafo2->addAresta(0, 2, 6);
	grafo2->addAresta(2, 0, 6);
	grafo2->addAresta(0, 3, 3);
	grafo2->addAresta(3, 0, 3);
	grafo2->addAresta(1, 3, 7);
	grafo2->addAresta(3, 1, 7);
	grafo2->addAresta(1, 2, 4);
	grafo2->addAresta(2, 1, 4);
	grafo2->addAresta(2, 3, 2);
	grafo2->addAresta(3, 2, 2);

	/*cria o graph3*/
	Grafo * grafo3 = new Grafo(4, 0);
	/*adiciona as arestas*/
	grafo3->addAresta(0, 1, 10);
	grafo3->addAresta(1, 0, 10);
	grafo3->addAresta(1, 3, 20);
	grafo3->addAresta(3, 1, 20);
	grafo3->addAresta(0, 2, 15);
	grafo3->addAresta(2, 0, 15);
	grafo3->addAresta(1, 3, 25);
	grafo3->addAresta(3, 1, 25);
	grafo3->addAresta(2, 3, 30);
	grafo3->addAresta(3, 2, 30);
	grafo3->addAresta(1, 2, 35);
	grafo3->addAresta(2, 1, 35);

	/*cria um grafo randomico. Parametro true é para a geração do grafo*/
	Grafo * grafo4 = new Grafo(10, 0, true);
	grafo4->infoGrafo();

	/*parametros: grafo, tamanho da população, número de gerações e mutações*/
	/*Parâmetro opcional: mostrar ou não a população*/
	Genetic genetico(grafo4, 10, 10000, 5, true);

	const clock_t tempoInicial = clock(); /*recupera o tempo*/


	genetico.executar(); /*roda o algoritmo genético*/


	cout << "\n\nTempo para executar o algoritmo genético: " << float(clock () - tempoInicial) /  CLOCKS_PER_SEC << " segundos."; /*mostra o tempo em segundos*/

	return 0;
}
