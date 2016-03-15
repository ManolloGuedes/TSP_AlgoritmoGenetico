#include <iostream>
#include <mpi.h>
#include "tsp.h"
using namespace std;

int main(int argc, char **argv) {

    Grafo * grafo4;
    int rank, size, flag;

    MPI_Init(&argc,&argv);
    MPI_Comm_size (MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand((unsigned)time(NULL)+rank+1*size); //funcao que inicia os numeros aleatorios

    /*cria um grafo randomico. Parametro true é para a geração do grafo*/
    grafo4 = new Grafo(50, 0, true);

    if(rank == 0) {
        grafo4->infoGrafo();
    }

	/*parametros: grafo, tamanho da população, número de gerações e mutações*/
	/*Parâmetro opcional: mostrar ou não a população*/
	Genetico genetico(grafo4, 10, 1000, 8/size, true);

	const clock_t begin_time = clock(); /*recupera o tempo*/
    MPI_Status status;

	genetico.executar(rank, size); /*roda o algoritmo genético*/

    MPI_Finalize();

    if(rank == 0) {
        cout << "\n\nTempo para executar o algoritmo genético: " << float(clock () - begin_time) /  CLOCKS_PER_SEC << " segundos." << endl; /*mostra o tempo em segundos*/
        cout << endl;
    }

	return 0;
}
