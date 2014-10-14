/* -----------------------------------------------------------------------
 * Code:   deadlock.c
 * Lab:    MPI Point-to-Point Communication
 *         Improper use of blocking calls results in deadlock. To fix the
 *         code, replace blocking send with non-blocking send.
 * Usage:  deadlock
 *         Run on two nodes
 *         You will need to stop the deadlocked program with lamclean 
 * ------------------------------------------------------------------------ */
#include <stdio.h>
#include "mpi.h"
#define MSGLEN 2048         /* length of message in elements */
#define TAG_A 100
#define TAG_B 200

main( argc, argv ) 
     int argc;
     char **argv;
{
  float message1 [MSGLEN],   /* message buffers                      */
        message2 [MSGLEN];
  int rank,                  /* rank of task in communicator         */
      dest, source,          /* rank in communicator of destination  */
                             /* and source tasks                     */
      send_tag, recv_tag,    /* message tags                         */
      i;
  MPI_Status status;         /* status of communication              */

  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  printf ( " Task %d initialized\n", rank );

  /* initialize message buffers */
  for ( i=0; i<MSGLEN; i++ )  {
    message1[i] = 100;
    message2[i] = -100;
  }

  /* ---------------------------------------------------------------
   * each task sets its message tags for the send and receive, plus
   * the destination for the send, and the source for the receive 
   * --------------------------------------------------------------- */
  if ( rank == 0 )  {
    dest = 1;
    source = 1;
    send_tag = TAG_A;
    recv_tag = TAG_B;
  }
  else if ( rank == 1)  {
    dest = 0;
    source = 0;
    send_tag = TAG_B;
    recv_tag = TAG_A;
  }

  /* ---------------------------------------------------------------
   * send and receive messages 
   * --------------------------------------------------------------- */
  printf ( " Task %d has sent the message\n", rank );
  MPI_Send ( message1, MSGLEN, MPI_FLOAT, dest, send_tag, MPI_COMM_WORLD );
  MPI_Recv ( message2, MSGLEN, MPI_FLOAT, source, recv_tag, MPI_COMM_WORLD, 
	    &status );
  printf ( " Task %d has received the message\n", rank );

  MPI_Finalize();
  return 0;
}
