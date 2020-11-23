#include "app.h"

void
draw_line(int x1, int y1, int x2, int y2)
{
	float fx1, fx2, fy1, fy2;

	// not offsetting by 0.5, results in 2-pixel width to match draw_point()

	fx1 = x1;
	fx2 = x2;
	fy1 = app_total_h - y1;
	fy2 = app_total_h - y2;

	CGContextBeginPath(gcontext);
	CGContextMoveToPoint(gcontext, fx1, fy1);
	CGContextAddLineToPoint(gcontext, fx2, fy2);

	CGContextSetLineWidth(gcontext, 0.5);
	CGContextStrokePath(gcontext);
	CGContextSetLineWidth(gcontext, 1.0);
}
