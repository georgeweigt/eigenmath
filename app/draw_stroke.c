#include "app.h"

void
draw_stroke(double x1, double y1, double x2, double y2, double stroke_width)
{
	CGPoint p[2];

	y1 = document_height - y1;
	y2 = document_height - y2;

	x1 = round(x1);
	x2 = round(x2);

	y1 = round(y1);
	y2 = round(y2);

	p[0].x = x1;
	p[0].y = y1;

	p[1].x = x2;
	p[1].y = y2;

	CGContextSetLineWidth(gcontext, stroke_width);
	CGContextStrokeLineSegments(gcontext, p, 2);
}
