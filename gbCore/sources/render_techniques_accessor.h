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
        
        render_techniques_accessor(void);
        virtual ~render_techniques_accessor(void);
        
        virtual ui32 get_screen_width(void) = 0;
        virtual ui32 get_screen_height(void) = 0;
    };
};

#endif
