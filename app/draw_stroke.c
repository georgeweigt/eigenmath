#include "app.h"

void
draw_stroke(double x1, double y1, double x2, double y2, double stroke_width)
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
