#include "app.h"

void
draw_stroke(float x1, float y1, float x2, float y2, float stroke_width)
{
	CGPoint p[2];

	y1 = app_total_h - y1;
	y2 = app_total_h - y2;

	p[0].x = x1;
	p[0].y = y1;

	p[1].x = x2;
	p[1].y = y2;

	CGContextSetLineWidth(gcontext, stroke_width);
	CGContextStrokeLineSegments(gcontext, p, 2);
}
