#include "app.h"

#define R 1.1

void
draw_point(float x, float y)
{
	CGRect r;

	y = app_total_h - y;

	r.origin.x = x - R;
	r.origin.y = y - R;

	r.size.height = 2.0 * R;
	r.size.width = 2.0 * R;

	CGContextFillEllipseInRect(gcontext, r);
}
