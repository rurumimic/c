# Futures

## Build

```bash
source BUILD
```

### Manual Build

```bash
autoreconf -i
```

```bash
mkdir build
cd build
```

```bash
../configure
```

```bash
make
# or
bear -- make
```

### Clean Build

```bash
cd build
make clean
make distclean
```

## Run

```bash
./build/futures
```

```txt
[Hello] Pending: Hello
[Goodbye] Ready
[Hello] Pending: Hello, World
[Goodbye] Ready
[Hello] READY: Hello, World!!!
[Goodbye] Ready
[Main] Goodbye.
```

---

## Clang Format

- torvalds/linux: [.clang-format](https://github.com/torvalds/linux/blob/master/.clang-format)

