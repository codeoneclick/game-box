//
//  text_label.h
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef text_label_h
#define text_label_h

#include "game_object.h"

namespace gb
{
    class text_label : public game_object
    {
    private:
        
    protected:
        
    public:
        
        text_label();
        ~text_label();
        
        std::property_rw<std::string> text;
        std::property_rw<ui32> font_size;
        std::property_rw<glm::u8vec4> text_color;
    };
};

#endif
