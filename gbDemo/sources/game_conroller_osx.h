//
//  game_conroller_osx.h
//  gbDemo
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "ns_declarations.h"

#if defined(__OSX__)

#include <Cocoa/Cocoa.h>

#if USED_GRAPHICS_API == METAL_API

#include <MetalKit/MetalKit.h>

#endif

#endif

namespace game
{
    class game_conroller_osx
    {
    private:
        
    protected:
        
        main_game_controller_shared_ptr m_game_controller;
        main_menu_transition_shared_ptr m_game_transition;
        
    public:
        
        game_conroller_osx(NSView* ogl_view);
        
#if USED_GRAPHICS_API == METAL_API
        
        game_conroller_osx(MTKView* metal_view);
        
#endif
        
        ~game_conroller_osx();
        
    };
};
