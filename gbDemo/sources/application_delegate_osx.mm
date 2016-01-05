//
//  application_delegate_osx.mm
//  gbDemo
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "application_delegate_osx.h"
#include "demo_conroller_osx.h"

@interface application_delegate_osx ()

@property (weak) IBOutlet NSWindow *window;
@property (nonatomic, unsafe_unretained) std::shared_ptr<demo_conroller_osx> m_demo_controller;
@property (weak) IBOutlet NSView *view;

@end

@implementation application_delegate_osx

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    NSOpenGLView *ogl_view = [[NSOpenGLView alloc] initWithFrame:CGRectMake(0.0,
                                                                            0.0,
                                                                            self.view.frame.size.width,
                                                                            self.view.frame.size.height)];
    [self.view addSubview:ogl_view];
    self.m_demo_controller = std::make_shared<demo_conroller_osx>(ogl_view);
}

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
    
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)aSender
{
    return YES;
}

@end
