#include "app.h"

// post to display list

void
shipout(struct display *p)
{
	p->dy = round(p->dy);

	p->height = round(p->height);

	total_height += p->height;
	total_width = fmax(total_width, p->width);

	p->total_height = total_height;
	p->total_width = total_width;

	p->next = display_list;

	OSMemoryBarrier(); // because cpu reorders memory writes

	display_list = p;
}
