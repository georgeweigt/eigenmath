#include "app.h"

void
draw_stroke(float x1, float y1, float x2, float y2, float stroke_width)
{
	y1 = app_total_h - y1;
	y2 = app_total_h - y2;

	x1 = roundf(x1);
	x2 = roundf(x2);

	y1 = roundf(y1);
	y2 = roundf(y2);

	CGContextBeginPath(gcontext);
	CGContextMoveToPoint(gcontext, x1, y1);
	CGContextAddLineToPoint(gcontext, x2, y2);
	CGContextSetLineWidth(gcontext, stroke_width);
	CGContextStrokePath(gcontext);
}
