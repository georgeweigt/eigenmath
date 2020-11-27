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

NSString *filename;
NSTimer *timer;
int spinner;
char *input_c_string;

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

	[_input setFocusRingType:NSFocusRingTypeNone];
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
		if (result == NSFileHandlingPanelOKButton) {
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
		if (result == NSFileHandlingPanelOKButton) {
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

	if (input_c_string)
		free((void *) input_c_string);

	input_c_string = strdup([s UTF8String]);

	echo_input(input_c_string);

	run_thread(input_c_string);

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

	if (input_c_string)
		free((void *) input_c_string);

	input_c_string = strdup([[_textview string] UTF8String]);

	clear_display();

	[self updateDisplay];

	zero = NULL; // force full init

	run_thread(input_c_string);

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

// to here on clear button

-(IBAction)clearButtonHandler:(id)sender
{
	if (running)
		return;
	zero = NULL; // force full init on next eval
	clear_display();
	[self updateDisplay];
}

// to here on draw button

-(IBAction)drawButtonHandler:(id)sender
{
	[self evalString:@"draw"];
}

// to here on simplify button

-(IBAction)simplifyButtonHandler:(id)sender
{
	[self evalString:@"simplify"];
}

// to here on float button

-(IBAction)floatButtonHandler:(id)sender
{
	[self evalString:@"float"];
}

// to here on derivative button

-(IBAction)derivativeButtonHandler:(id)sender
{
	[self evalString:@"derivative"];
}

// to here on integral button

-(IBAction)integralButtonHandler:(id)sender
{
	[self evalString:@"integral"];
}

// to here when user resizes app

-(void)windowDidResize:(NSNotification *)notification
{
	[self updateDisplay];
}

// to here on timer interrupt

-(void)timerFunction:(NSTimer *)t
{
	[self checkDisplay];
}

-(void)checkDisplay
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
