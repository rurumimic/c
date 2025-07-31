#!/bin/sh

OPTION=""
if [ -n "$1" ]; then
  OPTION="$1"
fi

if [ "$OPTION" = "clean" ]; then
  if [ -d "build" ]; then
    echo "[INFO] Clean build directory:"
    echo "rm -rf ./build"
    rm -rf ./build
    echo
  fi
fi

echo "[INFO] CMAKE Configure:"
cmake \
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
-DENABLE_FORMAT=ON \
-DTARGET_GROUP=test \
-S . -B build \
-G Ninja
echo

echo "[INFO] Build with Ninja:"
ninja -C build

echo "[INFO] Test with ctest:"
cd build
ctest

