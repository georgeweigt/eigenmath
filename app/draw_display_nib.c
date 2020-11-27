#include "app.h"

void
draw_display_nib(struct display *p, double y, double ymin, double ymax)
{
	// clip to view rect

	if (y + p->height < ymin || y > ymax)
		return;

	switch (p->color) {
	case BLACK:
		CGContextSetRGBFillColor(gcontext, 0.0, 0.0, 0.0, 1.0);
		CGContextSetRGBStrokeColor(gcontext, 0.0, 0.0, 0.0, 1.0);
		break;
	case BLUE:
		CGContextSetRGBFillColor(gcontext, 0.0, 0.0, 1.0, 1.0);
		CGContextSetRGBStrokeColor(gcontext, 0.0, 0.0, 1.0, 1.0);
		break;
	case RED:
		CGContextSetRGBFillColor(gcontext, 1.0, 0.0, 0.0, 1.0);
		CGContextSetRGBStrokeColor(gcontext, 1.0, 0.0, 0.0, 1.0);
		break;
	}

	switch (p->type) {

	case 0:
		draw_text(p->dx, y + p->dy, p->buf, p->len);
		break;

	case 1:
		draw_formula(p->dx, y + p->dy, p->mem);
		break;
	}
}
