#include "app.h"

// post to display list

void
shipout(struct display *p)
{
	total_height += p->height;
	total_width = fmax(total_width, p->width);

	p->total_height = total_height;
	p->total_width = total_width;

	p->next = NULL;

	if (first == NULL)
		first = p;
	else
		last->next = p;

	OSMemoryBarrier(); // because cpu reorders memory writes

	last = p;
}
