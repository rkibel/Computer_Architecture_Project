#!/bin/sh

. /etc/profile
module avail
module load gcc/12.1.0
echo "* Running fluid *"

step_count=1
input_file="small.fld"
output_file="final.fld"

perf stat ./build/fluid/fluid $step_count $input_file $output_file