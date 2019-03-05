#!/bin/zsh

### jobname
#BSUB -J ChainJob[1-50]%1
###BSUB -J job

### output file
#BSUB -o testOut.%J

### time constraint
#BSUB -W 1:00

### memory constraint
#BSUB -M 2000

### Use node exclusively
#BSUB -x

### Use only one node
#BSUB -n 1

### Use AICES cluster
#BSUB -P aices2

### which processor
#BSUB -R model==Haswell_EP

### send email when finished
#BSUB -N

cd $HOME/PMCP/
taskset -c 1 build/out.x

