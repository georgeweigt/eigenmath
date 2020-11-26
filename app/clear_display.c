#include "app.h"

void
clear_display(void)
{
	struct display *p;

	if (running)
		return;

	while (first) {
		p = first;
		first = first->next;
		free(p);
	}

	last = NULL;
	barrier = NULL;

	total_height = 0.0;
	total_width = 0.0;

	cleared = 1;
}
