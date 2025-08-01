#!/bin/sh

CLEAN_BUILD=false
TARGET_GROUP="debug"
VERBOSE_MODE=false

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
    -v|--verbose)
      VERBOSE_MODE=true
      ;;
    -h|--help)
      echo "Usage: $0 [OPTIONS]"
      echo
      echo "Options:"
      echo "  clean            Remove the build directory before configuring"
      echo "  release          Configure with release target group"
      echo "  test             Configure with test target group (runs ctest)"
      echo "  -v, --verbose    Enable verbose mode"
      echo "  -h, --help       Show this help message"
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
if [ "$VERBOSE_MODE" = true ]; then
  ninja -v -C build
else
  ninja -C build
fi
echo

if [ "$TARGET_GROUP" = "test" ]; then
  echo "[INFO] Run CTest:"
  cd build/tests
  if [ "$VERBOSE_MODE" = true ]; then
    ctest --output-on-failure --verbose
  else
    ctest --output-on-failure
  fi
  echo
fi

