#include "app.h"

void
clear_display(void)
{
	struct display *t;
	while (first) {
		t = first;
		first = first->next;
		free(t);
	}
	last = NULL;
	mark = NULL;
	total_w = 0;
	total_h = 0;
	cleared = 1;
}
