//
//  EigenmathView.m
//  Eigenmath
//
//  Created by George Weigt on 7/26/13.
//  Copyright (c) 2013 George Weigt. All rights reserved.
//

#import "EigenmathView.h"

extern double document_height;
extern double document_width;

extern void draw_display(double ymin, double ymax);

CGContextRef gcontext;

@implementation EigenmathView

-(BOOL)acceptsFirstResponder
{
	return YES;
}

-(void)drawRect:(NSRect)rect
{
	double y1, y2;

//	gcontext = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort]; [DEPRECATED]
	gcontext = (CGContextRef)[[NSGraphicsContext currentContext] CGContext];

	CGAffineTransform w;
	w.a = 1.0;
	w.b = 0.0;
	w.c = 0.0;
	w.d = 1.0;
	CGContextSetTextMatrix(gcontext, w); // is indeterminant, must be set

	y1 = rect.origin.y + rect.size.height;
	y2 = rect.origin.y;

	draw_display(y1, y2);
}

-(void)copy:(id)sender
{
	NSRect r;
	NSPasteboard *p;

	r.origin.x = 0;
	r.origin.y = 0;

	r.size.width = document_width;
	r.size.height = document_height;

	p = [NSPasteboard generalPasteboard];
	[p declareTypes:[NSArray arrayWithObjects:NSPasteboardTypePDF, nil] owner:self];
	[self writePDFInsideRect:r toPasteboard:p];
}

@end
