#!/bin/sh

. /etc/profile
module avail
module load gcc/12.1.0
echo "* Running fluid *"
perf stat ./build/fluid/fluid 1 small.fld small-1-test.fld