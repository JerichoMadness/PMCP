#!/bin/zsh

### jobname
#BSUB -J interactive

### output file
#BSUB -o testOut.%J

### time constraint
#BSUB -W 0:20

### memory constraint
#BSUB -M 2000

### which processor
###BSUB -R select"[model=<Broadwell_EP>]"

### send email when finished
#BSUB -N

cd $HOME/PMCP/
time build/out.x

