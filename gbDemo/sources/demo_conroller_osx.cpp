//
//  demo_conroller_osx.cpp
//  gbDemo
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "demo_conroller_osx.h"
#include "demo_game_controller.h"
#include "demo_game_transition.h"
#include "ogl_window.h"

demo_conroller_osx::demo_conroller_osx(NSView *ogl_view)
{
    std::shared_ptr<gb::ogl_window> window = std::make_shared<gb::ogl_window>((__bridge void*)ogl_view);
    m_game_controller = std::make_shared<demo_game_controller>(window);
    m_game_transition = std::make_shared<demo_game_transition>("transition.demo.xml", false);
    
    m_game_controller->add_transition(m_game_transition);
    m_game_controller->goto_transition("transition.demo.xml");
}

demo_conroller_osx::~demo_conroller_osx(void)
{
    
}