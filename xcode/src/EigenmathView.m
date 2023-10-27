//
//  EigenmathView.m
//  Eigenmath
//
//  Created by George Weigt on 7/26/13.
//  Copyright (c) 2013 George Weigt. All rights reserved.
//

#import "EigenmathView.h"

extern void draw_display(double ymin, double ymax);
extern void draw_selection_rect(double x, double y, double width, double height);

CGContextRef gcontext;

@implementation EigenmathView

-(BOOL)acceptsFirstResponder {return YES;}

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

@end
