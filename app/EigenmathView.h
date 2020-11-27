#ifndef __EigenmathView__
#define __EigenmathView__

#import <Cocoa/Cocoa.h>

@interface EigenmathView : NSView
{
	NSPoint startingPoint;
	NSRect selectionRect;
}

@end

#endif	// __EigenmathView__
