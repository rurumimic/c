# A simple blocking HTTP client

## Build and Run

Build:

```bash
CC=gcc make
bear -- make
```

Run:

```bash
./main
```

## Format

```bash
clang-format -style=llvm -dump-config > .clang-format
find . -iname '*.h' -o -iname '*.c' | xargs clang-format -i
```

