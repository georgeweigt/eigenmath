#include "app.h"

void
shipout(struct display *p)
{
	p->h += 2 * VPAD;
	p->w += 2 * HPAD;

	total_height += p->h;

	if (p->w > total_width)
		total_width = p->w;

	p->document_height = total_height;
	p->document_width = total_width;

	p->next = NULL;

	if (first == NULL) {
		last = p;
		first = p;
	} else {
		last->next = p;
		last = p;
	}
}
