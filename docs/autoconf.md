# Autoconf

- gnu.org: [autoconf](https://www.gnu.org/software/autoconf/)
  - [manual](https://www.gnu.org/software/autoconf/manual/index.html)

## Commands

### autoreconf

```bash
autoreconf --install
```

or:

```bash
aclocal
autoconf
automake --add-missing
```

### build

```bash
mkdir build
cd build
```

```bash
../configure
make
```

