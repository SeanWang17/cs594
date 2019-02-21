#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<mpi.h>
#include<stdlib.h>

void swap(float *a, float *b){
	float *t = malloc(sizeof(float));
	*t = *a;
	*a = *b;
	*b = *t;
	free(t);
}

float* transpose( float* matrix, int n, int p, int q, MPI_Datatype ddt ){
	float* localcopy = (float*)malloc(sizeof(float) * p * q);
	int i, j, mc = 0;
	int count = 1, position = 0;;
	//	printf("matrix starts with %d\n", );

	MPI_Pack(matrix, count, ddt, localcopy, sizeof(float) * p * q, &position, MPI_COMM_SELF);

	/*
	   for( i = 0; i < q; i++ ){
	   memcpy( &localcopy[i*p], &matrix[mc], sizeof(float) * p );
	   mc+=n;
	   }
	   */

	//		for( i = 0; i < p*q; i++ )
	//			printf("%.f ", localcopy[i]);
	//		printf("\n");
	for( i = 0; i < p; ++i )
		for( j = i+1; j < p; ++j )
			swap( &localcopy[p*i+j], &localcopy[p*j+i] );

	return localcopy;
}

int main(int argc, char **argv){
	int rank, size, tag=0;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int n = atoi(argv[1]), p = atoi(argv[2]), q = atoi(argv[3]);
//	lol
	int matrix[n][n];
	int i, j;
	for(i=0; i<n; i++){
		for(j=0; j<n; j++){
			if (i-j<=1 && i-j>=-1)
				matrix[i][j] = i+1;
			else
				matrix[i][j] = 0;
		}
	}
	if(rank==0){
		for(i=0; i<n; i++){
			for(j=0; j<n; j++){
				if(j==n-1)
					printf("%d\n", matrix[i][j]);
				else
					printf("%d ",matrix[i][j]);
			}
		}
	}
	int row, col,send[n/q][n/p],recv[n/q][n/p];
	row = rank/q;
	col = rank%q;
	for(i=0; i<n/p; i++){
		for(j=0; j<n/q; j++){
			send[j][i] = recv[j][i] = matrix[row*n/p+i][col*n/q+j];
		}
	}
	
	if(row < col)
		MPI_Send(&send, n/q*n/p, MPI_INT, col*q+row, tag, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);
	if(row > col){
		MPI_Recv(&recv, n/p*n/q, MPI_INT, col*q+row, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Send(&send, n/q*n/p, MPI_INT, col*q+row, tag, MPI_COMM_WORLD);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	if(row < col)
		MPI_Recv(&recv, n/p*n/q, MPI_INT, col*q+row, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	MPI_Barrier(MPI_COMM_WORLD);
//	for(j=0; j<p; j++){
//		MPI_Barrier(MPI_COMM_WORLD);
//		if(row==j){
//			for(i=0; i<n/q; i++){
//				if(col==q-1 && i==n/q-1)
//					printf("%d\n", sub[0][i]);
//				else
//					printf("%d ", sub[0][i]);	
//			}
//		}
//	}
	for(i=0; i<n/q;i++){
		for (j=0; j<n/p; j++)
			printf("%d ",recv[i][j]);
	}
	printf("rank %d\n",rank);	

	MPI_Finalize();
	exit(0);

}

