# Epoll Server

## Build

```bash
export CC=clang
export CFLAGS="-g"
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
./build/server
```

---

## Clang Format

- torvalds/linux: [.clang-format](https://github.com/torvalds/linux/blob/master/.clang-format)

