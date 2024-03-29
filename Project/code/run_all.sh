#!/usr/bin/env bash

# If project not ready, generate cmake file.
if [[ ! -d build ]]; then
    echo "good"
else
    rm -rf build
fi
cmake -B build
cmake --build build

ROUNDS=5
MAX_DEPTH=5
STEP=2

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
# build/PT testcases/scene06_bunny_1k_vn.txt output/scene06_vn.bmp
# build/PT testcases/ball.txt output/ball.bmp $ROUNDS $MAX_DEPTH $STEP
# build/PT testcases/ball_checker.txt output/ball_checker.bmp $ROUNDS $MAX_DEPTH $STEP
# build/PT testcases/ball_noise.txt output/ball_noise.bmp $ROUNDS $MAX_DEPTH $STEP
# build/PT testcases/ball_move.txt output/ball_move.bmp $ROUNDS $MAX_DEPTH $STEP
# build/PA1 testcases/scene07_shine.txt output/scene07.bmp
# build/PT testcases/scene18_dof.txt output/scene18.bmp $ROUNDS $MAX_DEPTH $STEP
# build/PT testcases/curve_bezier.txt output/bezier.bmp $ROUNDS $MAX_DEPTH $STEP
# build/PT testcases/scene09_norm.txt output/scene09_norm.bmp $ROUNDS $MAX_DEPTH $STEP
# build/PT testcases/balls.txt output/balls.bmp $ROUNDS $MAX_DEPTH $STEP
# build/PT testcases/box.txt output/box.bmp $ROUNDS $MAX_DEPTH $STEP
# build/PT testcases/box_media.txt output/box_media.bmp $ROUNDS $MAX_DEPTH $STEP
# build/PT testcases/scene10_wineglass_720p.txt output/scene10.bmp $ROUNDS $MAX_DEPTH $STEP
# build/PT testcases/1.txt output/1_1.bmp $ROUNDS $MAX_DEPTH $STEP
# build/PT testcases/2.txt output/2_1.bmp $ROUNDS $MAX_DEPTH $STEP
# build/PT testcases/5.txt output/5_1.bmp $ROUNDS $MAX_DEPTH $STEP
# build/PT testcases/6.txt output/6_1.bmp $ROUNDS $MAX_DEPTH $STEP
# build/PT testcases/9.txt output/9_1.bmp $ROUNDS $MAX_DEPTH $STEP
# build/PT testcases/10.txt output/10_1.bmp $ROUNDS $MAX_DEPTH $STEP
build/PT testcases/final.txt output/final.bmp $ROUNDS $MAX_DEPTH $STEP