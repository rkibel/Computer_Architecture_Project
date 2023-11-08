#!/bin/sh

. /etc/profile
module avail
module load gcc/12.1.0
echo "* Running fluid *"
perf stat -r 5 ./build/fluid/fluid $@

# to run properly on avignon, run in the following way:
# sbatch run.sh step_number input_file output_file

# example: 
# sbatch run.sh 1 reference-input/small.fld final.fld

# you can also test with as many arguments as you want