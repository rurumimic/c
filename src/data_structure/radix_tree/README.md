# Radix Tree

A simple radix tree implementation for single-threaded use.

Documentation:

- [Radix Tree Node Structure](docs/README.md)

---

## Tools

- build: cmake 3.20+, ninja
- script: just or make
- test: cmocka
- valgrind

### Justfile

- github: [casey/just](https://github.com/casey/just)

```bash
cargo install just
```

Available recipes:

```bash
just --list
```

---

## Build

build library + examples:

```bash
just
# or
just debug
```

build library:

```bash
just release
```

---

## Run Examples

```bash
./build/debug/examples/example_tree
```

### valgrind

```bash
valgrind --show-error-list=yes --leak-check=yes \
./build/debug/examples/tree/example_tree
```

---

## Format with .clang-format

```bash
just format
```

---

## Test

### Install cmocka library

- [cmocka](https://cmocka.org/)

```bash
sudo apt install --no-install-recommends libcmocka-dev
```

### Run test

```bash
just test
```
