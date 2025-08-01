#!/bin/sh

CLEAN_BUILD=false
TARGET_GROUP="debug"

for arg in "$@"; do
  case "$arg" in
    clean)
      CLEAN_BUILD=true
      ;;
    release)
      TARGET_GROUP="release"
      ;;
    test)
      TARGET_GROUP="test"
      ;;
    -h|--help)
      echo "Usage: $0 [OPTIONS]"
      echo
      echo "Options:"
      echo "  clean        Remove the build directory before configuring"
      echo "  release      Configure with release target group"
      echo "  test         Configure with test target group (runs ctest)"
      echo "  -h, --help   Show this help message"
      exit 0
      ;;
    *)
      echo "[WARN] Unknown option: $arg"
      ;;
  esac
done

if [ "$CLEAN_BUILD" = true ]; then
  if [ -d "build" ]; then
    echo "[INFO] Clean build directory:"
    echo "rm -rf ./build"
    rm -rf ./build
    echo
  fi
fi

echo "[INFO] CMAKE Configure: (TARGET_GROUP=$TARGET_GROUP)"
cmake \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DENABLE_FORMAT=ON \
  -DTARGET_GROUP=$TARGET_GROUP \
  -S . -B build \
  -G Ninja
echo

echo "[INFO] Build with Ninja:"
ninja -C build
echo

if [ "$TARGET_GROUP" = "test" ]; then
  echo "[INFO] Run CTest:"
  cd build/tests && ctest --output-on-failure
  echo
fi

