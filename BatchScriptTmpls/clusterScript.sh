#!/bin/zsh

### jobname
#BSUB -J Test

### output file
#BSUB -o testOut.%J

### time constraint
#BSUB -W 0:05

### memory constraint
#BSUB -M 2048

### send email when finished
#BSUB -N

### exclusive node
#BSUB -x

### openmp
#BSUB -a openmp

### number of cores
#BSUB -n 1

### mpi-s node
#BSUB -m mpi-s

###export OMP_NUM_THREADS=1
###export MKL_NUM_THREADS=$OMP_NUM_THREADS

cd $HOME/HPMC/MatrixChainProduct/Code/dynamicRegular/
time ./main.x

