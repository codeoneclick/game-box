//
//  game_controller_ios.mm
//  indie2dEngine
//
//  Created by Sergey Sergeev on 5/17/13.
//  Copyright (c) 2013 Sergey Sergeev. All rights reserved.
//

#include "game_controller_ios.h"
#include "main_game_controller.h"
#include "main_menu_transition.h"
#include "window_impl.h"

#if USED_GRAPHICS_API == METAL_API

#import <MetalKit/MetalKit.h>

#endif

@interface game_controller_ios ()

@property (nonatomic, unsafe_unretained) std::shared_ptr<game::main_game_controller> m_game_controller;
@property (weak, nonatomic) IBOutlet opengl_view *m_opengl_view;

@end

@implementation game_controller_ios

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    f32 width = [[UIScreen mainScreen] bounds].size.width;
    f32 height = [[UIScreen mainScreen] bounds].size.height;
    [self.view setFrame:CGRectMake(0.f, 0.f, width, height)];
    
#if USED_GRAPHICS_API == OPENGL_20_API || USED_GRAPHICS_API == OPENGL_30_API
    
    [self.m_opengl_view setFrame:CGRectMake(0.f, 0.f, width, height)];
    std::shared_ptr<gb::window_impl> window = std::make_shared<gb::window_impl>((__bridge void*)self.m_opengl_view);
    
#elif USED_GRAPHICS_API == METAL_API
    
    MTKView* mtl_view = [[MTKView alloc] initWithFrame:CGRectMake(0.0,
                                                                  0.0,
                                                                  self.view.frame.size.width,
                                                                  self.view.frame.size.height)];
    [self.view addSubview:mtl_view];
    std::shared_ptr<gb::window_impl> window = std::make_shared<gb::window_impl>((__bridge void*)mtl_view);
    
#endif
    
    self.m_game_controller = std::make_shared<game::main_game_controller>(window);
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown && interfaceOrientation != UIInterfaceOrientationPortrait);
}

- (void)viewDidUnload
{
    [super viewDidUnload];
}

@end
