#include "app.h"

void
draw_selection_rect(double x, double y, double width, double height)
{
	double fx1, fx2, fy1, fy2;
	static double lengths[2] = {2.0, 2.0}; // for dashed line

	// both equal to 0.0 means there is no selection

	if (width == 0.0 && height == 0.0)
		return;

	fx1 = x;
	fx2 = x + width;

	fy1 = y;
	fy2 = y + height;

	if (fx1 < 0)
		fx1 = 0;

	if (fx2 >= document_width)
		fx2 = document_width - 1;

	if (fy1 < 0)
		fy1 = 0;

	if (fy2 >= document_height)
		fy2 = document_height - 1;

	CGContextSetRGBFillColor(gcontext, 0.0, 0.0, 0.0, 1.0); // black
	CGContextSetRGBStrokeColor(gcontext, 0.0, 0.0, 0.0, 1.0);

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
