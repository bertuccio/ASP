/**
 * File: parallelSum.c
 * Description:
 *      The program resolves the an arithmetic progression of N first numbers
 *       (starting with number 1) with Parallel Computation
 * 
 * Author: Ari Handler Gamboa & Adrian Lorenzo Mateo
 * Date:    09/29/2013
 * 
 * @param argc
 * @param argv
 * @return 
 */

#include <stdlib.h>  
#include <stdio.h>
#include <mpi.h>

#define RANK_MASTER 0
#define RANGE 2
#define NUM_ARG 2

int main(int argc, char **argv){  
	int rank, size;  

	int i, n;
    int l, m; /*Indexes for distributing the numbers 
               * (of the arithmetic progression) to the slave processes*/
    int interval; /*interval of indexes l and m */
	int sendNumbers[RANGE];/*Array to send the indexes l and m (RANGE=2)*/
	int addition=0; /* Sum of the remaining positions */
	int result=0; /* Final result */
	MPI_Status status;


	/* Start up MPI */  

	MPI_Init(&argc, &argv);  

	/* Get some info about MPI */  

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
	MPI_Comm_size(MPI_COMM_WORLD, &size);  



    /*Input Error Control*/
	if(argc!=NUM_ARG){
		if(rank==RANK_MASTER)
			printf("\nUSAGE: \n %s <n_number>\n", argv[0]);

		MPI_Finalize();
		return -1;
	}
	
	if (((n=atoi(argv[1])) <= 0)){
		if(rank==RANK_MASTER)
			fprintf(stderr, "(ERROR\t) Invalid number\n");

		MPI_Finalize();
		return -1;
	}


	if(rank==RANK_MASTER){
                
        /*Each slave process have a common interval for indexes l and m*/
		interval = n / (size-1);
		l = 1;
		m = l+interval-1;
                
        /*The master sends the indexes to each slave to calculate the partial sums*/
		for(i=1;i<size;i++){
			sendNumbers[0] = l;
			sendNumbers[1] = m;
			MPI_Send(sendNumbers,2,MPI_INT,i,0,MPI_COMM_WORLD);
			l = m+1;
			m = l+interval-1;
		}
                
		/*If the master can't distribute all numbers, does the rest of partial sums*/
		if(l <= n){
			for(i=l; i<=n ;i++) 
				addition+=i;
		}
                
         /* The master calculates all the partial sums into "result" */
		MPI_Reduce(&addition,&result, 1, MPI_INT, MPI_SUM,RANK_MASTER, MPI_COMM_WORLD);

		printf("\n(RESULT\t) Total sum of the first %d natural numbers = %d\n", n, result);

	}
	else{
        /* Each slave process receives the appropriate indexes l and m */
		MPI_Recv(sendNumbers,2, MPI_INT, RANK_MASTER, MPI_ANY_TAG,MPI_COMM_WORLD,&status);
		l = sendNumbers[0];
		m = sendNumbers[1];
                
        /* Each slave calculates the partial sums */
		for (i=l; i<=m ; i++)
			addition += i;

        
        /*Each slave do a MPI_Reduce message to send partial addition to the master */
		MPI_Reduce(&addition,NULL, 1, MPI_INT, MPI_SUM, RANK_MASTER, MPI_COMM_WORLD);
	}

	/* All done */  

	MPI_Finalize();

	return 0;  
}  

