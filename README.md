# NQueens OMP-MPI

N Queens puzzle implemented in C with OpenMP and OpenMPI.

# NQueens Puzzle

The N queens puzzle is the problem of placing N queens in a N X N chess board such that no queen can be attacked by another queen in any possible direction(vertically, horizontally and diagonally).

# Compile and run

Compile:
 
 > mpicc -o rainhasMPI -fopenmp rainhasMPI.c -lpthread
 
Run:
 
 > mpirun -np numberOfProcessors ./rainhasMPI numberOfQueens numberOfThreads
 
# Contributor(s)
 
Bianca Nunes Coelho
 
Alejandro Tomas Reyes Alberoni
 
Patrick Silva da Rosa
