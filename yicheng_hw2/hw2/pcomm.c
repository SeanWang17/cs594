#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>

int main(int argc, char **argv){
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int p = atoi(argv[1]), q = atoi(argv[2]);
	if( size < p * q ){
		if( rank == 0)
			printf("# of processes < P x Q\n");
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

	int ns_comm = rank % p, ew_comm = rank / p;

	MPI_Comm ns[p], ew[q];
	MPI_Comm_split(MPI_COMM_WORLD, rank%p, rank, &ns[ns_comm]);
	MPI_Comm_split(MPI_COMM_WORLD, rank/p, rank, &ew[ew_comm]);

	int nsFinal[p], ewFinal[q];
	int nsToken[p], ewToken[q];
	int i;
	for( i = 0; i < p; i++ ){
		nsToken[i] = 0;
		nsFinal[i] = 0;
	}
	for( i = 0; i < q; i++ ){
		ewToken[i] = 0;
		ewFinal[i] = 0;
	}

	int ns_rank, ew_rank, ns_size, ew_size;
	ns_rank = rank / p;
	ew_rank = rank % p;
	MPI_Comm_size(ns[ns_comm], &ns_size);
	MPI_Comm_size(ew[ew_comm], &ew_size);

	int next_ns = ns_rank+1, prev_ns = ns_rank - 1;
	int next_ew = ew_rank+1, prev_ew = ew_rank - 1;
	int count = 1, tag = 0;

	if( ns_rank == 0 ){
		nsToken[ns_comm]++;
		MPI_Send( &nsToken[ns_comm], count, MPI_INT, next_ns, tag, ns[ns_comm] );
//		printf("NorthSouth rank %d send %d to next ns %d\n",
//			rank, nsToken[ns_comm], next_ns);
	} else if( ns_rank == q - 1 ){
		MPI_Recv( &nsToken[ns_comm],count,MPI_INT,prev_ns,tag,ns[ns_comm],MPI_STATUS_IGNORE );
		printf("rank %d receives %d from NorthSouth\n", 
			rank, nsToken[ns_comm]);
	} else {
		MPI_Recv( &nsToken[ns_comm],count,MPI_INT,prev_ns,tag,ns[ns_comm],MPI_STATUS_IGNORE );
//		printf("rank %d receives %d from NorthSouth\n", 
//			rank, nsToken[ns_comm]);
		nsToken[ns_comm]++;
		MPI_Send( &nsToken[ns_comm], count, MPI_INT, next_ns, tag, ns[ns_comm] );
//		printf("NorthSouth rank %d send %d to next ns %d\n",
//			rank, nsToken[ns_comm], next_ns);
	}

	if( ew_rank == 0 ){
		ewToken[ew_comm]++;
		MPI_Send( &ewToken[ew_comm], count, MPI_INT, next_ew, tag, ew[ew_comm] );
//		printf("EastWest rank %d send %d to next ew %d\n",
//			rank, ewToken[ew_comm], next_ew);
	} else if( ew_rank == p - 1 ){
		MPI_Recv( &ewToken[ew_comm],count,MPI_INT,prev_ew,tag,ew[ew_comm],MPI_STATUS_IGNORE );
		printf("rank %d receives %d from EastWest\n", 
			rank, ewToken[ew_comm]);
	} else {
		MPI_Recv( &ewToken[ew_comm],count,MPI_INT,prev_ew,tag,ew[ew_comm],MPI_STATUS_IGNORE );
//		printf("rank %d receives %d from EastWest\n", 
//			rank, ewToken[ew_comm]);
		ewToken[ew_comm]++;
		MPI_Send( &ewToken[ew_comm], count, MPI_INT, next_ew, tag, ew[ew_comm] );
//		printf("EastWest rank %d send %d to next ew %d\n",
//			rank, ewToken[ew_comm], next_ew);
	}


	MPI_Finalize();
	exit(0);

}
