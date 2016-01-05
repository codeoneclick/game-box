//
//  demo_controller_ios.mm
//  indie2dEngine
//
//  Created by Sergey Sergeev on 5/17/13.
//  Copyright (c) 2013 Sergey Sergeev. All rights reserved.
//

#include "demo_controller_ios.h"
#include "demo_game_controller.h"
#include "demo_game_transition.h"
#include "ogl_window.h"

@interface demo_controller_ios ()

@property (nonatomic, unsafe_unretained) std::shared_ptr<demo_game_controller> m_game_controller;
@property (nonatomic, unsafe_unretained) std::shared_ptr<demo_game_transition> m_game_transition;
@property (weak, nonatomic) IBOutlet opengl_view *m_opengl_view;

@end

@implementation demo_controller_ios

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    f32 width = std::max([[UIScreen mainScreen] bounds].size.height, [[UIScreen mainScreen] bounds].size.width);
    f32 height = std::min([[UIScreen mainScreen] bounds].size.height, [[UIScreen mainScreen] bounds].size.width);
    [self.view setFrame:CGRectMake(0.f, 0.f, width, height)];
    [self.m_opengl_view setFrame:CGRectMake(0.f, 0.f, width, height)];

    std::shared_ptr<gb::ogl_window> window = std::make_shared<gb::ogl_window>((__bridge void*)self.m_opengl_view);
    
    self.m_game_controller = std::make_shared<demo_game_controller>(window);
    self.m_game_transition = std::make_shared<demo_game_transition>("transition.demo.xml", false);
    self.m_game_controller->add_transition(self.m_game_transition);
    self.m_game_controller->goto_transition("transition.demo.xml");
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
