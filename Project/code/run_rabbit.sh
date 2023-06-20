#!/usr/bin/env bash

# If project not ready, generate cmake file.
if [[ ! -d build ]]; then
    echo "good"
else
    rm -rf build
fi
cmake -B build
cmake --build build

ROUNDS=100
MAX_DEPTH=20
STEP=10

# Run all testcases. 
# You can comment some lines to disable the run of specific examples.
mkdir -p output
# build/PT testcases/smallpt.txt output/smallpt.bmp
# build/PT testcases/scene01_basic.txt output/scene01.bmp
# build/PT testcases/scene02_cube.txt output/scene02.bmp
# build/PA1 testcases/scene03_sphere.txt output/scene03.bmp
# build/PA1 testcases/scene04_axes.txt output/scene04.bmp
# build/PT testcases/scene05_bunny_200.txt output/scene05.bmp
# build/PT testcases/scene06_bunny_1k.txt output/scene06.bmp
build/PT testcases/scene06_bunny_1k_vn.txt output/scene06_vn.bmp $ROUNDS $MAX_DEPTH $STEP
# build/PT testcases/ball.txt output/ball.bmp
# build/PT testcases/ball_move.txt output/ball_move.bmp $ROUNDS $MAX_DEPTH
# build/PA1 testcases/scene07_shine.txt output/scene07.bmp
