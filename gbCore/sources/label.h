//
//  label.hpp
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef label_h
#define label_h

#include "renderable_game_object.h"

namespace gb
{
    class label : public renderable_game_object
    {
    private:
        
    protected:
        
    public:
        
        label();
        ~label();
        
        void set_text(const std::string& text);
        std::string get_text() const;
        
        void set_font_height(i32 height);
        
        glm::vec2 get_min_bound() const;
        glm::vec2 get_max_bound() const;
        
        glm::vec4 get_bound_frame() const;
    };
};

#endif
