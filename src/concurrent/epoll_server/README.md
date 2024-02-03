# Epoll Server

## Introduction

This is a simple epoll server that can handle multiple clients concurrently.  
It is a simple implementation of a server that can handle multiple clients concurrently using epoll.

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

