#include "app.h"

// app calls check_display() then prep_display() then draw_display()

void
draw_display(double ymin, double ymax)
{
	double y;
	struct display *p;

	y = document_height;

	p = fence;

	while (p) {
		y -= p->height;
		draw_display_nib(p, y, ymin, ymax);
		p = p->next;
	}
}
