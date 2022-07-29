//
//  EigenmathAppDelegate.h
//  Eigenmath
//
//  Created by George Weigt on 7/26/13.
//  Copyright (c) 2013 George Weigt. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "EigenmathView.h"

@interface EigenmathAppDelegate : NSObject <NSApplicationDelegate>

@property (unsafe_unretained) IBOutlet NSWindow *window;

@property (weak) IBOutlet NSProgressIndicator *progressIndicator;

-(IBAction)evaluate:(id)sender;

-(IBAction)runButtonHandler:(id)sender;
-(IBAction)stopButtonHandler:(id)sender;

-(void)evalString:(NSString *)s;

-(void)windowDidResize:(NSNotification *)notification;
-(void)updateDisplay;

@property (unsafe_unretained) IBOutlet NSTextView *textview;
@property (weak) IBOutlet EigenmathView *eigenmathOutput;
@property (weak) IBOutlet NSTextField *input;

@end
