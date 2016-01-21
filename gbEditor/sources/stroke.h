//
//  stroke.h
//  gbEditor
//
//  Created by sergey.sergeev on 1/20/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef stroke_h
#define stroke_h

#include "renderable_game_object.h"

namespace gb
{
    namespace ed
    {
        class stroke : public renderable_game_object
        {
        private:
            
        protected:
            
            glm::vec4 m_color;
            
        public:
            
            stroke();
            ~stroke();
            
            void set_color(const glm::vec4& color);
            glm::vec4 get_color() const;
            
            void set_size(const glm::vec2& size);
            
            glm::vec4 get_bound() const;
        };
    };
};

#endif
