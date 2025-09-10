# Test

## cmocka

- [cmocka](https://cmocka.org/)
- gitlab: [cmocka](https://gitlab.com/cmocka/cmocka)

### Install cmocka

```bash
sudo apt install --no-install-recommends libcmocka-dev
```

---

## ceedling

- [ceedling](https://www.throwtheswitch.org/ceedling)
- github: [docs/CeedlingPacket.md](https://github.com/ThrowTheSwitch/Ceedling/blob/master/docs/CeedlingPacket.md)

### Install ceedling

```bash
gem install ceedling
```

### Create a new project

```bash
ceedling new tests
```

### Edit `project.yml`

```yaml
:paths:
  :test:
    - +:test/**
    - -:test/support
  :source:
    - ../src/**
  :include:
    - ../include/**
    - ../src/**
  :support:
    - test/support
  :libraries: []
```
