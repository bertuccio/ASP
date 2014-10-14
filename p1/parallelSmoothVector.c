#include <stdlib.h>  
#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <string.h>

#define RANK_MASTER 0
#define MIN_SLAVE_PROCESS 4
#define VIRTUAL_ELEMENTS 4
#define NUM_ARG 2
#define MIN_VIRTUAL_SIZE_SUBVECTOR 5
#define FIRST_INDEX 2
#define TAG_SIZE 1
#define TAG_SUB_VECTOR 2
#define TAG_SMOOTHED 3

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

int smooth_vector(int * vector, int virtual_size)
{
	int i;
	int real_size;

	if (virtual_size < MIN_VIRTUAL_SIZE_SUBVECTOR)
		return -1;

	real_size = virtual_size - VIRTUAL_ELEMENTS;

	for(i=2; i<real_size+2 ;i++){
		vector[i] = (vector[i]+vector[i-2]+vector[i-1]+vector[i+1]+vector[i+2])/5; /* Cada operacion es de 5 operandos */
	}


	return 0;
}

int main(int argc, char **argv)
{
	int rank, n_process;  

	int *vector=NULL;
	int *subvector=NULL;
	int *subvectorSlave=NULL;
	int i, n, j;
	int interval;
	int l,m;
	MPI_Status status;
	int size_vector, n_process_working, size_subvector;

	srand(time(NULL));

	if(argc!=2){
		printf("USAGE:\n %s log2(size_vector)\n", argv[0]);
		return -1;
	}

	if ((n = atoi(argv[1])) < 2){
		printf("(ERROR\t) Vector size must be greater than 2^2\n");
		return -1;
	}
	
	size_vector = pow(2,n);

	/* Start up MPI */  

	MPI_Init(&argc, &argv);  

	/* Get some info about MPI */  

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
	MPI_Comm_size(MPI_COMM_WORLD, &n_process);  

	/*If the number of slave process is smaller than MIN_SLAVE_PROCESS(4)*/
	if(n_process-1 < MIN_SLAVE_PROCESS){
		printf("(ERROR\t) Needed at least four slave processes\n");
		MPI_Finalize();
		return -1;
	}

	n_process--;

	if(rank == RANK_MASTER){

		if ((vector = (int *)calloc(size_vector+VIRTUAL_ELEMENTS, sizeof(int))) == NULL) {
			perror("calloc vector");
			MPI_Finalize();
			return -1;
		}

		printf("# MASTER # VECTOR = ");
		for(i=FIRST_INDEX; i < size_vector+2 ;i++){
			vector[i] = aleat_num(1,size_vector);
			printf("%d ",vector[i]);
		}

		if((size_vector % n_process)!=0){

			m = log2(n_process);
			while(m > n)
				m = log2(m);
		
	
			n_process_working = pow(2,m);
			size_subvector = pow(2,n-m);
	
		}
		else{
			n_process_working = n_process;
			size_subvector = size_vector/n_process;
		}

		/* Broadcast size of the subvector */
		MPI_Bcast(&size_subvector, 1, MPI_INT, RANK_MASTER, MPI_COMM_WORLD);

		/* Broadcasts the number of processes working */
		MPI_Bcast(&n_process_working, 1, MPI_INT, RANK_MASTER, MPI_COMM_WORLD);

		/* Calculation and sending of the subvectors to each process */
		l=0;
		m=size_subvector-1;
		if ((subvector = (int *)malloc((size_subvector+VIRTUAL_ELEMENTS)*sizeof(int))) == NULL){
			perror("malloc subvector");
			MPI_Finalize();
			return -1;
		}

		for(i=1; i <= n_process_working ;i++){
			memcpy(
				subvector, 
				&vector[l], 
				(size_subvector+VIRTUAL_ELEMENTS)*sizeof(int));

			printf("\n# MASTER # SLAVE %d SUB_VECTOR = ", i);
			for(j=0; j<size_subvector+VIRTUAL_ELEMENTS; j++){
				printf("%d ",subvector[j]);
			}

			l = m+1;
			m += size_subvector;

			/* And then the subvector */
			MPI_Send(
				subvector,
				size_subvector+VIRTUAL_ELEMENTS,
				MPI_INT,
				i,
				TAG_SUB_VECTOR,
				MPI_COMM_WORLD);
		}

		/* Master receives all the subvectors and compounds then into the resultant vector */
		for (i = 1; i <= n_process_working; i++)
		{
			MPI_Recv(
				&(vector[FIRST_INDEX + ((i-1) * size_subvector)]),
				size_subvector, 
				MPI_INT, 
				i,
				TAG_SMOOTHED,
				MPI_COMM_WORLD,
				&status);
		}


		printf("\n# MASTER # SMOOTHED_SUB_VECTOR = ");
		for(i=FIRST_INDEX; i < size_vector+2; i++){
			printf("%d ",vector[i]);
		}

		printf("\n");
		
		free(vector);
		free(subvector);

		MPI_Barrier(MPI_COMM_WORLD);
	}
	else{

		MPI_Bcast(&size_subvector, 1, MPI_INT, RANK_MASTER, MPI_COMM_WORLD);

		MPI_Bcast(&n_process_working, 1, MPI_INT, RANK_MASTER, MPI_COMM_WORLD);

		if (rank <= n_process_working) {

			subvectorSlave = (int *)malloc((size_subvector+VIRTUAL_ELEMENTS)*sizeof(int));
			
			MPI_Recv(
				subvectorSlave,
				size_subvector+VIRTUAL_ELEMENTS, 
				MPI_INT, 
				RANK_MASTER, 
				TAG_SUB_VECTOR,
				MPI_COMM_WORLD,
				&status);

			/*
			printf("\n# SLAVE %d # SUB_VECTOR = ", rank);
			for(j=0; j<size_subvector+VIRTUAL_ELEMENTS; j++){
				printf("%d ",subvectorSlave[j]);
			}
			*/
			smooth_vector(subvectorSlave,size_subvector+VIRTUAL_ELEMENTS);
			/*
			printf("\n# SLAVE %d # SMOOTHED = ", rank);
			for(j=0; j<size_subvector+VIRTUAL_ELEMENTS; j++){
				printf("%d ",subvectorSlave[j]);
			}
			*/

			MPI_Send(
				&(subvectorSlave[FIRST_INDEX]),
				size_subvector,
				MPI_INT,
				RANK_MASTER,
				TAG_SMOOTHED,
				MPI_COMM_WORLD);

			free(subvector);
		}

		MPI_Barrier(MPI_COMM_WORLD);
	}

	/* All done */  

	MPI_Finalize();

	return 0;  
}