#!/usr/bin/env bash


level=${1?Error: no level given}

touch lvl.csv

file="lvl.csv"

if [ $level -eq 1 ] ; then
    cat /sys/devices/system/cpu/cpu0/cache/index1/size | tee $file
elif [ $level -eq 2 ] ; then
    cat /sys/devices/system/cpu/cpu0/cache/index2/size | tee $file
elif [ $level -eq 3 ] ; then
    cat /sys/devices/system/cpu/cpu0/cache/index3/size | tee $file
else
    echo "Incorrect paramater input!"
fi

sed -i 's/K//' $file

if grep -q 'K' "$file"; then
    echo "File conventions not correct!"
fi
