#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "tags_defines.h"

namespace gb
{
    namespace anim
    {
        class ani_subobject_state
        {
        private:
            
            std::vector<ui32> m_custom_properties_value_indexes;
            
        public:
            
            ui32 m_object_id_reference;
            ui32 m_mask_object_id_reference;
            
            i32 m_z_index;
            ani_affine_transform m_affine_transform;
            
            bool empty(ui32 object_id_reference);
            
            ani_subobject_state();
            ~ani_subobject_state();
            
            const std::vector<uint32_t>& get_custom_properties_value_indexes() const
            {
                return m_custom_properties_value_indexes;
            }
            
            std::vector<uint32_t>& get_custom_properties_value_indexes()
            {
                return m_custom_properties_value_indexes;
            }
        };
    };
};
