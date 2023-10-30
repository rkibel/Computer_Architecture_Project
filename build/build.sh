#!/bin/sh

.   /etc/profile
module avail
module load gcc/12.1.0
cmake --version
clang-tidy --version
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .