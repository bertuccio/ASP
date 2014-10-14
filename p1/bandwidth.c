/**
 * File: bandwidth.c
 * 
 * Author: Ari Handler Gamboa & Adrian Lorenzo Mateo
 * Date:    09/29/2013
 * 
 * @param argc
 * @param argv
 * @return 
 */
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define NUM_ARG 2
#define RANK_MASTER 0
#define STARTSIZE     1
#define ENDSIZE       1000000
#define INCREMENT     1000
#define THRESHOLD 32768
#define LOOP 10

int main (int argc, char *argv[]){

  int     numtasks, rank, n, i, loop, j, nbytes, start, end, incr,
          rc, tag=1;
  double  thistime, bw=0,
           t1, t2;
  char    msgbuf[ENDSIZE];
  MPI_Status status;


  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

  if (numtasks != NUM_ARG) {
    printf("ERROR: Must be an even number of tasks!  Quitting...\n");
    MPI_Abort(MPI_COMM_WORLD, rc);
    exit(-1);
  }
    
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  start = STARTSIZE;
  end = ENDSIZE;
  incr = INCREMENT;
  loop = LOOP;

  /*Fill de message with 'x'*/
  for (i=0; i<end; i++)
    msgbuf[i] = 'x';


  if (rank == RANK_MASTER) {

    for (n=start; n<=end; n=n+incr) {
      
      for(i=0;i<LOOP;i++){
          nbytes =  sizeof(char) * n;

          t1 = MPI_Wtime();
          MPI_Send(&msgbuf, n, MPI_CHAR, 1, tag, MPI_COMM_WORLD);
          MPI_Recv(&msgbuf, n, MPI_CHAR, 1, tag, MPI_COMM_WORLD, &status);
          t2 = MPI_Wtime();
          thistime = t2 - t1;
          /* *2 for the round trip, and divides 1000000.0 to convert to MBs*/
          bw += (((double)nbytes * 2) / thistime) / 1000000.0;  
  	      
       }
       /*Smoothes the result*/
       bw = bw / LOOP;
       
       printf("%d %.3f\n",n,bw);
       
        
    }
    

  }

  else {
    for (n=start; n<=end; n=n+incr) {
      	
        for(i=0;i<LOOP;i++){
  	      MPI_Recv(&msgbuf, n, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
  	      MPI_Send(&msgbuf, n, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
        }
      }
  }


  MPI_Finalize();
  return 0;

}  /* end of main */
