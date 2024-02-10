# State Machine with coroutines

## Build

```bash
source BUILD
```

### Manual Build

```bash
autoreconf -i
mkdir build
cd build
../configure
make
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
make
./server
```

### Client

```bash
telnet localhost 10000
```

---

## Clang Format

- torvalds/linux: [.clang-format](https://github.com/torvalds/linux/blob/master/.clang-format)

