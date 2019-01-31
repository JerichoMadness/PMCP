#!/usr/bin/env bash


level=${1?Error: no level given}

if [ $level -eq 1 ] ; then
    size="$(cat /sys/devices/system/cpu/cpu0/cache/index1/size)"
elif [ $level -eq 2 ] ; then
    size="$(cat /sys/devices/system/cpu/cpu0/cache/index2/size)"
elif [ $level -eq 3 ] ; then
    size="$(cat /sys/devices/system/cpu/cpu0/cache/index3/size)"
else
    echo "Incorrect paramater input!"
fi

size=$(sed "s/K//g" <<< $size)

echo "${size}"
