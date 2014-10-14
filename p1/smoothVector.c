
/**
 * File: smoothVector.c
 * Description:
 *      The program performs one-dimensional smoothing on a 2^N element vector.
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
#include <math.h>
#include <string.h>

#define VIRTUAL_ELEMENTS 4

int aleat_num(int inf, int sup)
{
    /* Control de errores de los parametros de entrada*/
    if ((sup < inf) || (sup > RAND_MAX) || ((inf < 0 ) ||
            (sup < 0)) || (sup > RAND_MAX))
        return -1;
    /*la función rand() limita el rango [0, 1) debido a la división entre RAND_MAX+1.
     * La segunda parte multiplica dicho valor por un número que se encuentre aproximadamente
     * en la mitad del rango (sup-inf +1), de este modo se normaliza el anterior valor
     * acotándolo entre 0 y el número intermedio.  Sin embargo, es necesario sumar el límite
     * inferior al resultado con el fin de que se obtenga un valor superior a este.*/
    return (int)((rand()/(RAND_MAX+1.))*(sup-inf+1))+inf;
}

int main(int argc, char **argv)  
{
	int n, size_vector;
	int * vector, smoothed_vector;
	int i;

	if (argc != 2){
		printf("Run: %s log2(tamano_vector)\n", argv[0]);
		return -1;
	}

	n = atoi(argv[1]);

	size_vector = pow(2, n);

	vector = (int *)malloc((size_vector+VIRTUAL_ELEMENTS)*sizeof(int));

	/* Posiciones adicionales a 0 */
	vector[0] = 0; vector[1] = 0;
	vector[size_vector+1] = 0; vector[size_vector+2] = 0;

	printf("\n## VECTOR = \t\t");
	for(i=2;i<size_vector+2;i++){
		vector[i] = aleat_num(1,size_vector);
		printf("%d ",vector[i]);
	}
	printf("\n");

	printf("\n## SMOOTHED VECTOR = \t");
	for(i=2; i<size_vector+2; i++){
		printf("%d ", (vector[i-2] + vector[i-1] + vector[i] + vector[i+1] + vector[i+2])/5);
	}

	printf("\n\n");

	free(vector);

	return 0;
}