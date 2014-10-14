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
#define NELEMENTS 6
int main (int argc, char *argv[]){

	int   i,
		  numtasks, rank;
	int tag=1;
	float a[16] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0};
	float b[SIZE];
	int blocklengths[2];
	int displacements[2];
	MPI_Datatype indextype;
	MPI_Status status;


	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);


	blocklengths[0] = 4; blocklengths[1] = 2; displacements[0] = 5; displacements[1] = 12;
	MPI_Type_indexed(2, blocklengths, displacements, MPI_FLOAT, &indextype);
	MPI_Type_commit(&indextype);



	if (rank == RANK_MASTER) {

		for(i=0;i<SIZE;i++)
				printf("%.1f ",a[i]);
		printf("\n");
		

		for (i=0; i<numtasks; i++)
		MPI_Send(a, 1, indextype, i, tag, MPI_COMM_WORLD);
	}

	MPI_Recv(b, NELEMENTS, MPI_FLOAT, 0, tag, MPI_COMM_WORLD, &status);
	for(i=0;i<NELEMENTS;i++)
		printf("RANK #%d b[%d]:=%.1f\n",rank,i,b[i]);
	
	MPI_Type_free(&indextype);

	MPI_Finalize();
	return 0;

}  /* end of main */
