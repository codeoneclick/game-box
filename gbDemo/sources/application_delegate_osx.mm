//
//  application_delegate_osx.mm
//  gbDemo
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "application_delegate_osx.h"
#include "game_conroller_osx.h"

#if USED_GRAPHICS_API == METAL_API

#import <MetalKit/MetalKit.h>

#endif

@interface application_delegate_osx ()

@property (weak) IBOutlet NSWindow *window;
@property (nonatomic, unsafe_unretained) std::shared_ptr<game::game_conroller_osx> m_game_controller;
@property (weak) IBOutlet NSView *view;

@end

@implementation application_delegate_osx

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    
#if USED_GRAPHICS_API == OPENGL_20_API || USED_GRAPHICS_API == OPENGL_30_API
    
    NSOpenGLView *ogl_view = [[NSOpenGLView alloc] initWithFrame:CGRectMake(0.0,
                                                                            0.0,
                                                                            self.view.frame.size.width,
                                                                            self.view.frame.size.height)];
    [self.view addSubview:ogl_view];
    self.m_game_controller = std::make_shared<game::game_conroller_osx>(ogl_view);
    
#elif USED_GRAPHICS_API == METAL_API
    
    MTKView* mtl_view = [[MTKView alloc] initWithFrame:CGRectMake(0.0,
                                                                  0.0,
                                                                  self.view.frame.size.width,
                                                                  self.view.frame.size.height)];
    [self.view addSubview:mtl_view];
    self.m_game_controller = std::make_shared<game::game_conroller_osx>(mtl_view);
    
#endif

}

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
    
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)aSender
{
    return YES;
}

@end
