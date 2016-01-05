//
//  render_techniques_accessor.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef render_techniques_accessor_h
#define render_techniques_accessor_h

#include "main_headers.h"
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
        virtual const std::map<std::string, std::shared_ptr<render_technique_ws>>& get_ws_techniques() const = 0;
    };
};

#endif
