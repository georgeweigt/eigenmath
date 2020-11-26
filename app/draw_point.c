#include "app.h"

#define R 1.25

void
draw_point(double x, double y)
{
	CGRect r;

	y = document_height - y;

	r.origin.x = x - R;
	r.origin.y = y - R;

	r.size.height = 2.0 * R;
	r.size.width = 2.0 * R;

	CGContextFillEllipseInRect(gcontext, r);
}
