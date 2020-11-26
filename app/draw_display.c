#include "app.h"

void
draw_display(double y1, double y2)
{
	double y;
	struct display *p;
	y = 0.0;
	p = first;
	while (p) {
		draw_block(p, y, y1, y2);
		y += p->h;
		if (p == barrier)
			break;
		p = p->next;
	}
}
