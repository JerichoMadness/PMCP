#!/bin/zsh

### jobname
###BSUB -J ChainJob[1-50]%1
#BSUB -J job

### output file
#BSUB -o testOut.%J

### time constraint
#BSUB -W 0:20

### memory constraint
#BSUB -M 2000

### Use node exclusively
#BSUB -x

### Use only one node
#BSUB n 1

### which processor
###BSUB -R select"[model=<Broadwell_EP>]"

### send email when finished
#BSUB -N

cd $HOME/PMCP/
time build/out.x

