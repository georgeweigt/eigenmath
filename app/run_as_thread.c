#include "app.h"
#include <pthread.h>

int running;
extern void run(char *);

void
run_as_thread(char *s)
{
	pthread_t thread;
	pthread_create(&thread, NULL, run1, s);
	pthread_detach(thread);
}

void *
run1(void *s)
{
	running = 1;
	run((char *) s);
	running = 0;
	return NULL;
}
