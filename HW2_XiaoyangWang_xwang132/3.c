#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>

int main(int argc, char **argv){
	int rank, size, rowRank, colRank, count=1, tag=0;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int p = atoi(argv[1]), q = atoi(argv[2]);
	if( size < p * q ){
		if( rank == 0)
			printf("The size %d is less than than P*Q (%d*%d)\n", size, p, q);
		MPI_Finalize();
		exit(0);
	}

	if( size > p * q ){
		int i;
		for( i = size - 1; i > p*q - 1; i-- ){
			if(rank == i){
				printf("rank %d finalize\n", rank);
				MPI_Finalize();
				exit(0);
			}
		}
	}


	MPI_Comm rowCOMM[p], colCOMM[q];
	MPI_Comm_split(MPI_COMM_WORLD, rank/q, rank/q, &rowCOMM[rank/q]);
	MPI_Comm_split(MPI_COMM_WORLD, rank%q, rank%q, &colCOMM[rank%q]);
	MPI_Comm_rank(rowCOMM[rank/q], &rowRank);
	MPI_Comm_rank(colCOMM[rank%q], &colRank);
	
	int rowToken[p], colToken[q];
	
	if( rowRank == 0 ){
		rowToken[rank/q] = 0;
		MPI_Send( &rowToken[rank/q], count, MPI_INT, rowRank+1, tag, rowCOMM[rank/q] );
		printf("rowCOMM %d rank %d sends %d.\n",
			rank/q, rowRank, rowToken[rank/q]);
	} else if( rowRank == p-1 ){
		MPI_Recv( &rowToken[rank/q], count, MPI_INT, rowRank-1, tag, rowCOMM[rank/q], MPI_STATUS_IGNORE );
		printf("rowCOMM %d rank %d receives %d.\n", 
			rank/q, rowRank, rowToken[rank/q]);
		rowToken[rank/q]++;
		printf("rowCOMM %d's final token is %d.\n",
			rank/q, rowToken[rank/q]);
	} else {
		MPI_Recv( &rowToken[rank/q], count, MPI_INT, rowRank-1, tag, rowCOMM[rank/q], MPI_STATUS_IGNORE );
		printf("rowCOMM %d rank %d receives %d.\n", 
			rank/q, rowRank, rowToken[rank/q]);
		rowToken[rank/q]++;
		MPI_Send( &rowToken[rank/q], count, MPI_INT, rowRank+1, tag, rowCOMM[rank/q] );
		printf("rowCOMM %d rank %d sends %d.\n",
			rank/q, rowRank, rowToken[rank/q]);
	}

	if( colRank == 0 ){
		colToken[rank%q] = 0;
		MPI_Send( &colToken[rank%q], count, MPI_INT, colRank+1, tag, colCOMM[rank%q] );
		printf("colCOMM %d rank %d sends %d.\n",
			rank%q, colRank, colToken[rank%q]);
	} else if( colRank == q-1 ){
		MPI_Recv( &colToken[rank%q], count, MPI_INT, colRank-1, tag, colCOMM[rank%q], MPI_STATUS_IGNORE );
		printf("colCOMM %d rank %d receives %d.\n", 
			rank%q, colRank, colToken[rank%q]);
		colToken[rank%q]++;
		printf("colCOMM %d's final token is %d.\n",
			rank%q, colToken[rank%q]);
	} else {
		MPI_Recv( &colToken[rank%q], count, MPI_INT, colRank-1, tag, colCOMM[rank%q], MPI_STATUS_IGNORE );
		printf("colCOMM %d rank %d receives %d.\n", 
			rank%q, colRank, colToken[rank%q]);
		colToken[rank%q]++;
		MPI_Send( &colToken[rank%q], count, MPI_INT, colRank+1, tag, colCOMM[rank%q] );
		printf("colCOMM %d rank %d sends %d.\n",
			rank%q, colRank, colToken[rank%q]);
	}


	MPI_Finalize();
	exit(0);

}
