#include <mpi.h>
#include <iostream>

using namespace std;

constexpr int RANK_MASTER = 0;      // Rango del proceso maestro
constexpr int RANK_WORKER = 1;      // Rango del proceso trabajador
constexpr int TAG = 0;              // Etiqueta para los mensajes
constexpr int NUM_DATOS = 1;        // Datos a enviar/recibir

/*
 * Calcula el factorial de un número entero.
 * @param n Número entero.
 * @return Factorial de n.
 */
long double calculateFactorial(int n);

/*
 * Programa principal.
 * @param argc Cantidad de argumentos.
 * @param argv Argumentos.
 * @return Código de salida.
*/
int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        printf("ERROR: No hay procesos suficientes para ejecutar el programa. (%d < 2)", size);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int number = 0;
    long double result = 0;
    MPI_Request request;
    MPI_Status status;

    if (rank == RANK_MASTER) {
        while (true) {
            printf("Introduce un número (-1 para salir): ");
            while (!(cin >> number)) {
                cin.clear(); // Limpiar el estado de error
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Descartar la entrada inválida
                printf("Entrada inválida. Por favor, introduce un número (-1 para salir): ");
            }

            // Enviar el número al proceso trabajador de manera no bloqueante
            MPI_Isend(&number, NUM_DATOS, MPI_INT, RANK_WORKER, TAG, MPI_COMM_WORLD, &request);

            if (number == -1) {
                break; // Condición de salida
            }

            printf("Esperando resultado...\n");

            // Recibir el resultado del proceso trabajador de manera no bloqueante
            MPI_Irecv(&result, NUM_DATOS, MPI_LONG_DOUBLE, RANK_WORKER, TAG, MPI_COMM_WORLD, &request);

            // Esperar a que la recepción se complete
            MPI_Wait(&request, &status);

			printf("%d! = %.0lf\n", number, result);
        }
    }
    else if (rank == RANK_WORKER) {
        while (true) {
            // Recibir el número del proceso maestro de manera no bloqueante
            MPI_Irecv(&number, NUM_DATOS, MPI_INT, RANK_MASTER, TAG, MPI_COMM_WORLD, &request);

            // Esperar a que la recepción se complete
            MPI_Wait(&request, &status);

            if (number == -1) {
                break; // Condición de salida
            }

            // Calcular el factorial
            result = calculateFactorial(number);

            // Enviar el resultado al proceso maestro de manera no bloqueante
            MPI_Isend(&result, NUM_DATOS, MPI_LONG_DOUBLE, RANK_MASTER, TAG, MPI_COMM_WORLD, &request);

            // Esperar a que el envío se complete
            MPI_Wait(&request, &status);
        }
    }

    MPI_Finalize();
    return 0;
}

long double calculateFactorial(int n) {
    long double factorial = 1;
    for (int i = 1; i <= n; i++) {
        factorial *= i;
    }
    return factorial;
}