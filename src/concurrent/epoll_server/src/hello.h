#ifndef _HELLO_H
#define _HELLO_H

enum hello_state {
	HELLO,
	WORLD,
	END,
};

typedef struct {
	enum hello_state state;
} hello;

#define INIT_HELLO()            \
	{                       \
		.state = HELLO, \
	};

#endif // _HELLO_H
