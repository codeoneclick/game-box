//
//  demo_conroller_osx.h
//  gbDemo
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef demo_conroller_osx_h
#define demo_conroller_osx_h

#include "main_headers.h"

#if defined(__OSX__)

#include <Cocoa/Cocoa.h>

#endif

class demo_game_controller;
class demo_game_transition;
class demo_conroller_osx
{
private:
    
protected:
    
    std::shared_ptr<demo_game_controller> m_game_controller;
    std::shared_ptr<demo_game_transition> m_game_transition;
    
public:
    
    demo_conroller_osx(NSView *ogl_view);
    ~demo_conroller_osx(void);
    
};

#endif
