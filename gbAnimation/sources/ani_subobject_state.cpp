#include "ani_subobject_state.h"

namespace gb
{
    namespace anim
    {
        ani_subobject_state::ani_subobject_state() :
        m_object_id_reference(std::numeric_limits<ui32>::max()),
        m_mask_object_id_reference(std::numeric_limits<ui32>::max()),
        m_position(glm::vec2(0.f)),
        m_scale(glm::vec2(1.f)),
        m_rotation(0.f),
        m_visible(false)
        {
        }
        
        ani_subobject_state::~ani_subobject_state()
        {
        }
        
        bool ani_subobject_state::empty(ui32 object_id_reference)
        {
            m_object_id_reference = object_id_reference;
            m_z_index = 0;
            return true;
        }
        
        void ani_subobject_state::set_affine_transform(const ani_affine_transform& affine_transform)
        {
            m_scale.x = sqrtf((affine_transform.a * affine_transform.a) + (affine_transform.c * affine_transform.c));
            m_scale.y = sqrtf((affine_transform.b * affine_transform.b) + (affine_transform.d * affine_transform.d));
            
            m_position.x = affine_transform.tx;
            m_position.y = affine_transform.ty;
            
            f32 sign = atanf(-affine_transform.c / affine_transform.a);
            f32 radians  = acosf(affine_transform.a / m_scale.x);
            m_rotation  = glm::degrees(radians);
            
            if ((m_rotation > 90.f && sign > 0.f) || (m_rotation < 90.f && sign < 0.f))
            {
                m_rotation = 360.f - m_rotation;
            }
        }
        
        void ani_subobject_state::set_z_index(i32 z_index)
        {
            m_z_index = z_index;
        }
        
        glm::vec2 ani_subobject_state::get_position() const
        {
            return m_position;
        }
        
        glm::vec2 ani_subobject_state::get_scale() const
        {
            return m_scale;
        }
        
        f32 ani_subobject_state::get_rotation() const
        {
            return m_rotation;
        }
        
        i32 ani_subobject_state::get_z_index() const
        {
            return m_z_index;
        }
        
        void ani_subobject_state::set_visible(bool value)
        {
            m_visible = value;
        }
        
        bool ani_subobject_state::get_visible() const
        {
            return m_visible;
        }
    }
}
