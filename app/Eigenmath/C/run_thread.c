#include "app.h"
#include <pthread.h>

void
run_thread(char *s)
{
	pthread_t thread;

	if (running)
		return;

	running = 1;

	pthread_create(&thread, NULL, run_thread_nib, s);
	pthread_detach(thread);
}

void *
run_thread_nib(void *s)
{
	run((char *) s);
	running = 0;
	return NULL;
}
