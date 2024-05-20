# Valgrind

- [valgrind.org](https://valgrind.org/)
- [docs](https://valgrind.org/docs/manual/index.html)
  - [quick start](https://valgrind.org/docs/manual/quick-start.html)
  - [faq](https://valgrind.org/docs/manual/faq.html)
  - [manual](https://valgrind.org/docs/manual/manual.html)

## Install

- ubuntu
  - wiki: [valgrind](https://wiki.ubuntu.com/Valgrind)

```bash
sudo apt-get install valgrind
```

```bash
valgrind --version
```

## Quick Start

### Compile

```bash
gcc -g -O0 -Wall -Wextra -o main main.c
clang -gdwarf-2 -O0 -Wall -Wextra -o main main.c
```

#### avoid warnings

add `-gdwarf-2` to avoid the following warnings

```log
### unhandled dwarf2 abbrev form code 0x25
### unhandled dwarf2 abbrev form code 0x1b
get_Form_szB: unhandled 27 (DW_FORM_addrx)
```

### Run

```bash
./main arg1 arg2
```

### with Valgrind

```bash
valgrind --leak-check=yes ./main arg1 arg2
```

#### Result

```log
==29844== Memcheck, a memory error detector
==29844== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==29844== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==29844== Command: ./main arg1 arg2
==29844==
==29844== Invalid write of size 4
==29844==    at 0x10915A: f (main.c:6)
==29844==    by 0x10918A: main (main.c:11)
==29844==  Address 0x4aa2068 is 0 bytes after a block of size 40 alloc'd
==29844==    at 0x4848899: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==29844==    by 0x109151: f (main.c:5)
==29844==    by 0x10918A: main (main.c:11)
==29844==
==29844==
==29844== HEAP SUMMARY:
==29844==     in use at exit: 40 bytes in 1 blocks
==29844==   total heap usage: 1 allocs, 0 frees, 40 bytes allocated
==29844==
==29844== 40 bytes in 1 blocks are definitely lost in loss record 1 of 1
==29844==    at 0x4848899: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==29844==    by 0x109151: f (main.c:5)
==29844==    by 0x10918A: main (main.c:11)
==29844==
==29844== LEAK SUMMARY:
==29844==    definitely lost: 40 bytes in 1 blocks
==29844==    indirectly lost: 0 bytes in 0 blocks
==29844==      possibly lost: 0 bytes in 0 blocks
==29844==    still reachable: 0 bytes in 0 blocks
==29844==         suppressed: 0 bytes in 0 blocks
==29844==
==29844== For lists of detected and suppressed errors, rerun with: -s
==29844== ERROR SUMMARY: 2 errors from 2 contexts (suppressed: 0 from 0)
```

