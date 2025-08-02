# Radix Tree

## Tools

- build: cmake 3.20+, ninja
- script: just or make
- test: cmocka

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

```bash
just debug
```

---

## Run Examples

```bash
./build/debug/examples/example_tree
```

---

## Format

```bash
just format
```

---

## Test

### Install cmocka library

```bash
sudo apt install --no-install-recommends libcmocka-dev
```

### Run test

```bash
just test
```
