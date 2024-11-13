//
//  EigenmathAppDelegate.m
//  Eigenmath
//
//  Created by George Weigt on 7/26/13.
//  Copyright (c) 2013 George Weigt. All rights reserved.
//

#import "EigenmathAppDelegate.h"

extern void *zero;
extern int interrupt;
extern int running;
extern double document_height;
extern double document_width;

extern void init_fonts(void);
extern void echo_input(char *);
extern void run_thread(char *);
extern void clear_display(void);
extern int check_display(void);
extern void prep_display(void);

extern char *history_up(char *);
extern char *history_down(char *);
extern void history_push(char *);

NSString *filename;
NSTimer *timer;
int spinner;
char *instring;

@implementation EigenmathAppDelegate

-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
	return YES;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	// Insert code here to initialize your application
	if (@available(macOS 10.14, *))
	{
		NSApp.appearance = [NSAppearance appearanceNamed: NSAppearanceNameAqua];
	}
	NSNotificationCenter *p = [NSNotificationCenter defaultCenter];
	[p addObserver:self selector:@selector(windowDidResize:) name:NSWindowDidResizeNotification object:_window];

//	[_input setFocusRingType:NSFocusRingTypeNone];
	[_input setFont:[NSFont fontWithName:@"Courier" size:16.0]];
	[_window makeFirstResponder:_input];
	[_textview setFont:[NSFont fontWithName:@"Courier" size:16.0]];
	[_textview setAutomaticSpellingCorrectionEnabled:NO];
	[_textview setAutomaticQuoteSubstitutionEnabled:NO];
	_textview.enabledTextCheckingTypes = 0; // need this so -- (dash dash) isn't changed

	init_fonts();
}

// this function is linked to file menu automatically

-(IBAction)openDocument:(id)sender {
	NSOpenPanel* panel = [NSOpenPanel openPanel];
	[panel beginWithCompletionHandler:^(NSInteger result){
//		if (result == NSFileHandlingPanelOKButton) { [DEPRECATED]
		if (result == NSModalResponseOK) {
			NSURL *url = [[panel URLs] objectAtIndex:0];
			NSString *s = [NSString stringWithContentsOfURL:url encoding:NSUTF8StringEncoding error:nil];
			[self->_textview setString:s];
			[self->_textview setNeedsDisplay:YES];
			filename = [[NSString alloc] initWithString:[url absoluteString]];
			[self->_window setTitle:filename];
		}

	}];
}

// this function is linked to file menu automatically

-(IBAction)saveDocument:(id)sender
{
	if (filename == nil)
		[self saveDocumentAs:sender];
	else {
		NSString *s = [_textview string];
		[s writeToURL:[NSURL URLWithString:filename] atomically:NO encoding:NSUTF8StringEncoding error:nil];
	}
}

// this function is linked to file menu automatically

-(IBAction)saveDocumentAs:(id)sender
{
	NSSavePanel *panel = [NSSavePanel savePanel];
	[panel setCanSelectHiddenExtension:YES];
	[panel setTreatsFilePackagesAsDirectories:YES];
	[panel beginSheetModalForWindow:_window completionHandler:^(NSInteger result) {
//		if (result == NSFileHandlingPanelOKButton) { [DEPRECATED]
		if (result == NSModalResponseOK) {
			NSURL *url = [panel URL];
			NSString *s = [self->_textview string];
			[s writeToURL:url atomically:NO encoding:NSUTF8StringEncoding error:nil];
			filename = [[NSString alloc] initWithString:[url absoluteString]];
			[self->_window setTitle:filename];
		}
	}];
}

// this function handles the command line

-(IBAction)evaluate:(id)sender
{
	if (running)
		return;

	[self evalString:[_input stringValue]]; // evaluate user input

	[_input setStringValue:@""]; // clear command line
}

-(void)evalString:(NSString *)s
{
	if (running)
		return;

	if (instring)
		free((void *) instring);

	instring = strdup([s UTF8String]);

	history_push(instring);

	echo_input(instring);

	run_thread(instring);

	// start timer to update display

	timer = [NSTimer
		scheduledTimerWithTimeInterval:0.1
		target:self
		selector:@selector(timerFunction:)
		userInfo:nil
		repeats:YES];
}

// to here on run button

-(IBAction)runButtonHandler:(id)sender
{
	if (running)
		return;

	if (instring)
		free((void *) instring);

	instring = strdup([[_textview string] UTF8String]);

	clear_display();

	[self updateDisplay];

	zero = NULL; // force full init

	run_thread(instring);

	// start timer to update display

	timer = [NSTimer
		scheduledTimerWithTimeInterval:0.1
		target:self
		selector:@selector(timerFunction:)
		userInfo:nil
		repeats:YES];
}

// to here on stop button

-(IBAction)stopButtonHandler:(id)sender
{
	interrupt = 1;
}

// to here on history up

-(IBAction)historyUpHandler:(id)sender
{
	if (running)
		return;

	if (instring)
		free((void *) instring);

	instring = strdup([[_input stringValue] UTF8String]);

	char *cstr = history_up(instring);
	NSString *str = [[NSString alloc] initWithCString:cstr encoding:NSASCIIStringEncoding];

	[_input setStringValue:str];
	[[_input currentEditor] setSelectedRange:NSMakeRange(strlen(cstr),0)];
}

// to here on history down

-(IBAction)historyDownHandler:(id)sender
{
	if (running)
		return;

	if (instring)
		free((void *) instring);

	instring = strdup([[_input stringValue] UTF8String]);

	char *cstr = history_down(instring);
	NSString *str = [[NSString alloc] initWithCString:cstr encoding:NSASCIIStringEncoding];

	[_input setStringValue:str];
	[[_input currentEditor] setSelectedRange:NSMakeRange(strlen(cstr),0)];
}

// to here when user resizes app

-(void)windowDidResize:(NSNotification *)notification
{
	[self updateDisplay];
}

// to here on timer interrupt

-(void)timerFunction:(NSTimer *)t
{
	if (check_display())
		[self updateDisplay];

	if (running == 1 && spinner == 0) {
		[_progressIndicator startAnimation:nil];
		spinner = 1;
	}

	if (running == 0 && spinner == 1) {
		[_progressIndicator stopAnimation:nil];
		spinner = 0;
	}

	if (running == 0)
		[timer invalidate];
}

-(void)updateDisplay
{
	NSSize size;

	prep_display(); // to update document_height, document_width

	size.height = document_height;
	size.width = document_width;

	// for some reason, this has to be done twice when scrollers set to auto-hide
	// otherwise frame size is not updated sometimes

	[_eigenmathOutput setFrameSize:size];
	[_eigenmathOutput setFrameSize:size];

	[_eigenmathOutput setNeedsDisplay:YES];
}

@end
