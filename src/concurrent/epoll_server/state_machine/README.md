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

```bash
make
./state_machine
```

---

## Clang Format

- torvalds/linux: [.clang-format](https://github.com/torvalds/linux/blob/master/.clang-format)

