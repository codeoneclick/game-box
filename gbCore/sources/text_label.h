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
        
        i32 m_font_height;
        glm::vec4 m_text_color;
        
    protected:
        
    public:
        
        text_label();
        ~text_label();
        
        std::property_rw<std::string> text;
        std::property_rw<i32> font_height;
        std::property_rw<glm::vec4> text_color;
    };
};

#endif
