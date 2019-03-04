//
//  game_controller_tvos.mm
//  indie2dEngine
//
//  Created by Sergey Sergeev on 5/17/13.
//  Copyright (c) 2013 Sergey Sergeev. All rights reserved.
//

#include "game_controller_tvos.h"
#include "main_game_controller.h"
#include "main_menu_transition.h"
#include "window_impl.h"

@interface game_controller_tvos ()

@property (nonatomic, unsafe_unretained) std::shared_ptr<game::main_game_controller> m_game_controller;
@property (strong, nonatomic) IBOutlet opengl_view *m_opengl_view;

@end

@implementation game_controller_tvos

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    f32 width = [[UIScreen mainScreen] bounds].size.width;
    f32 height = [[UIScreen mainScreen] bounds].size.height;
    [self.view setFrame:CGRectMake(0.f, 0.f, width, height)];
    
    self.m_opengl_view = [[opengl_view alloc] initWithFrame:CGRectMake(0.f, 0.f, width, height)];
    [self.view addSubview:self.m_opengl_view];

    std::shared_ptr<gb::window_impl> window = std::make_shared<gb::window_impl>((__bridge void*)self.m_opengl_view);
    
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
