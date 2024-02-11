#ifndef _HELLO_H
#define _HELLO_H

enum hello_state {
	HELLO,
	WORLD,
	END,
};

struct hello {
	enum hello_state state;
};

struct hello *hello_init(void);
void hello_free(struct hello *hello);

#endif // _HELLO_H
