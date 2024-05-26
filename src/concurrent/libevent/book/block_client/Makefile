CC ?= clang
CFLAGS=-g -Wall -Wextra -Werror -std=c99 -pedantic
FILES=main.c
TARGET=main

ifeq ($(CC), gcc)
    CFLAGS += -D_GNU_SOURCE
else
    CFLAGS += -D_DEFAULT_SOURCE
endif

$(TARGET): $(FILES)
	$(CC) $(CFLAGS) -o $@ $(FILES)
