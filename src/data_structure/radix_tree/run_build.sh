#!/bin/sh

CLEAN_BUILD=false
PRESET_NAME="configure-debug"
BUILD_PRESET="build-debug"
TEST_PRESET=""
RUN_FORMAT=false
VERBOSE_MODE=false

for arg in "$@"; do
  case "$arg" in
    clean)
      CLEAN_BUILD=true
      ;;
    release)
      PRESET_NAME="configure-release"
      BUILD_PRESET="build-release"
      ;;
    test)
      PRESET_NAME="configure-test"
      BUILD_PRESET="build-unit-tests"
      TEST_PRESET="run-unit-tests"
      ;;
    format)
      RUN_FORMAT=true
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
      echo "  format           Run clang-format on source files"
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

echo "[INFO] Configure with preset: $PRESET_NAME"
cmake --preset "$PRESET_NAME"
echo

if [ "$RUN_FORMAT" = true ]; then
  echo "[INFO] Running clang-format on source files"
  cmake --build --preset "$BUILD_PRESET" --target format
  echo
fi

echo "[INFO] Build with preset: $BUILD_PRESET"
if [ "$VERBOSE_MODE" = true ]; then
  cmake --build --preset "$BUILD_PRESET" --verbose
else
  cmake --build --preset "$BUILD_PRESET"
fi
echo

if [ -n "$TEST_PRESET" ]; then
  echo "[INFO] Run tests with preset: $TEST_PRESET"
  if [ "$VERBOSE_MODE" = true ]; then
    ctest --preset "$TEST_PRESET" --verbose
  else
    ctest --preset "$TEST_PRESET"
  fi
  echo
fi
