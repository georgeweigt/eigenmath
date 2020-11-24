#include "app.h"
#include <pthread.h>

int running;
extern void run(char *);

void
run_thread(char *s)
{
	pthread_t thread;
	pthread_create(&thread, NULL, run_thread_nib, s);
	pthread_detach(thread);
}

void *
run_thread_nib(void *s)
{
	running = 1;
	run((char *) s);
	running = 0;
	return NULL;
}
