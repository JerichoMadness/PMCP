#!/bin/zsh

### jobname
###BSUB -J ChainJob[1-20]%1
#BSUB -J job

### output file
#BSUB -o testOut.%J

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

cd $HOME/PMCP/test/GEMMTest/
out.x

