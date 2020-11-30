#include "app.h"

void
clear_display(void)
{
	struct display *p, *q;

	if (running)
		return;

	p = atomic_exchange(&display_list, NULL);

	while (p) {
		q = p->next;
		free(p);
		p = q;
	}

	fence = NULL;

	total_height = 0.0;
	total_width = 0.0;
}
