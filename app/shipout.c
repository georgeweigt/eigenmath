#include "app.h"

void
shipout(struct display *p)
{
	p->height += 2 * VPAD;
	p->width += 2 * HPAD;

	p->height = ceil(p->height);
	p->width = ceil(p->width);

	total_height += p->height;

	if (p->width > total_width)
		total_width = p->width;

	p->total_height = total_height;
	p->total_width = total_width;

	p->next = NULL;

	if (first == NULL) {
		last = p;
		first = p;
	} else {
		last->next = p;
		last = p;
	}
}
