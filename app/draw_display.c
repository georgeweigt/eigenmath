#include "app.h"

// app calls check_display() then prep_display() then draw_display()

void
draw_display(double y1, double y2)
{
	double y;
	struct display *p;

	y = document_height;

	p = fence;

	while (p) {
		y -= p->height;
		draw_display_nib(p, y, y1, y2);
		p = p->next;
	}
}
