# Forking ROT13 server

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

Client:

```bash
telnet localhost 40713

Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
Hello, World!
Uryyb, Jbeyq!
Bye.
Olr.
^]
telnet> quit
Connection closed.
```

## Format

```bash
clang-format -style=llvm -dump-config > .clang-format
find . -iname '*.h' -o -iname '*.c' | xargs clang-format -i
```

