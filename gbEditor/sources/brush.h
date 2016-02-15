//
//  brush.hpp
//  gbEditor
//
//  Created by Serhii Serhiiv on 2/5/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef brush_hpp
#define brush_hpp

#include "game_object.h"

namespace gb
{
    namespace ed
    {
        class brush : public game_object
        {
        private:
            
            f32 m_radius;
            i32 m_active_texture_index;
            std::vector<texture_shared_ptr> m_textures;
            
        protected:
            
        public:
            
            brush();
            ~brush();
            
            void set_radius(f32 radius);
            f32 get_radius() const;
            
            void set_position(const glm::vec2& position);
            
            glm::vec4 get_bound() const;
            
            void add_texture(const texture_shared_ptr& texture);
            void remove_texture(const texture_shared_ptr& texture);
            
            texture_shared_ptr get_active_texture() const;
        };
    };
};


#endif
