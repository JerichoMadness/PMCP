#!/usr/local_rwth/bin/zsh

#SBATCH --get-user-env 
#SBATCH --job-name=GEMM_BENCH_JOB
#SBATCH --nodes=1
#SBATCH --cpus-per-task=24
#SBATCH --ntasks=1
##SBATCH --mem-per-cpu=7G
#SBATCH --mem=50GB
#SBATCH --output=output-%j.txt
#SBATCH --error=error-%j.txt
#SBATCH --time=2:00:00
#SBATCH --partition=ih
#SBATCH --exclusive
#SBATCH --account=aices2
#SBATCH --mail-type=end
#SBATCH --mail-user=alexander.reeh@rwth-aachen.de
#SBATCH --nodelist=linuxihdc072

module purge
module load DEVELOP
module load intel/18.0
module load gcc/8

export OMP_NUM_THREADS="8"
#export MKL_NUM_THREAD="8"
export OMP_NESTED="TRUE"
#export OMP_MAX_ACTIVE_LEVELS=2
export MKL_DYNAMIC="false"
export OMP_DYNAMIC="false"
export KMP_AFFINITY="verbose"


cd $HOME/PMCP/
numactl -C 0,2,4,6,8,10,12,14 build/out.x
