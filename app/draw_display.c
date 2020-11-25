#include "app.h"

void
draw_display(int y1, int y2)
{
	int y;
	struct display *p;
	y = 0;
	p = first;
	while (p) {
		draw_block(p, y, y1, y2);
		y += p->h;
		if (p == mark)
			break;
		p = p->next;
	}
}
