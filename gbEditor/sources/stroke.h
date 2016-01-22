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
            glm::vec4 m_animated_color;

            void on_color_change_callback(const gb::ces_entity_shared_ptr& entity, f32 deltatime);
            
        public:
            
            stroke();
            ~stroke();
            
            void set_color(const glm::vec4& color);
            glm::vec4 get_color() const;
            
            void set_size(const glm::vec2& size);
            
            void set_is_animated(bool value);
            
            glm::vec4 get_bound() const;
        };
    };
};

#endif
