//
//  brush.hpp
//  gbEditor
//
//  Created by Serhii Serhiiv on 2/5/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef brush_hpp
#define brush_hpp

#include "renderable_game_object.h"

namespace gb
{
    namespace ed
    {
        class brush : public renderable_game_object
        {
        private:
            
        protected:
            
            f32 m_radius;
            
        public:
            
            brush();
            ~brush();
            
            void set_radius(f32 radius);
            f32 get_radius() const;
            
            void set_position(const glm::vec2& position);
            
            glm::vec4 get_bound() const;
        };
    };
};


#endif
