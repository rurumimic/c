# Non-Block Echo Server

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
./build/echo_server
```

```bash
Connected: (1/3)
Bye.
Connected: (2/3)
Bye.
Connected: (3/3)
Bye.
```

### Client

```bash
telnet localhost 9000
```

```bash
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
Hello, Server
Echo (3/3): Hello, Server

Connection closed by foreign host.
```

---

## Clang Format

- torvalds/linux: [.clang-format](https://github.com/torvalds/linux/blob/master/.clang-format)

