#include "app.h"

// post to display list

void
shipout(struct display *p)
{
	p->dy = round(p->dy); // fractions look better

	p->height = round(p->height);

	total_height += p->height;
	total_width = fmax(total_width, p->width);

	p->total_height = total_height;
	p->total_width = total_width;

	p->next = atomic_load(&display_list);

	atomic_store(&display_list, p);
}
