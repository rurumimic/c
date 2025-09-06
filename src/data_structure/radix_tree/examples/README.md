# Examples

## Tree

```bash
just
./build/debug/examples/tree/example_tree
```

### valgrind

```bash
valgrind --leak-check=yes ./build/debug/examples/tree/example_tree
```

### Run

```bash
[insert]
insert  key=0x58b3ce65f4e0 -> OK (size=16)
insert  key=0x58b3ce660820 -> OK (size=32)
insert  key=0x58b3ce660c90 -> OK (size=48)
insert  key=0x58b3ce660ef0 -> OK (size=64)
insert  key=0x58b3ce660f40 -> OK (size=128)

[replace]
insert  key=0x58b3ce660820 -> REPLACED (size=320, expected REPLACED)
verify  key=0x58b3ce660820 current_value=320

[search]
search  key=0x58b3ce65f4e0 -> OK (value=16)
search  key=0x58b3ce660c90 -> OK (value=48)
search  key=0x58b3ce660f40 -> OK (value=128)

[notfound]
search  key=0x58b3ce660fd0 -> NOTFOUND (expected NOTFOUND)
search  key=0x58b3ce660ff0 -> NOTFOUND (expected NOTFOUND)

[delete]
delete  key=0x58b3ce660c90 -> OK (value=48)
search  key=0x58b3ce660c90 -> NOTFOUND (after delete)
delete  key=0x58b3ce65f4e0 -> OK (value=16)
search  key=0x58b3ce65f4e0 -> NOTFOUND (after delete)

[prune]
prune   key=(nil) -> OK

[cleanup]
delete  key=0x58b3ce660820 -> OK (value=320)
delete  key=0x58b3ce660ef0 -> OK (value=64)
delete  key=0x58b3ce660f40 -> OK (value=128)
```
