#!/bin/bash

# This test is for Intel DC PMM benchmarks

dtm=$(date +"%F_%H-%M")
prefix=aep_poly
resdir="res_$dtm"

mkdir -p "$resdir"

conffile="$resdir/config.txt"
touch "$conffile"


export MALLOC_TOP_PAD_=$((16777216))
export MALLOC_TRIM_THRESHOLD_=$((16777216))


echo " --- hostname ---" >> $conffile
hostname >> $conffile

echo " --- free -g ---" >> $conffile
free -g >> $conffile

echo " --- numactl -H ---" >> $conffile
numactl -H >> $conffile

echo " --- imdtversion -vvv ---" >> $conffile
imdtversion -vvv >> $conffile

echo " --- uname -a ---" >> $conffile
uname -a >> $conffile

echo " --- lscpu ---" >> $conffile
lscpu >> $conffile

echo " --- ENV ---" >> $conffile
env >> $conffile

echo " --- running script ---" >> $conffile
cat $0 >> $conffile


export OMP_PLACES=cores
export OMP_PROC_BIND=true

for nthreads in 56 112
do
    export OMP_NUM_THREADS=$nthreads

    for gigs in 50 150 450 900 1800
    do
        for polysize in 16 32 64 128 256 512 768 1024 2048 4096 8192
        do

            if (( gigs <= 100 ))
            then
                if (( polysize < 128 ))
                then
                    repeats=50
                fi
            else
                repeats=1
            fi

            for bench in "ro" "rw"
            do
                numactl --interleave=all ./poly_${bench}.x $((polysize-1)) $((gigs*1024*1024*128)) $repeats > ${resdir}/${prefix}_${bench}_${polysize}_${gigs}g_${nthreads}t.log
            done
        done
    done
done
