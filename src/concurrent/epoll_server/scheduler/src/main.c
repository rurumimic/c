#include <stdio.h>
#include <stdlib.h>

#include "executor.h"
#include "spawner.h"
#include "hello.h"

int main(int argc, char *argv[])
{
	struct executor *e = executor_init();
	struct spawner *s = executor_get_spawner(e);

  if (!s) {
    perror("main: failed to get spawner");
    return EXIT_FAILURE;
  }

	spawner_spawn(s, hello_init());

	executor_run(e);

	spawner_free(s);
	executor_free(e);

	return EXIT_SUCCESS;
}
