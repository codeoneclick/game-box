#pragma once

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    namespace anim
    {
        class ani_texture_atlas_element
        {
        public:
            
            enum class e_rotation : int8_t
            {
                ccw_90 = -1,
                none = 0,
                cw_90 = 1,
            };
            
        private:
            
            f32 m_scale;
            f32 m_scale_x;
            f32 m_scale_y;
            
        public:
            
            std::string m_linkage_name;
            std::string m_base_class;
            std::string m_origin_class;
            glm::vec2 m_pivot;
            glm::vec4 m_bounds;
            
            ui32 m_atlas_index;
            ui32 m_element_atlas_index;
            
            e_rotation m_rotation;
            
            glm::vec4 m_scale9_grid;
            
            ani_texture_atlas_element();
            
            void set_scale(f32 value);
            f32 get_scale() const { return m_scale; }
            
            void set_scale_x(f32 value);
            f32 get_scale_x() const { return m_scale_x; }
            
            void set_scale_y(f32 value);
            f32 get_scale_y() const { return m_scale_y; }
            
        };
        
    };
};
