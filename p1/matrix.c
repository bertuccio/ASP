
#include <stdio.h>
#include <stdlib.h> 

#define NUM_ARG 2

/***************************************************/
/* Funcion: aleat_num Fecha: 29/09/2012            */
/* Autores: Adrián Lorenzo Mateo                   */
/*        Ari Handler Gamboa               */
/*                                                 */
/* Rutina que genera un numero aleatorio           */
/* entre dos numeros dados                         */
/*                                                 */
/* Entrada:                                        */
/* int inf: limite inferior                        */
/* int sup: limite superior                        */
/* Salida:                                         */
/* int: numero aleatorio                           */
/***************************************************/
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

int main(int argc, char *argv[]){


  int  i, j, k, sum = 0;
  int **first, **second, **multiply=NULL;
  int tamanio;
  
  if(argc!=NUM_ARG){
      printf("USAGE:\n %s <n_size>\n", argv[0]);
    return -1;
  }


  if((tamanio =atoi(argv[1]))<=0){
      printf("(ERROR\t) Invalid size\n");
    return -1;
  }

  srand(time(NULL));


  first = (int **)malloc(sizeof(int)*tamanio);
  second = (int **)malloc(sizeof(int)*tamanio);
  multiply = (int **)malloc(sizeof(int)*tamanio);

  for(i=0;i<tamanio;i++){
    first[i]= (int *)malloc(sizeof(int)*tamanio);
    second[i] = (int *)malloc(sizeof(int)*tamanio); 
    multiply[i] = (int *)malloc(sizeof(int)*tamanio); 
  }

  for(i=0;i<tamanio;i++){
    for(j=0;j<tamanio;j++){
      first[i][j]=aleat_num(0,tamanio);
      second[i][j]=aleat_num(0,tamanio);
    }
  }

  printf("First matrix:\n");
  for(i=0;i<tamanio;i++){
    for(j=0;j<tamanio;j++){
      printf("%d ",first[i][j]);
      if(j==tamanio-1)
        printf("\n");
    }
  }

  printf("Second matrix:\n");
  for(i=0;i<tamanio;i++){
    for(j=0;j<tamanio;j++){
      printf("%d ",second[i][j]);
      if(j==tamanio-1)
        printf("\n");
    }
  }
 
    for ( i = 0 ; i < tamanio ; i++ )
    {
      for ( j = 0 ; j < tamanio ; j++ )
      {
        for ( k = 0 ; k < tamanio ; k++ )
        {
          sum = sum + first[i][k]*second[k][j];
        }
 
        multiply[i][j] = sum;
        sum = 0;
      }
    }
 
    printf("Product of entered matrices:\n");
 
    for ( i = 0 ; i < tamanio ; i++ )
    {
      for ( j = 0 ; j < tamanio ; j++ ){
        printf("%d ", multiply[i][j]);
        if(j==tamanio-1)
          printf("\n");
      }
    }

    free(first);
    free(second);
    free(multiply);
  
 
  return 0;
}