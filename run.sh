#!/usr/bin/env bash

    export OMP_MAX_ACTIVE_LEVELS="2" # set the maximum number of nested active parallel regions on the device.
    export OMP_NESTED="true"         # enable nested parallelism on the device
    export OMP_NUM_THREADS=1,5       # Set the number of OpenMP threads for the first level to 2 and the second level to 5
    export OMP_DYNAMIC="false"       # disable OpenMP from dynamically being able to adjust the number of threads
    export MKL_DYNAMIC="false"       # disable MKL from dynamically being able to adjust the number of threads
    export KMP_AFFINITY="compact,verbose"      # set affinity for MKL threads (relative to the thread that launches the gemm)


#Compile
echo "Compiling..."
make

#Execute file
echo "Running code..."

build/out.x scripts/sizes1.txt

