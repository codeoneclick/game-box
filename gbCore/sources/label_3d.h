//
//  label_3d.h
//  gbCore
//
//  Created by serhii.s on 4/4/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "game_object_3d.h"

namespace gb
{
    class label_3d : public game_object_3d
    {
    private:
        
    protected:
        
    public:
        
        label_3d();
        ~label_3d() = default;
        
        std::property_rw<std::string> text;
        std::property_rw<f32> font_size;
        std::property_rw<glm::u8vec4> font_color;
        void set_multiline(bool value, i32 max_line_width);
    };
};

