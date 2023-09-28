#!/bin/sh
cmake -S fluid -B fluid-debug -DCMAKE_BUILD_TYPE=Debug
cmake -S fluid -B fluid-release -DCMAKE_BUILD_TYPE=Release
cmake --build fluid-debug
cmake --build fluid-release