#!/bin/sh

. /etc/profile
module avail
module load gcc/12.1.0
echo "* Running fluid *"
./build/ftest/ftest