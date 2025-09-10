# Examples

## Tree

```bash
just
./build/debug/examples/tree/example_tree
```

### valgrind

```bash
valgrind --show-error-list=yes --leak-check=yes \
./build/debug/examples/tree/example_tree
```

### Run

```bash
==278879== Memcheck, a memory error detector
==278879== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
==278879== Using Valgrind-3.22.0 and LibVEX; rerun with -h for copyright info
==278879== Command: ./build/debug/examples/tree/example_tree
==278879==

[insert]
insert  key=0x4a872e0 -> OK (size=16)
insert  key=0x4a88800 -> OK (size=32)
insert  key=0x4a88ab0 -> OK (size=48)
insert  key=0x4a88b20 -> OK (size=64)
insert  key=0x4a88ba0 -> OK (size=128)

[replace]
insert  key=0x4a88800 -> REPLACED (size=320, expected REPLACED)
verify  key=0x4a88800 current_value=320

[search]
search  key=0x4a872e0 -> OK (value=16)
search  key=0x4a88ab0 -> OK (value=48)
search  key=0x4a88ba0 -> OK (value=128)

[notfound]
search  key=0x4a88c60 -> NOTFOUND (expected NOTFOUND)
search  key=0x4a88cc0 -> NOTFOUND (expected NOTFOUND)

[delete]
delete  key=0x4a88ab0 -> OK (value=48)
search  key=0x4a88ab0 -> NOTFOUND (after delete)
delete  key=0x4a872e0 -> OK (value=16)
search  key=0x4a872e0 -> NOTFOUND (after delete)

[prune]
prune   key=(nil) -> OK

[cleanup]
delete  key=0x4a88800 -> OK (value=320)
delete  key=0x4a88b20 -> OK (value=64)
delete  key=0x4a88ba0 -> OK (value=128)
==278879==
==278879== HEAP SUMMARY:
==278879==     in use at exit: 0 bytes in 0 blocks
==278879==   total heap usage: 19 allocs, 19 frees, 6,168 bytes allocated
==278879==
==278879== All heap blocks were freed -- no leaks are possible
==278879==
==278879== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```
