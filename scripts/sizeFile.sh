#!/usr/bin/env bash

if [ "$#" -ne 4 ]; then
    echo "Wrong number of arguments!"
fi

IT=1

MIN=$1
MAX=$2
LENGTH=$(($3 + $IT))
N=$4 

i=1
j=1

while [[ $i -le $N ]]
do
    file="sizes${i}.txt"
    touch "${file}"

    j=1

    while [[ $j -le $LENGTH ]]
    do
        while :; do rand=$RANDOM; ((rand < 32760)) &&  rand=$(((rand%${MAX})+${MIN})) && break; done 
        echo "$rand" >> $file
        ((j = j+1)) 
    done
       
    ((i = i+1))
done
