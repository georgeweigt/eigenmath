#include "app.h"

void
draw_block(struct display *p, double y, double y1, double y2)
{
	// clip to view rect

	if (y + p->h < y1 || y > y2)
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
		draw_text(HPAD, y, p->buf, p->len);
		break;

	case 1:
		draw_formula(HPAD, y + VPAD, p->tab);
		break;
	}
}
