#include "app.h"

void
shipout(struct display *p)
{
	p->h += 2 * VPAD;
	p->w += 2 * HPAD;

	total_h += p->h;

	if (p->w > total_w)
		total_w = p->w;

	p->document_height = total_h;
	p->document_width = total_w;

	p->next = NULL;

	if (first == NULL) {
		last = p;
		first = p;
	} else {
		last->next = p;
		last = p;
	}
}
