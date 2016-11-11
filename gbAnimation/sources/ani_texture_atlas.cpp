#include "ani_texture_atlas.h"
#include "ani_texture_atlas_element.h"

namespace gb
{
    namespace anim
    {
        ani_texture_atlas::ani_texture_atlas()
        : m_scale(1.f)
        {
        }
        
        ani_texture_atlas::~ani_texture_atlas()
        {
           
        }
        
        bool ani_texture_atlas::compare_atlases_by_id(const ani_atlas_metadata& value_01, const ani_atlas_metadata& value_02)
        {
            return value_01.m_id < value_02.m_id;
        }
        
        void ani_texture_atlas::set_scale(f32 value)
        {
            m_scale = value;
        }
        
        float ani_texture_atlas::get_scale() const
        {
            return m_scale;
        }
        
        void ani_texture_atlas::push_atlas_metadata(const ani_atlas_metadata& metadata)
        {
            m_atlas_metadatas.push_back(metadata);
        }
        
        void ani_texture_atlas::push_element(ui32 index, const std::shared_ptr<ani_texture_atlas_element>& element)
        {
            m_elements[index] = element;
        }
        
        bool ani_texture_atlas::swap_element(ui32 index, const std::shared_ptr<ani_texture_atlas_element>& element)
        {
            elements_t::iterator iterator = m_elements.find(index);
            if (iterator != m_elements.end())
            {
                m_elements.erase(iterator);
            }
            push_element(index, element);
            return true;
        }
        
        const ani_texture_atlas::elements_t& ani_texture_atlas::get_elements() const
        {
            return m_elements;
        }
        
        const ani_texture_atlas::atlas_metadatas_t& ani_texture_atlas::get_atlas_metadatas() const
        {
            return m_atlas_metadatas;
        }
    }
}
