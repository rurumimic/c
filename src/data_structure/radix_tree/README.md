# Radix Tree

## Tools

- build: cmake 3.20+, ninja
- test: cmocka

## Build

```bash
make
```

or:

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
cmake --build --preset build-debug
# cmake --build build
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
make clang-format
# cmake --build build --target clang-format
# make -C build clang-format
# ninja -C build clang-format
```

### Format code

```bash
make format
# cmake --build build --target format
# make -C build format
# ninja -C build format
```

## Test

- install: [cmocka](docs/test.md)

```bash
make test
```
