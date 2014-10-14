/****************************************************************************
* FILE: mpi_bandwidth.c
* DESCRIPTION:
*   Provides point-to-point communications timings for any even
*   number of MPI tasks.
****************************************************************************/ 
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

/* Change the next four parameters to suit your case */
#define STARTSIZE     1
#define ENDSIZE       1000000
#define INCREMENT     1000
#define THRESHOLD 32768

int main (int argc, char *argv[])
{
int     numtasks, rank, n, i, loop, j, nbytes, start, end, incr,
        src, dest, rc, tag=1, namelength;
double  thistime, bw=0, bestbw, worstbw, totalbw, avgbw,
        bestall, avgall, worstall,
         tmptimes[3],
        resolution, t1, t2;
char    msgbuf[ENDSIZE];
struct 	timeval tv1, tv2;
MPI_Status status;

/* Some initializations and error checking */
MPI_Init(&argc,&argv);
MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

if (numtasks != 2) {
  printf("ERROR: Must be an even number of tasks!  Quitting...\n");
  MPI_Abort(MPI_COMM_WORLD, rc);
  exit(0);
  }
  
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
start = STARTSIZE;
end = ENDSIZE;
incr = INCREMENT;
loop = 10;


for (i=0; i<end; i++)
  msgbuf[i] = 'x';




/*************************** first half of tasks *****************************/
/* These tasks send/receive messages with their partner task, and then do a  */ 
/* few bandwidth calculations based upon message size and timings.           */

if (rank == 0) {
  for (n=start; n<=end; n=n+incr) {
    
    for(i=0;i<loop;i++){
	    nbytes =  sizeof(char) * n;
	    
	      t1 = MPI_Wtime();
	      MPI_Send(&msgbuf, n, MPI_CHAR, 1, tag, MPI_COMM_WORLD);
	      MPI_Recv(&msgbuf, n, MPI_CHAR, 1, tag, MPI_COMM_WORLD, &status);
	      t2 = MPI_Wtime();
	      thistime = t2 - t1;
	      bw += (((double)nbytes * 2) / thistime) / 1000000.0;
	      
	      
     }
     
     bw = bw / loop;
     
     printf("%d %.3f\n",n,bw);
     
      
  }
  
    
   

}



/**************************** second half of tasks ***************************/
/* These tasks do nothing more than send and receive with their partner task */

else {
  for (n=start; n<=end; n=n+incr) {
    	
      for(i=0;i<loop;i++){
	      MPI_Recv(&msgbuf, n, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
	      MPI_Send(&msgbuf, n, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
      }
    }
}


MPI_Finalize();

}  /* end of main */
