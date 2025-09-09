# Radix Tree Documentation

## Radix Tree Node Structure

### shift and tree level

```txt
shift: 64 bits  32 bits
   54: 000000   24: 0000 == root
   48: 000000   20: 0000
   42: 000000   16: 0000
   36: 000000   12: 0000
   30: 000000    8: 0000
   24: 000000    4: 0000
   18: 000000    0: 0000 == leaf
   12: 000000       0000 == tag
    6: 000000
    0: 000000 == leaf
         0000 == tag
```

### offset

- RDX_MAP_MASK: `0011 1111`

```txt
values' offset = (index >> shift) & RDX_MAP_MASK
```

### raw value → tagged pointer

#### drop 4 bits with bit shift

- RDX_TAG_BITS: 4

```txt
     Address 64 bits:      0000000000000000010111101101010101100011001001000001001011110010
        address >> 4:          000000000000000001011110110101010110001100100100000100101111 | 0010
                                                                                               tag
```

#### group by 6 bits

- RDX_MAP_SHIFT: 6

```txt
     group by 6 bits: 000000 000000 000001 011110 110101 010110 001100 100100 000100 101111
```

#### shift bits

- RDX_ROOT_SHIFT: 54
- RDX_MAP_MASK: `0011 1111`

```txt
               shift: 60     54     48     42     36     30     24     18     12     6
             Address: 000000 000000 000001 011110 110101 010110 001100 100100 000100 101111
                 HEX:     00     00     01     1E     35     16     0C     24      4     2F
                 DEC:      0      0      1     30     53     22     12     36      4     47
(address >> 4) >>  0: 000000 000000 000001 011110 110101 010110 001100 100100 000100 101111
(address >> 4) >>  6:        000000 000000 000001 011110 110101 010110 001100 100100 000100
(address >> 4) >> 12:               000000 000000 000001 011110 110101 010110 001100 100100
(address >> 4) >> 18:                      000000 000000 000001 011110 110101 010110 001100
(address >> 4) >> 24:                             000000 000000 000001 011110 110101 010110
(address >> 4) >> 30:                                    000000 000000 000001 011110 110101
(address >> 4) >> 36:                                           000000 000000 000001 011110
(address >> 4) >> 42:                                                  000000 000000 000001
(address >> 4) >> 48:                                                         000000 000000
(address >> 4) >> 54:                                                                000000

        RDX_MAP_MASK:                                                                111111
              offset: 000000 000000 000000 000000 000000 000000 000000 000000 000000 xxxxxx
```

#### results

```txt
 root: shift 54, offset  0, parent NULL
node1: shift 48, offset  0, parent root
node2: shift 42, offset  1, parent node1
node3: shift 36, offset 30, parent node2
node4: shift 30, offset 53, parent node3
node5: shift 24, offset 22, parent node4
node6: shift 18, offset 12, parent node5
node7: shift 12, offset 36, parent node6
node8: shift  6, offset  4, parent node7
node9: shift  0, offset 47, parent node8 <-- leaf node. save the value
```
