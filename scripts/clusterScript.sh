#!/usr/bin/env zsh

### Name of the job
#SBATCH --job-name=job

### Name of the ouztput file
#SBATCH --output=../output/out.%J.txt

### Time your job needs to execute, e. g. 30 min
#SBATCH --time=00:30:00

### Memory your job needs per node, e. g. 500 MB
#SBATCH --mem=2000M

### Use nodes exclusively
#SBATCH --exclusive

### Use N nodes
#SBATCH --nodes=24

### Use AICES or AICES2 cluster
###SBATCH --clusters=aices2

### Use this specific node
#SBATCH --nodelist=linuxihdc072

### send email when done
#SBATCH --mail-type=end 
#SBATCH --mail-user=alexander.reeh@rwth-aachen.de 

    export MKL_NUM_THREADS="2"
    export OMP_MAX_ACTIVE_LEVELS="2" # set the maximum number of nested active parallel regions on the device.
    export OMP_NESTED="TRUE"         # enable nested parallelism on the device
    export OMP_NUM_THREADS=2,4       # Set the number of OpenMP threads in each specific level
    export OMP_PROC_BIND="spread, close"
    export OMP_PLACES="{1,2,3,4},{5,6,7,8}"
    export OMP_DYNAMIC="false"       # disable OpenMP from dynamically being able to adjust the number of threads
    export MKL_DYNAMIC="false"       # disable MKL from dynamically being able to adjust the number of threads
    export KMP_AFFINITY="verbose"      # set affinity for MKL threads (relative to the thread that launches the gemm)

cd $HOME/PMCP/
build/out.x input/sizes4N.txt

