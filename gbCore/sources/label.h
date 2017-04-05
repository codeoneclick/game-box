//
//  label.h
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "game_object.h"

namespace gb
{
    class label : public game_object
    {
    private:
        
    protected:
        
    public:
        
        label();
        ~label();
        
        std::property_rw<std::string> text;
        std::property_rw<f32> font_size;
        std::property_rw<glm::u8vec4> font_color;
        std::property_rw<bool> is_luminous;
    };
};
