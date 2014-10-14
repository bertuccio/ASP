/**
 * File: latency.c
 * 
 * Author: Ari Handler Gamboa & Adrian Lorenzo Mateo
 * Date:    09/29/2013
 * 
 * @param argc
 * @param argv
 * @return 
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"
#define MSGLEN 2048         /* length of message in elements */
#define MEGA_BYTE 20
#define RANK_MASTER 0
#define	NUMBER_REPS_LATENCY	1000
#define NUMBER_REPS_MESSAGE 10

int main( int argc, char *argv[]) {

	char *message1=NULL;
	char onebyte;
	int rank, size_message,        /* rank of task in communicator */
		tag=1, i,j;

	MPI_Status status;         		/* status of communication */
	double T1, T2, 					/* start/end times per rep */
		latency=0, tw;              


  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );

	if (rank == RANK_MASTER) {

		for(i=0;i<NUMBER_REPS_LATENCY;i++){
		
			T1 = MPI_Wtime();     /* start time */
			MPI_Send(&onebyte, 1, MPI_CHAR, 1, tag, MPI_COMM_WORLD);
			MPI_Recv(&onebyte, 1, MPI_CHAR, 1, tag, MPI_COMM_WORLD, &status);
			T2 = MPI_Wtime();     /* end time */

			/* calculate latency, divide 2 due to round trip */
			latency += (double)(( T2 - T1 ))/2; /* *1000000 because are uSecs*/
			

		}
		latency = (double) latency/NUMBER_REPS_LATENCY;
		printf("%.4f\n",latency);
		

	  for(i=1;i<=MEGA_BYTE;i++){

  		size_message = pow(2,i);
		message1 = (char *)malloc(sizeof(char)*pow(2,i));

	  	for(j=0;j<NUMBER_REPS_MESSAGE;j++){

			T1 = MPI_Wtime();     /* start time */  
			MPI_Send(message1, size_message, MPI_CHAR, 1, tag, MPI_COMM_WORLD);
			MPI_Recv(message1, size_message, MPI_CHAR, 1, tag, MPI_COMM_WORLD, &status);
			T2 = MPI_Wtime();     /* end time */
			tw += (double)((( T2 - T1 )) /2);
	  	}

	  	printf("%d; %.3f\n",size_message,(double)tw/NUMBER_REPS_MESSAGE);
		free(message1);
	  	
	 }
	}
	else {

		/* One byte (latency)*/
		for(i=0;i<NUMBER_REPS_LATENCY;i++){
			
			MPI_Recv(&onebyte, 1, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
			MPI_Send(&onebyte, 1, MPI_CHAR, 0, tag, MPI_COMM_WORLD);

			
		}

		for(i=1;i<=MEGA_BYTE;i++){
			message1 = (char *)malloc(sizeof(char)*pow(2,i));
			size_message = pow(2,i);

			for(j=0;j<NUMBER_REPS_MESSAGE;j++){
				MPI_Recv(message1, size_message, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
				MPI_Send(message1, size_message, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
			}
			free(message1);
		}

  	}

  
  MPI_Finalize();
  return 0;
}
