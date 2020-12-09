#include "app.h"

void
draw_stroke(double x1, double y1, double x2, double y2, double stroke_width)
{
	CGPoint p[2];

	y1 = document_height - y1;
	y2 = document_height - y2;

	p[0].x = round(x1);
	p[0].y = round(y1);

	p[1].x = round(x2);
	p[1].y = round(y2);

	CGContextSetLineWidth(gcontext, stroke_width);
	CGContextStrokeLineSegments(gcontext, p, 2);
}
