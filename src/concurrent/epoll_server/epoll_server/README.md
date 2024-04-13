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

### Server

```bash
./build/server
```

```bash
Welcome to the Echo Server.
Usage: telnet localhost 10000
Press Ctrl+C to stop the Server.
Accept (6): 127.0.0.1
Echo (6): Hello?
Accept (7): 127.0.0.1
Echo (7): Good evening!
Echo (7): Goodbye.
Close (7)
Echo (6): Bye.
Close (6)
^C[SIG: 2]
Close: io_selector
Close: executor
Goodbye.
```

### Client

```bash
telnet localhost 10000
```

```bash
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
Hello?
Hello?
Bye.
Bye.
^]
telnet> quit
Connection closed.
```

---

## Clang Format

- torvalds/linux: [.clang-format](https://github.com/torvalds/linux/blob/master/.clang-format)

```bash
find . -iname '*.h' -o -iname '*.c' | xargs clang-format -i
```

