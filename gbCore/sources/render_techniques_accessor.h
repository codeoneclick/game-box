//
//  render_techniques_accessor.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"

#if USED_GRAPHICS_API != NO_GRAPHICS_API

#include "declarations.h"

namespace gb
{
    class render_techniques_accessor
    {
    private:
        
    protected:
        
    public:
        
        render_techniques_accessor() = default;
        virtual ~render_techniques_accessor() = default;
        
        virtual ui32 get_screen_width() = 0;
        virtual ui32 get_screen_height() = 0;
        
        virtual std::shared_ptr<graphics_context> get_graphics_context() const = 0;
        virtual const std::list<std::shared_ptr<render_technique_ws>>& get_ws_techniques() const = 0;
    };
};

#endif
