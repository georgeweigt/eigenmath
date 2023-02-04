#include "app.h"
#include <pthread.h>

pthread_t thread_mem;

void
run_thread(char *s)
{
	if (running)
		return;

	running = 1;

	memset(&thread_mem, 0, sizeof thread_mem);
	pthread_create(&thread_mem, NULL, run_thread_nib, s);
	pthread_detach(thread_mem);
}

void *
run_thread_nib(void *p)
{
	run((char *) p);
	running = 0;
	return NULL;
}
