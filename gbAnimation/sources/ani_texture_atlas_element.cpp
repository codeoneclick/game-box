
#include "ani_texture_atlas_element.h"

namespace gb
{
    namespace anim
    {
        ani_texture_atlas_element::ani_texture_atlas_element()
        : m_linkage_name("")
        , m_base_class("")
        , m_origin_class("")
        , m_rotation(e_rotation::none)
        , m_scale9_grid(glm::vec4(0.f))
        {
            set_scale(1.f);
        }
        
        void ani_texture_atlas_element::set_scale(f32 value)
        {
            m_scale = m_scale_x = m_scale_y = value;
        }
        
        void ani_texture_atlas_element::set_scale_x(f32 value)
        {
            m_scale = m_scale_x = value;
        }
        
        void ani_texture_atlas_element::set_scale_y(f32 value)
        {
            m_scale = m_scale_y = value;
        }
    }
}
