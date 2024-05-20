#include <stdlib.h>

void f(void)
{
	int *x = malloc(10 * sizeof(int));
	x[10] = 0; // heap block overrun
}            // memory leak -- x not freed

int main(int argc, char **argv)
{
	f();
	return 0;
}
