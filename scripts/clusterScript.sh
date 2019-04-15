#!/usr/bin/env bash

### jobname
###BSUB -J ChainJob[1-20]%1
#BSUB -J job

### output file
#BSUB -o testOut.%J

### error output file
###BSUB -e error.%J

### time constraint
#BSUB -W 1:00

### memory constraint
#BSUB -M 2000

### Use node exclusively
#BSUB -x

### Use only one node
#BSUB -n 24

### Use AICES cluster
#BSUB -P aices2

### Use this specific node
#BSUB -m linuxihdc072

### which processor
#BSUB -R model==Haswell_EP

### send email when finished
#BSUB -N

###Use OpenMP
#BSUB -a openmp

    export MKL_NUM_THREADS="1"
    #export MKL_NUM_THREADS_LOCAL="1"
    export OMP_MAX_ACTIVE_LEVELS="2" # set the maximum number of nested active parallel regions on the device.
    export OMP_NESTED="TRUE"         # enable nested parallelism on the device
    export OMP_NUM_THREADS=1,1       # Set the number of OpenMP threads in each specific level
    export OMP_DYNAMIC="false"       # disable OpenMP from dynamically being able to adjust the number of threads
    export MKL_DYNAMIC="false"       # disable MKL from dynamically being able to adjust the number of threads
    export KMP_AFFINITY="compact,verbose"      # set affinity for MKL threads (relative to the thread that launches the gemm)

cd $HOME/PMCP/
build/out.x input/sizes1.txt

