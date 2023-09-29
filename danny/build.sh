#!/bin/sh
cmake -S mandel -B mandel-debug -DCMAKE_BUILD_TYPE=Debug
cmake -S mandel -B mandel-release -DCMAKE_BUILD_TYPE=Release
cmake --build mandel-debug
cmake --build mandel-release
