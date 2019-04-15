#!/usr/bin/env bash

    export OMP_MAX_ACTIVE_LEVELS="2" # set the maximum number of nested active parallel regions on the device.
    export MKL_NUM_THREADS="4"     # set number of MKL Threads
    export OMP_NESTED="true"         # enable nested parallelism on the device
    export OMP_NUM_THREADS=2,4       # Set the number of OpenMP threads for each speific level
    #export OMP_PROC_BIND="spread, close"
    #export OMP_PLACES="{1,2,3,4},{5,6,7,8}"
    export OMP_DYNAMIC="false"       # disable OpenMP from dynamically being able to adjust the number of threads
    export MKL_DYNAMIC="false"       # disable MKL from dynamically being able to adjust the number of threads
    export KMP_AFFINITY="verbose"      # set affinity for MKL threads (relative to the thread that launches the gemm)


#Compile
echo "Compiling..."
make

#Execute file
echo "Running code..."

build/out.x input/sizesN4.txt

