#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

int main(int argc, char **argv){
	int rank, size;
	int tag = 0;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int token = 0;

	if( rank == 0 ){
                printf("rank %d holds token %d\n", rank, token);
		token++;
		MPI_Send( &token, 1, MPI_INT, rank+1, tag, MPI_COMM_WORLD );
	}
	else if( rank != size-1 ){
		MPI_Recv(&token, 1, MPI_INT, rank-1, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("rank %d holds token %d\n", rank, token);
		token++;
		MPI_Send( &token, 1, MPI_INT, rank+1, tag, MPI_COMM_WORLD );
	}
	else {
		MPI_Recv(&token, 1, MPI_INT, rank-1, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("rank %d holds token %d\n", rank, token);
	}

	MPI_Finalize();
	exit(0);
}
