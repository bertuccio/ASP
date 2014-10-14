/**
 * File: pi.c
 * Description:
 *      The program estimates the value of pi number through random numbers 
	with parallel computing
 * 
 * Authors: Ari Handler Gamboa & Adrian Lorenzo Mateo
 * Date:    09/29/2013
 * 
 * @param argc
 * @param argv
 * @return 
 */ 

#include <stdio.h>
#include <math.h>
#include <stdlib.h>  
#include <mpi.h>
#include <unistd.h>

#define RANK_MASTER 0
#define NUM_ARG 3

/***************************************************/
/* Function: random_num Date: 09/29/2012           */
/* Authors: Adrián Lorenzo Mateo                   */
/*        Ari Handler Gamboa               	   */
/*                                                 */
/* Description:					   */
/* 	Generates a random number 		   */
/*	  between 0 and sup     		   */
/*                                                 */
/* Inpunt:                                         */
/* 	float sup: limite superior                 */
/* Output:                                         */
/* 	float: random number                       */
/***************************************************/
float random_num(float sup)
{
    /*Input Error Control*/
    if ((sup > RAND_MAX) || (sup < 0) || (sup > RAND_MAX))
        return -1;
 
    return (float)rand()/((float)RAND_MAX/sup);
}

/**
 * Structure for model points with coordinates x, y
 */ 
typedef struct 
{
	float x;
	float y;
}point;


int main(int argc, char *argv[]){

	int rank, n_processes; 
	MPI_Status status;

	int i;
	int radius; /*square side*/
	float distance;
	point random_point;

	int n_points; /*number of random points to generate*/
	int k=0;	/*number of random points inside the circle sector*/
	int zero=0;
	float pi;


	/* Start up MPI */  

	MPI_Init(&argc, &argv);  

	/* Get some info about MPI */  

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
	MPI_Comm_size(MPI_COMM_WORLD, &n_processes);  



	/*Input Error Control*/
	if(argc!=NUM_ARG){
		if(rank==RANK_MASTER)
			printf("USAGE:\n %s radius n_points\n", argv[0]);
		MPI_Finalize();		
		return -1;
	}

	if((radius = atoi(argv[1]))<=0){
		if(rank==RANK_MASTER)
			printf("(ERROR\t) Invalid radius\n", argv[0]);
		MPI_Finalize();		
		return -1;
	}

	if((n_points = atoi(argv[2]))<=0){
		if(rank==RANK_MASTER)
			printf("(ERROR\t) Invalid number of points\n", argv[0]);
		MPI_Finalize();		
		return -1;
	}
	
	if(rank==RANK_MASTER){
	


		/*The master do a MPI_Reduce message for calculate all of the numbers of points inside the circle*/
		MPI_Reduce(&zero,&k, 1, MPI_INT, MPI_SUM,RANK_MASTER, MPI_COMM_WORLD);
		printf("(INFO\t) Number of k's total: %d\n",k);

		pi = (float) (4*k)/((n_processes-1)*n_points);

		printf("Approximate Pi: %.15f\n",pi);
	}
	else{

		
		/*Seeding with a value that depends on the process number*/
		srand(time(NULL)-rank*2);
		for(i=0;i<n_points;i++){
			/*Generates coordinates inradius the square*/
			random_point.x=random_num(radius);
			random_point.y=random_num(radius);
			/*Calculates the distance between the origin and the random point*/
			distance = sqrt(pow(random_point.x,2) + pow(random_point.y,2));
			
			/*Counts the number of random points inside the circle sector*/
			if(distance<radius)
				k++;
		}

		
		/*Each slave do a MPI_Reduce message to send the number of points inside the circle sector*/
		MPI_Reduce(&k,NULL, 1, MPI_INT, MPI_SUM, RANK_MASTER, MPI_COMM_WORLD);
		


	}
	/* All done */  

	MPI_Finalize();

	return 0;  
}  

