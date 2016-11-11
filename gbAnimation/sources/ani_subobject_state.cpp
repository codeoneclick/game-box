#include "ani_subobject_state.h"

namespace gb
{
    namespace anim
    {
        ani_subobject_state::ani_subobject_state() :
        m_object_id_reference(std::numeric_limits<ui32>::max()),
        m_mask_object_id_reference(std::numeric_limits<ui32>::max())
        {
        }
        
        ani_subobject_state::~ani_subobject_state()
        {
        }
        
        bool ani_subobject_state::empty(ui32 object_id_reference)
        {
            m_object_id_reference = object_id_reference;
            m_z_index = 0;
            m_affine_transform = { 1, 0, 0, 1, 0, 0 };
            return true;
        }
    }
}
