/**
 * File: sum.c
 * Description:
 *      The program resolves the arithmetic progression of the N first numbers
 *       (starting at 1)
 * 
 * Author: Ari Handler Gamboa & Adrian Lorenzo Mateo
 * Date:    09/29/2013
 * 
 * @param argc # of arguments
 * @param argv array of arguments
 * @return 0 on succes, -1 on failure
 */

#include<stdio.h>

#define NUM_ARG 2

int main(int argc, char *argv[]){

	int i, n;
	int suma=0;
	
	printf("\n");

	if(argc!=NUM_ARG){
		fprintf(stderr, "USAGE: \n%s <n number>\n", argv[0]);
		return -1;
	}
	
	if ((n=atoi(argv[1])) <= 0){
		fprintf(stderr, "(ERROR\t) Invalid number\n");
		return -1;
	}

	for(i=1;i<=n;i++)
		suma+=i;
        
	printf("(RESULT\t) Total sum of the first %d natural numbers = %d\n",n,suma);

	printf("\n");

	return 0;
}