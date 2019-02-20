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
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int n = atoi(argv[1]), p = atoi(argv[2]), q = atoi(argv[3]);

	float *matrix = (float*)malloc(sizeof(float) * n * n );

	//for test case create a matrix
	int initc, i, j;
	for(initc = 0; initc < n*n; initc++)
		matrix[initc] = (float)initc;

	//print initial matrix
	if(rank == 0){
		printf("initial matrix\n");
		for(initc = 0; initc < n*n; ){
			for(i = 0; i < n; i++){
				printf("%*.f ", 5, matrix[initc]);
				initc++;
			}
			printf("\n");
		}
		printf("-----------------------\n");
	}

	MPI_Barrier(MPI_COMM_WORLD);


	//transpose starts here
	int row = rank / (n / p) , col = rank % (n / p);
	int srank = col * (n / p) + row;

	int sp = col * p + row * q * n;
	//	printf("rank %d sp %d\n", rank, sp);
	float* tp = (float*)malloc(sizeof(float) * p * q);
	float* recv = (float*)malloc(sizeof(float) * p * q);
	int tag = 0, count = 1, position = 0;
	float* rbuf;

	MPI_Datatype ddt;
	MPI_Type_vector(q, p, n, MPI_FLOAT, &ddt);
	MPI_Type_commit(&ddt);

	//all rank do transpose locally
	tp = transpose( &matrix[ sp ], n, p, q, ddt );

	//upper triangle send first
	if( row > col ){
		//		MPI_Send( tp, p*q, MPI_FLOAT, srank, tag, MPI_COMM_WORLD );
		//		MPI_Recv( recv, p*q, MPI_FLOAT, srank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
		MPI_Send( tp, p*q, MPI_FLOAT, srank, tag, MPI_COMM_WORLD );
		MPI_Recv( recv, p*q, MPI_FLOAT, srank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE );

		MPI_Unpack( recv, sizeof(float)*p*q, &position, &matrix[sp], count, ddt, MPI_COMM_SELF );
	}

	//lower triangle recv first
	if( col > row ){
		//		MPI_Recv( recv, p*q, MPI_FLOAT, srank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
		//		MPI_Send( tp, p*q, MPI_FLOAT, srank, tag, MPI_COMM_WORLD );
		MPI_Recv( recv, p*q, MPI_FLOAT, srank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
		MPI_Unpack( recv, sizeof(float)*p*q, &position, &matrix[sp], count, ddt, MPI_COMM_SELF );
		MPI_Send( tp, p*q, MPI_FLOAT, srank, tag, MPI_COMM_WORLD );
	}

	//diagonal
	if( row == col ){
		memcpy( recv, tp, sizeof(float) * p * q );
		MPI_Unpack( recv, sizeof(float)*p*q, &position, &matrix[sp], count, ddt, MPI_COMM_SELF );
	}

	MPI_Barrier(MPI_COMM_WORLD);
	if(rank == 0)
		rbuf = (float*)malloc(sizeof(float) * n * n);

	//	for( j = 0; j < p; j++ ){
	//		printf("rank %d copying ", rank);
	//		for(i = 0; i < p; i++)
	//			printf("%.f ", recv[j*p+i]);
	//		printf("in rbuf[%d]\n", sp+j*n);
	//	}

	for(j = 0; j < size; j++){
		MPI_Barrier(MPI_COMM_WORLD);
		if(rank == j){
			printf("after transpose matrix rank %d has block\n", rank);
			for(initc = 0; initc < p*q; ){
				for(i = 0; i < p; i++){
					printf("%*.f ", 5, recv[initc]);
					initc++;
				}
				printf("\n");
			}
			printf("-----------------------\n");
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}



	MPI_Type_free(&ddt);
	free(recv);
	free(tp);
	free(matrix);
	MPI_Finalize();
	exit(0);

}
