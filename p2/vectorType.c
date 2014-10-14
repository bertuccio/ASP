/**
 * File: contiguousType.c
 * 
 * Author: Ari Handler Gamboa & Adrian Lorenzo Mateo
 * Date:    10/08/2013
 * 
 * @param argc
 * @param argv
 * @return 
 */
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define RANK_MASTER 0
#define SIZE 4

int main (int argc, char *argv[]){

	int   i,j,
		  numtasks, rank;
	int tag=1;
	float a[SIZE][SIZE] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0};
	float b[SIZE];
	MPI_Datatype columntype;
	MPI_Status status;


	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);


	MPI_Type_vector(SIZE, 1, SIZE, MPI_FLOAT, &columntype);
	MPI_Type_commit(&columntype);


	if (rank == RANK_MASTER) {

		for(i=0;i<SIZE;i++){
			for(j=0;j<SIZE;j++)
				printf("%.1f ",a[i][j]);
			printf("\n");
		}

		for (i=0; i<numtasks; i++)
		MPI_Send(&a[0][i], 1, columntype, i, tag, MPI_COMM_WORLD);
	}

	MPI_Recv(b, SIZE, MPI_FLOAT, 0, tag, MPI_COMM_WORLD, &status);
	for(i=0;i<SIZE;i++)
		printf("RANK #%d b[%d]:=%.1f\n",rank,i,b[i]);
	
	MPI_Type_free(&columntype);

	MPI_Finalize();
	return 0;

}  /* end of main */
