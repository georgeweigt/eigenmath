#import "EigenmathView.h"

extern double document_height;
extern double document_width;

extern void draw_display(double, double);
extern void draw_selection_rect(double, double, double, double);

CGContextRef gcontext;

@implementation EigenmathView

-(void)drawRect:(NSRect)rect
{
	double y1, y2;

	gcontext = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];

	CGAffineTransform w;
	w.a = 1.0;
	w.b = 0.0;
	w.c = 0.0;
	w.d = 1.0;
	CGContextSetTextMatrix(gcontext, w); // is indeterminant, must be set

	y1 = rect.origin.y + rect.size.height;
	y2 = rect.origin.y;

	y1 = document_height - y1;
	y2 = document_height - y2;

	draw_display(y1, y2);

	draw_selection_rect(selectionRect.origin.x, selectionRect.origin.y, selectionRect.size.width, selectionRect.size.height);
}

-(void)mouseDown:(NSEvent *)event
{
	startingPoint = [self convertPoint:[event locationInWindow] fromView:nil]; // from Cocoa Drawing Guide p. 54
}

-(void)mouseUp:(NSEvent *)event
{
	NSRect r = selectionRect;

	// clear selection rectangle so it is not drawn

	selectionRect.size.width = 0;
	selectionRect.size.height = 0;

	// copy contents of rectangle to pasteboard

	NSPasteboard *p = [NSPasteboard generalPasteboard];
	[p declareTypes:[NSArray arrayWithObjects:NSPDFPboardType, nil] owner:self];
	[self writePDFInsideRect:r toPasteboard:p];

	// clear selection rectangle on screen

	[self display];
}

-(void)mouseDragged:(NSEvent *)event
{
	NSPoint p = [self convertPoint:[event locationInWindow] fromView:nil]; // from Cocoa Drawing Guide p. 54

	// update the selection rectangle

	selectionRect.origin.x = (startingPoint.x < p.x) ? startingPoint.x : p.x;
	selectionRect.origin.y = (startingPoint.y < p.y) ? startingPoint.y : p.y;
	
	selectionRect.size.width = fabs(startingPoint.x - p.x);
	selectionRect.size.height = fabs(startingPoint.y - p.y);

	[self display];
}

@end
