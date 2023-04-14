// Problema das N Rainhas com OpenMP e OpenMPI
// 0 --> empty space | 1 ---> queen
// Introdução ao Processamento Paralelo e Distribuido (IPPD)
// Patrcik Rosa, Alejandro Alberoni, Bianca Coelho

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>

#ifdef _OPENMP
	#include <omp.h>
	#include <mpi.h>
#endif

bool check_queen(int **mat, int queens, int row, int column);
void put_queen(int **mat, int queens, int position);

int solutions = 0; // number of solutions to the problem

// Main
int main(int args, char *argv[]){
	
	if (args != 3){
		fprintf(stderr,"Not enough!\n");
	}
	
	int queens, threads;
	int rank, size; 
	int **mat;
	int i,j;
	double time;
	clock_t t;
	
	queens = atoi(argv[1]);
	threads = atoi(argv[2]);
	
	// Creates the board for the problem
	mat = malloc(queens * sizeof(int*));
	for (i=0;i < queens;i++){
		mat[i] = malloc(queens * sizeof(int));
	}
	
	for (i=0;i<queens;i++){
		for(j=0;j<queens;j++){
			mat[i][j] = 0 ;
		}
	}
	
//defining OMP-MPI or sequential
#ifdef _OPENMP
	omp_set_num_threads(threads); // set number of threads
	MPI_Init(&args,&argv); /* initialize MPI thread 
				MPI_Init() -> THREAD_SINGLE*/	
	MPI_Comm_size(MPI_COMM_WORLD, &size); /* get number of processes --
							returns the total number of processes*/
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); /* get current process id --
						 returns the rank of the calling MPI process */
#endif	
	printf("--------------------------------------------\n");
    	printf("Solving N-Queen\n");
    	printf("--------------------------------------------\n");
    	
	t = clock();
	// SOLUTION IS SIMPLE FOR N = 1
	if(queens == 1 ){
		++solutions;
		mat[0][0] = 1;
	}
	
	// DOES NOT EXIST A SOLUTION FOR N = 2 AND N = 3
	if(queens == 2 || queens == 3){
		solutions = 0;
	}
	
	// CASE N > 3, CACULATE THE NUMBER OF SOLUTIONS POSSIBLE AND PRINT BOARD
	if(queens > 3){
// Parallel region
#pragma omp parallel
{
	#pragma omp single
	{
		if (rank != 0){
			put_queen(mat,queens,0);
			MPI_Send(&solutions, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		}else{
			for(int i = 0; i < size-1; i++){
				int answer;
				MPI_Recv(&answer, 1, MPI_INT, MPI_ANY_SOURCE,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
				solutions += answer;
			}
		
		}
	}
}
}
// End of parallel region
	t = clock() - t;
	time = ((double)t)/CLOCKS_PER_SEC;
	printf("--------------------------------------------\n");
    	printf("Solved!\n");
	printf("Solutions: %d\n", solutions);
	printf("Queens : %d\n", queens);
	printf("Board size: %d\n", queens*queens);
#ifdef _OPENMP
	printf("Number of threads: %d\n", threads);
	MPI_Finalize();
#else
	printf("Number of threads: %d\n", 0);
#endif
	printf("Time(in seconds): %f\n", time);
	
	return 0;
}


// Put a queen in the board
void put_queen(int **mat, int queens, int positioned){
	int i,j;
	if (positioned == queens){
		#pragma omp critical
		{
			++solutions;
		}
	} 
	for(i = 0; i < queens; i++){	
		if (check_queen(mat, queens,positioned,i)){
			mat[positioned][i] = 1;
			#pragma omp task firstprivate(positioned) if(positioned <= (queens/2))
			{
				put_queen(mat, queens, positioned+1);
			}
			#pragma omp taskwait
			mat[positioned][i] = 0;
		}
	}
}

// Checks if there is a queen in the diagonal or in the same row or column
bool check_queen(int **mat, int queens, int pos, int column){
	int i,j;

	// column '-'
	for(i = 0; i < pos; i++){
		if(mat[i][column] == 1){
			return false;
		}
	}

	// '\'
	for(int i = pos, j = column; i >= 0 && j >= 0; i--,j--){
		if(mat[i][j] == 1){
			return false;
		}
	}
	
	// '/'
	for(int i = pos, j = column; i >= 0 && j < queens; i--,j++){
		if(mat[i][j] == 1){
			return false;
		}
	}

	return true;

}

