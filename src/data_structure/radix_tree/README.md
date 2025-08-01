# Radix Tree

## Build

```bash
./run_build.sh clean
```

or:

```bash
rm -rf build
```

```bash
cmake \
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
-DENABLE_FORMAT=ON \
-DTARGET_GROUP=debug \
-S . -B build \
-G Ninja
```

- `-DCMAKE_EXPORT_COMPILE_COMMANDS=ON` export compile commands
- `-DENABLE_FORMAT=ON` enable format target
- `-DTARGET_GROUP=debug` build debug target
- `-DFORMAT_STYLE=Google` use Google format style
- `--debug-output` print debug output
- `-S .` source directory
- `-B build` build directory
- `-G Ninja` use Ninja generator

```bash
cmake --build build
# make -C build
# ninja -C build
```

## Run

```bash
./build/radixtree
```

## Format

### Make .clang-format

```bash
cmake --build build --target clang-format
# make -C build clang-format
# ninja -C build clang-format
```

### Format code

```bash
cmake --build build --target format
# make -C build format
# ninja -C build format
```

## Test

- docs: [install cmocka](docs/test.md)

run tests:

```bash
./run_build.sh clean test
```

or:

```bash
rm -rf build
```

```bash
cmake \
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
-DENABLE_FORMAT=ON \
-DTARGET_GROUP=test \
-S . -B build \
-G Ninja
```

```bash
ninja -C build
```

```bash
cd build/tests
ctest --output-on-failure
```

