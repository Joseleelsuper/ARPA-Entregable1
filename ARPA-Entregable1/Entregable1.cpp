#include <mpi.h>
#include <iostream>

using namespace std;

constexpr int RANK_MASTER = 0;      // Rango del proceso maestro
constexpr int RANK_WORKER = 1;      // Rango del proceso trabajador
constexpr int TAG = 0;              // Etiqueta para los mensajes

/*
    * Calcula el factorial de un n�mero entero.
    * @param n N�mero entero.
    * @return Factorial de n.
 */
long int calculateFactorial(int n);

/*
	* Programa principal.
	* @param argc Cantidad de argumentos.
	* @param argv Argumentos.
	* @return C�digo de salida.
*/
int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int number;
    long int result;
    MPI_Request request;
    MPI_Status status;

    if (rank == RANK_MASTER) {
        while (true) {
            printf("Introduce un n�mero (-1 para salir): ");
            cin >> number;

            // Enviar el n�mero al proceso trabajador de manera no bloqueante
            MPI_Isend(&number, 1, MPI_INT, RANK_WORKER, TAG, MPI_COMM_WORLD, &request);

            if (number == -1) {
                break; // Condici�n de salida
            }

            printf("Esperando resultado...\n");

            // Recibir el resultado del proceso trabajador de manera no bloqueante
            MPI_Irecv(&result, 1, MPI_LONG, RANK_WORKER, TAG, MPI_COMM_WORLD, &request);

            // Esperar a que la recepci�n se complete
            MPI_Wait(&request, &status);

			printf("%d! = %ld\n\n", number, result);
        }
    }
    else if (rank == RANK_WORKER) {
        while (true) {
            // Recibir el n�mero del proceso maestro de manera no bloqueante
            MPI_Irecv(&number, 1, MPI_INT, RANK_MASTER, TAG, MPI_COMM_WORLD, &request);

            // Esperar a que la recepci�n se complete
            MPI_Wait(&request, &status);

            if (number == -1) {
                break; // Condici�n de salida
            }

            // Calcular el factorial
            result = calculateFactorial(number);

            // Enviar el resultado al proceso maestro de manera no bloqueante
            MPI_Isend(&result, 1, MPI_LONG, RANK_MASTER, TAG, MPI_COMM_WORLD, &request);

            // Esperar a que el env�o se complete
            MPI_Wait(&request, &status);
        }
    }

    MPI_Finalize();
    return 0;
}

long int calculateFactorial(int n) {
    long int factorial = 1;
    for (int i = 1; i <= n; i++) {
        factorial *= i;
    }
    return factorial;
}
