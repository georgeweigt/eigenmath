#include "app.h"

void
clear_display(void)
{
	struct display *p, *t;

	if (running)
		return;

	p = atomic_exchange(&display_list, NULL);

	while (p) {
		t = p;
		p = p->next;
		free(t);
	}

	fence = NULL;

	total_height = 0.0;
	total_width = 0.0;
}
