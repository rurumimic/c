# Test

- github: [CppUTest](https://github.com/cpputest/cpputest)
- github: [Unity](https://github.com/ThrowTheSwitch/Unity)

## CppUTest

### Build CppUTest

```bash
git clone https://github.com/cpputest/cpputest.git
cd cpputest
mkdir cpputest_build
cd cpputest_build
autoreconf .. -i
../configure
make tdd
```

```bash
cd .. # cpputest
export CPPUTEST_HOME=$(pwd)
```

