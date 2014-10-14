#include <stdlib.h>  
#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <string.h>

#define RANK_MASTER 0
#define TAG_ROWS 1
#define TAG_REMAINING_ROWS 2
#define TAG_RESULT 3
#define LABEL_MASTER "# \033[1;32mMASTER \033[0m#"
#define LABEL_SLAVE "\033[1;35mSLAVE\033[0m"
#define NUM_ARG 2

int multiply_row (int * result_row, int * first_row, int ** second_matrix, int size)
{
  int index_result, index_first;
  int sum;

  if (result_row == NULL || first_row == NULL || size <= 0)
    return -1;

  for (index_result = 0; index_result < size ;index_result++) {
    sum = 0;
    for (index_first = 0; index_first < size ;index_first++) {
      sum += first_row[index_first]*second_matrix[index_first][index_result];
    }
    result_row[index_result] = sum;
  }

  return 0;
}

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


  int  i, j, k, rows_index, last_rows_index, sum = 0;
  int **first, **second;
  int **result;
  int *row_first, *row_result = NULL;
  int size;
  int rank, n_process, n_process_working;
  int rows_per_process, remaining_rows;
  MPI_Status status;
  int no_need = 0;
  

  srand(time(NULL));

  /* Start up MPI */  

  MPI_Init(&argc, &argv);  

  /* Get some info about MPI */  

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
  MPI_Comm_size(MPI_COMM_WORLD, &n_process);

  if(argc!=NUM_ARG){
    if(rank == RANK_MASTER)
      printf("USAGE:\n %s <n_size>\n", argv[0]);
    MPI_Finalize();
    return -1;
  }

  if((size =atoi(argv[1]))<=0){
    if(rank == RANK_MASTER)
      printf("(ERROR\t) Invalid size\n");
    MPI_Finalize();
    return -1;
  }

  
  if (rank == RANK_MASTER) {

    /* Allocation and setting of the matrices */
    first = (int **)malloc(sizeof(int)*size);
    second = (int **)malloc(sizeof(int)*size);
    result = (int **)malloc(sizeof(int)*size);

    for(i=0;i<size;i++){
      first[i]= (int *)malloc(sizeof(int)*size);
      second[i] = (int *)malloc(sizeof(int)*size); 
	result[i] = (int *)malloc(sizeof(int)*size);
    }

    
    /* Setting the contents of the matrices and broadcast of the second matrix */
    for(i=0;i<size;i++){
      for(j=0;j<size;j++){
        first[i][j]=aleat_num(0,size);
        second[i][j]=aleat_num(0,size);
        result[i][j]=aleat_num(0,size);
      }
      MPI_Bcast(&(second[i][0]), size, MPI_INT, RANK_MASTER, MPI_COMM_WORLD);
    }

    /* Send to each slave (rank-1)*(size/n_process)+1 to i*(size/n_process)
     * rows of the first column */

    if (size < (n_process-1)) {
      rows_per_process = 1;
      remaining_rows = 0;
      n_process_working = size;
    }
    else {
      rows_per_process = size / (n_process-1);
      remaining_rows = size % (n_process-1);
      n_process_working = n_process-1;
    }

    printf(" %s ROWS PER PROCESS = %d\n",LABEL_MASTER, rows_per_process);
    printf(" %s REMAINING ROWS = %d\n",LABEL_MASTER, remaining_rows);
    printf(" %s PROCESSES WORKING = %d\n",LABEL_MASTER, n_process_working);

    for (i = 1; i <= n_process_working; i++)
    {
      for (rows_index = (i-1)*(rows_per_process); rows_index < i*rows_per_process ;rows_index++){
        MPI_Send(&(first[rows_index][0]),size,MPI_INT,i,TAG_ROWS,MPI_COMM_WORLD);
        printf(" %s SENT ROW [%d] to processor %d\n",LABEL_MASTER, rows_index, i);
      }
    }

    /* If there are remaining rows (never more than N-1), master sends to each
     * processos like a Round Robin */ 
    last_rows_index = rows_index; /* The first remaining row to send */
    for (i = 1; i <= remaining_rows ; i++, last_rows_index++){
        MPI_Send(&(first[last_rows_index][0]),size,MPI_INT,i,TAG_REMAINING_ROWS,MPI_COMM_WORLD);
        printf(" %s SENT REMAINING ROW [%d] to processor %d\n",LABEL_MASTER, last_rows_index, i);
    }

    /* Receives result */
    for (i = 1; i <= n_process_working; i++)
    {
      for (rows_index = (i-1)*(rows_per_process); rows_index < i*rows_per_process ;rows_index++){
        MPI_Recv(&(result[rows_index][0]),size,MPI_INT,i,TAG_RESULT,MPI_COMM_WORLD,&status);
      }
    }

    /* Receives remaining rows of the result */
    last_rows_index = rows_index; /* The first remaining row to receive */
    for (i = 1; i <= remaining_rows ; i++, last_rows_index++){
        MPI_Recv(&(result[last_rows_index][0]),size,MPI_INT,i,TAG_RESULT,MPI_COMM_WORLD,&status);
    }

    printf(" %s RESULTS\n", LABEL_MASTER);
    /* PRINT */
    for(i=0;i<size;i++){
      printf("\t");
      for(j=0;j<size;j++){
        printf("%d ",first[i][j]);
      }
      printf("x ");
      for(j=0;j<size;j++){
        printf("%d ",second[i][j]);
      }
      printf("= ");
      for(j=0;j<size;j++){
        printf("%d ",result[i][j]);
      }
      printf("\n");
    }


    free(first);
    free(second);
    free(result);

    MPI_Barrier(MPI_COMM_WORLD);
  }
  else {

    /* Cannot scatter the workload between a high number of processors with a matrix
     * size relative small */
    if (rank-1 >= size) {
      printf(" # %s %d # NOT NECESSARY\n",LABEL_SLAVE, rank);
      no_need = 1;
      MPI_Barrier(MPI_COMM_WORLD);

    }

    if (no_need != 1){
      /* Receives the second matrix */
      second = (int **)malloc(sizeof(int)*size);
      for (i = 0; i < size; i++)
      {
        second[i] = (int *)malloc(sizeof(int)*size);
        MPI_Bcast(&(second[i][0]), size, MPI_INT, RANK_MASTER, MPI_COMM_WORLD);
      }

      /* Each slave receives (rank-1)*(size/n_process)+1 to i*(size/n_process)
       * rows of the first matrix to calculate that many rows of the product matrix */
      row_first = (int *)malloc(sizeof(int)*size);
      result = (int **)malloc(sizeof(int)*size);
      rows_per_process = (size < (n_process-1)) ? 1 : (size / (n_process-1));

      for (rows_index = 0; rows_index < rows_per_process ;rows_index++){
        MPI_Recv(row_first,size,MPI_INT,RANK_MASTER,TAG_ROWS,MPI_COMM_WORLD,&status);
        /* Calculation of the result row */
        result[rows_index] = (int *)malloc(sizeof(int)*size);
        multiply_row(result[rows_index], row_first, second, size);
        MPI_Send(&(result[rows_index][0]),size,MPI_INT,RANK_MASTER,TAG_RESULT,MPI_COMM_WORLD);
      }

      /* If there are remaining rows to finish (never more than N-1) each slave
       * calculates the remaining row according to their rank */
      remaining_rows = (size < (n_process-1)) ? 0 : size % (n_process-1);
      last_rows_index = rows_index; /* This is the corresponding index of the remaining row */

      /* Only the first processes ought to deal with the remaining rows */
      if ((remaining_rows >= rank) && (remaining_rows != 0)) {
        MPI_Recv(row_first,size,MPI_INT,RANK_MASTER,TAG_REMAINING_ROWS,MPI_COMM_WORLD,&status);
        result[last_rows_index] = (int *)malloc(sizeof(int)*size);
        multiply_row(result[last_rows_index], row_first, second, size);
        MPI_Send(&(result[last_rows_index][0]),size,MPI_INT,RANK_MASTER,TAG_RESULT,MPI_COMM_WORLD);
      }

      free(second);
      free(row_first);
      free(result);

      MPI_Barrier(MPI_COMM_WORLD);
    }
  }
  
  /* All done */  

  MPI_Finalize();

  return 0;  
}
