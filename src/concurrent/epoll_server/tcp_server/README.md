# TCP Server

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

### Server

```bash
./build/server
```

### Client

```bash
telnet localhost 9000
```

---

## Clang Format

- torvalds/linux: [.clang-format](https://github.com/torvalds/linux/blob/master/.clang-format)

