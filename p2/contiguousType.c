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
int main (int argc, char *argv[]){

	int   i, j, 
		  numtasks, rank, 
		  count=0, numslavetasks;
	int tag=0;
	float **a=NULL;
	float *b=NULL;
	MPI_Datatype rowtype;
	MPI_Status status;



	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

	numslavetasks = numtasks -1;

	MPI_Type_contiguous(numslavetasks, MPI_FLOAT, &rowtype);
	MPI_Type_commit(&rowtype);



	if (rank == RANK_MASTER){
		a = (float **)malloc(sizeof(float)*numslavetasks);

		for(i=0;i<numslavetasks;i++)
			a[i] = (float *)malloc(sizeof(float)*numslavetasks);

		for(i=0;i<numslavetasks;i++){
			for(j=0;j<numslavetasks;j++)
				a[i][j]=++count;
		}

		for(i=0;i<numslavetasks;i++){
			for(j=0;j<numslavetasks;j++)
				printf("%.2f ",a[i][j]);
			printf("\n");		
		}

		for (i=1; i<numtasks; i++)
			MPI_Send(&a[i-1][0], 1, rowtype, i, tag, MPI_COMM_WORLD);

		free(a);
	}
	else{
		

		b = (float *)malloc(sizeof(float)*numslavetasks);	
		MPI_Recv(b, numslavetasks, MPI_FLOAT, 0, tag, MPI_COMM_WORLD, &status);
		printf("SLAVE #%d\n",rank);
		for(i=0;i<numslavetasks;i++)
			printf("%.3f ",b[i]);
		printf("\n");	

		free(b);
	}

	
	MPI_Type_free(&rowtype);

	MPI_Finalize();
	return 0;

}  /* end of main */
