#ifndef _HELLO_H
#define _HELLO_H

struct hello {
	enum hello_state state;
};

struct hello *hello_init(void);
void hello_free(struct hello *h);

#endif // _HELLO_H
