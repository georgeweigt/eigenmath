#include "app.h"

void
draw_selection_rect(float x, float y, float width, float height)
{
	float fx1, fx2, fy1, fy2;
	static CGFloat lengths[2] = {2.0, 2.0}; // for dashed line

	// both equal to 0.0 means there is no selection

	if (width == 0.0 && height == 0.0)
		return;

	// pixel alignment

	fx1 = floor(x);
	fx2 = ceil(x + width);

	fy1 = floor(y);
	fy2 = ceil(y + height);

	if (fx1 < 0)
		fx1 = 0;

	if (fx2 >= app_total_w)
		fx2 = app_total_w - 1;

	if (fy1 < 0)
		fy1 = 0;

	if (fy2 >= app_total_h)
		fy2 = app_total_h - 1;

	// this is required to get a line that is exactly 1 pixel wide

	fx1 += 0.5;
	fx2 += 0.5;

	fy1 += 0.5;
	fy2 += 0.5;

	// set up the path

	CGContextBeginPath(gcontext);
	CGContextMoveToPoint(gcontext, fx1, fy1);
	CGContextAddLineToPoint(gcontext, fx2, fy1);
	CGContextAddLineToPoint(gcontext, fx2, fy2);
	CGContextAddLineToPoint(gcontext, fx1, fy2);
	CGContextClosePath(gcontext);

	// draw the rectangle

	CGContextSetLineDash(gcontext, 0.0, lengths, 2);
	CGContextSetLineWidth(gcontext, 1.0);
	CGContextStrokePath(gcontext);
	CGContextSetLineDash(gcontext, 0.0, NULL, 0);
}
