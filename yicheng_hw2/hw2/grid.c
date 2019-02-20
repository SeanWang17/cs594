#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

int main(int argc, char **argv){
	int rank, size;

	if(argc < 3){
		printf("Need two inputs P & Q\n");
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

		printf("number of processes is not enough for your grid!\n");
		MPI_Finalize();
		exit(0);
	}

        MPI_Comm ns[q], ew[p];
        MPI_Comm_split(MPI_COMM_WORLD, rank/p, rank/p, &ew[rank/p]);
        MPI_Comm_split(MPI_COMM_WORLD, rank%q, rank%q, &ns[rank%q]);

	

	MPI_Finalize();
	exit(0);
}
