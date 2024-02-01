#include <stdio.h>

typedef enum {
    HELLO,
    WORLD,
    END,
} hello_state;

typedef struct {
    hello_state state;
} hello;

#define INIT_HELLO()                                                           \
    {                                                                          \
        .state = HELLO,                                                        \
    };

int main(int argc, char *argv[]) {

    hello h = INIT_HELLO();

    printf("Hello state: %d\n", h.state);

    return 0;
}
