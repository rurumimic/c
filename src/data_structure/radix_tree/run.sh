#!/bin/sh

cmake \
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
-DENABLE_FORMAT=ON \
-S . -B build \
-G Ninja

ninja -C build

./build/main

