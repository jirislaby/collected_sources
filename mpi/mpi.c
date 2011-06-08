#include <stdio.h>
#include <unistd.h>

#include <mpi.h>

int main(int argc, char **argv)
{
	int size, rank;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	printf("size: %d/%d\n", rank, size);
	sleep(20);

	MPI_Finalize();

	return 0;
}
