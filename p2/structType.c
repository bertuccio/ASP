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
#define NELEMENTS 3

typedef struct{
	float x,y,z,velocity;
	int n,type;
} Particle;

int main (int argc, char *argv[]){

	int   i,
		  numtasks, rank;
	int tag=1;
	float a[16] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0};
	float b[SIZE];
	int blockcounts[2] ={4, 2};
	MPI_Datatype oldtypes[2] = {MPI_FLOAT, MPI_INT};
	int offsets[2];
	MPI_Aint extent;
	MPI_Status status;
	MPI_Datatype particletype;
	Particle particles[NELEMENTS], p[NELEMENTS];

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);


	MPI_Type_extent(MPI_FLOAT,&extent);
	offsets[0]=0;
	offsets[1]=4*extent;
	MPI_Type_struct(2,blockcounts,offsets,oldtypes,&particletype);
	MPI_Type_commit(&particletype);



	if (rank == RANK_MASTER) {

		for(i=0;i<NELEMENTS;i++){

			particles[i].x=1;
     		particles[i].y=2;
     		particles[i].z=3;
     		particles[i].velocity=4;
     		particles[i].n=5;
     		particles[i].type=6;

		}


		for (i=0; i<numtasks; i++)
		MPI_Send(particles,NELEMENTS, particletype, i, tag, MPI_COMM_WORLD);
	}

	MPI_Recv(p, NELEMENTS, particletype, 0, tag, MPI_COMM_WORLD, &status);

	for(i=0;i<NELEMENTS;i++)
		printf("RANK #%d: %.1f %.1f %.1f %.1f %d %d\n", rank,p[i].x,
     p[i].y,p[i].z,p[i].velocity,p[i].n,p[i].type);


	MPI_Type_free(&particletype);

	MPI_Finalize();
	return 0;

}  /* end of main */
