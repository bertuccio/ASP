/**
 * File: pi.c
 * Description:
 *      The program approximates the value of pi through random numbers 8Montecarlo's method)
 * 
 * Author: Ari Handler Gamboa & Adrian Lorenzo Mateo
 * Date:    09/29/2013
 * 
 * @param argc
 * @param argv
 * @return 
 */ 

#include <stdio.h>
#include <math.h>
#include <stdlib.h>  


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

	int i;
	int radius;
	float distance;
	point random_point;
	int n_points;
	int k=0;
	float pi;

	if(argc!=NUM_ARG){
		printf("\nUSAGE: %s radius n_points\n", argv[0]);
		return -1;
	}

	if ((radius=atoi(argv[1])) <= 0){
		fprintf(stderr, "(ERROR\t) Invalid radius\n");
		return -1;
	}

	if ((n_points=atoi(argv[2])) <= 0){
		fprintf(stderr, "(ERROR\t) Invalid number of points\n");
		return -1;
	}

	srand(time(NULL));

	for(i=0;i<n_points;i++){
		/*Generates coordinates inradius the square*/
		random_point.x=random_num(radius);
		random_point.y=random_num(radius);
		/* Euclidean distance */
		distance = sqrt(pow(random_point.x,2) + pow(random_point.y,2));

		if(distance<radius)
			k++;
	}

	printf("(INFO\t) Number of k's total: %d\n",k);

	pi = (float) (4*k)/n_points;

	printf("(RESULT\t) Approximate Pi = %.15f\n",pi);

	return 0;
}



