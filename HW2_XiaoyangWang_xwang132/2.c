#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

int main(int argc, char **argv){
	int rank, size, rowRank, colRank;

	if(argc < 3){
		printf("Please input a P*Q matrix.\n");
		exit(0);
	}


        MPI_Init(&argc, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);

	int p = atoi(argv[1]);
	int q = atoi(argv[2]);

	if( size < p * q ){
		if( rank != 0 ){
			MPI_Finalize();
			exit(0);
		}

		printf("The size %d is less than than P*Q (%d*%d)\n", size, p, q);
		MPI_Finalize();
		exit(0);
	}

        MPI_Comm rowCOMM[p], colCOMM[q];
        MPI_Comm_split(MPI_COMM_WORLD, rank/q, rank/q, &rowCOMM[rank/q]);
        MPI_Comm_split(MPI_COMM_WORLD, rank%q, rank%q, &colCOMM[rank%q]);
	
	MPI_Comm_rank(rowCOMM[rank/q], &rowRank);
	MPI_Comm_rank(colCOMM[rank%q], &colRank);

	printf ("rank %d in MPI_COMM_WORLD while rank %d in rowCOMM %d.\n", rank, rowRank, rank/q);	
	printf ("rank %d in MPI_COMM_WORLD while rank %d in colCOMM %d.\n", rank, colRank, rank%q);	
	
	MPI_Finalize();
	exit(0);
}
