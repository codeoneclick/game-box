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
            
        protected:
            
            glm::vec2 m_position;
            glm::vec2 m_scale;
            f32 m_rotation;
            i32 m_z_index;
            bool m_visible;
            
        public:
            
            ui32 m_object_id_reference;
            ui32 m_mask_object_id_reference;
            
            bool empty(ui32 object_id_reference);
            
            ani_subobject_state();
            ~ani_subobject_state();
            
            ani_subobject_state(const ani_subobject_state& copy) = delete;
            ani_subobject_state& operator=(const ani_subobject_state& copy) = delete;
            
            void set_affine_transform(const ani_affine_transform& affine_transform);
            void set_z_index(i32 z_index);
            
            glm::vec2 get_position() const;
            glm::vec2 get_scale() const;
            f32 get_rotation() const;
            i32 get_z_index() const;
            
            void set_visible(bool value);
            bool get_visible() const;
            
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
