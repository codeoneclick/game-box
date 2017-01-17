#include "ani_timeline.h"
#include "ani_animation_sequence.h"
#include "ani_texture_atlas.h"

namespace gb
{
    namespace anim
    {
        ani_external_object::ani_external_object() : ani_external_object(std::numeric_limits<ui32>::max(), "")
        {
        }
        
        ani_external_object::ani_external_object(ui32 object_id_reference, const std::string& name)
        : m_object_id_reference(object_id_reference)
        , m_name(name)
        {
        }
        
        ani_timeline::ani_timeline(const std::shared_ptr<ani_timeline>& parent, ui32 id, const glm::vec4& aabb, const glm::vec2& pivot, ui32 frames_count)
        : m_id(id)
        , m_aabb(aabb)
        , m_pivot(pivot)
        , m_scene_fps(0)
        , m_scene_width(0)
        , m_scene_height(0)
        , m_frames_count(frames_count)
        , m_current_texture_atlas(nullptr)
        , m_used_atlas_csf(1.f)
        , m_parent(parent)
        {
        }
        
        ani_timeline::~ani_timeline()
        {

        }
        
        void ani_timeline::push_texture_atlas(const std::shared_ptr<ani_texture_atlas>& atlas)
        {
            m_texture_atlases.push_back(atlas);
        }
        
        void ani_timeline::push_animation_mask(ui32 object_id, ui32 element_atlas_id_reference, e_ani_character_type character_type)
        {
            m_animation_masks[object_id] = std::make_tuple(element_atlas_id_reference, character_type);
        }
        
        void ani_timeline::push_animation_object(ui32 object_id, ui32 element_atlas_id_reference, e_ani_character_type character_type)
        {
            m_animation_objects[object_id] = std::make_tuple(element_atlas_id_reference, character_type);
        }
        
        void ani_timeline::push_animation_frame(const std::shared_ptr<ani_animation_frame>& frame)
        {
            m_animation_frames.push_back(frame);
        }
        
        void ani_timeline::push_animation_sequence(const std::string& name, i32 start, i32 end)
        {
            std::shared_ptr<ani_animation_sequence> sequence = std::make_shared<ani_animation_sequence>();
            sequence->m_name = name;
            sequence->m_start_frame = start;
            sequence->m_end_frame = end;
            m_animation_sequences[name] = sequence;
        }
        
        void ani_timeline::push_named_part(ui32 object_id_reference, const std::string& name)
        {
            m_named_parts.insert(std::pair<std::string, ui32>(name, object_id_reference));
        }
        
        void ani_timeline::push_external_object(const std::shared_ptr<ani_external_object>& external_object)
        {
            m_external_objects[external_object->get_object_id_reference()] = external_object;
        }
        
        void ani_timeline::set_scene_fps(ui32 value)
        {
            m_scene_fps = value;
        }
        
        void ani_timeline::set_scene_width(ui32 value)
        {
            m_scene_width = value;
        }
        
        void ani_timeline::set_scene_height(ui32 value)
        {
            m_scene_height = value;
        }
        
        void ani_timeline::set_scene_color(const glm::u8vec4& value)
        {
            m_scene_color = value;
        }
        
        const ani_animation_objects_t& ani_timeline::get_animation_objects() const
        {
            return m_animation_objects;
        }
        
        const ani_animation_masks_t& ani_timeline::get_animation_masks() const
        {
            return m_animation_masks;
        }
        
        const ani_animation_frames_t& ani_timeline::get_animation_frames() const
        {
            return m_animation_frames;
        }
        
        const ani_animation_sequences_t& ani_timeline::get_animation_sequences() const
        {
            return m_animation_sequences;
        }
        
        const ani_named_parts_t& ani_timeline::get_named_parts() const
        {
            return m_named_parts;
        }
        
        const ani_texture_atlases_t& ani_timeline::get_texture_atlases() const
        {
            return m_texture_atlases;
        }
        
        const ani_external_objects_t& ani_timeline::get_external_objects() const
        {
            return m_external_objects;
        }
        
        void ani_timeline::set_custom_properties(const ani_custom_properties_t& properties)
        {
            m_custom_properties = properties;
        }
        
        const ani_custom_properties_t& ani_timeline::get_custom_properties() const
        {
            return m_custom_properties;
        }
        
        const std::shared_ptr<ani_animation_sequence> ani_timeline::get_sequence(const std::string& name) const
        {
            ani_animation_sequences_t::const_iterator iterator = m_animation_sequences.find(name);
            
            if (iterator != m_animation_sequences.end())
            {
                return iterator->second;
            }
            return nullptr;
        }
        
        const std::shared_ptr<ani_animation_sequence> ani_timeline::get_sequence_by_last_frame(size_t frame) const
        {
            if (m_animation_sequences.empty())
            {
                return nullptr;
            }
            
            for (ani_animation_sequences_t::const_iterator iterator = m_animation_sequences.begin(), end = m_animation_sequences.end(); iterator != end; ++iterator)
            {
                if (iterator->second->m_end_frame == frame + 1)
                {
                    return iterator->second;
                }
            }
            return nullptr;
        }
        
        const std::shared_ptr<ani_animation_sequence> ani_timeline::get_sequence_by_first_frame(size_t frame) const
        {
            if (m_animation_sequences.empty())
            {
                return nullptr;
            }
            
            for (ani_animation_sequences_t::const_iterator iterator = m_animation_sequences.begin(), end = m_animation_sequences.end(); iterator != end; ++iterator)
            {
                if (iterator->second->m_start_frame == frame)
                {
                    return iterator->second;
                }
            }
            return nullptr;
        }
        
        std::shared_ptr<ani_texture_atlas> ani_timeline::get_texture_atlas()
        {
            return m_current_texture_atlas;
        }
        
        void ani_timeline::set_linkage_name(const std::string &linkage_name)
        {
            m_linkage_name = linkage_name;
        }
        
        void ani_timeline::set_base_class(const std::string &base_class)
        {
            m_base_class = base_class;
        }
        
        void ani_timeline::set_origin_class(const std::string &origin_class)
        {
            m_origin_class = origin_class;
        }
        
        uint32_t ani_timeline::get_frames_count() const
        {
            return m_frames_count;
        }
        
        const glm::vec4& ani_timeline::get_bounds() const
        {
            return m_aabb;
        }
        
        const glm::vec2& ani_timeline::get_pivot() const
        {
            return m_pivot;
        }
        
        const std::string& ani_timeline::get_linkage_name() const
        {
            return m_linkage_name;
        }
        
        const std::string& ani_timeline::get_base_class() const
        {
            return m_base_class;
        }
        
        const std::string& ani_timeline::get_origin_class() const
        {
            return m_origin_class;
        }
        
        std::shared_ptr<ani_timeline> ani_timeline::get_parent() const
        {
            return m_parent.lock();
        }
        
        void ani_timeline::assign_atlas(f32 desired_atlas_scale)
        {
            if (m_texture_atlases.empty())
            {
                m_current_texture_atlas = nullptr;
                m_used_atlas_csf = desired_atlas_scale;
                return;
            }
            choose_texture_atlas(desired_atlas_scale);
        }
        
        void ani_timeline::choose_texture_atlas(f32 desired_atlas_scale)
        {
            f32 atlas_scale = m_texture_atlases[0]->get_scale();
            m_current_texture_atlas = m_texture_atlases[0];
            const size_t count = m_texture_atlases.size();
            
            for (size_t i = 1; i < count; ++i)
            {
                f32 current_atlas_scale = m_texture_atlases[i]->get_scale();
                if (fabs(atlas_scale - desired_atlas_scale) > fabs(current_atlas_scale - desired_atlas_scale))
                {
                    m_current_texture_atlas = m_texture_atlases[i];
                    atlas_scale = current_atlas_scale;
                }
            }
            m_used_atlas_csf = atlas_scale;
        }
        
        f32 ani_timeline::used_atlas_scale() const
        {
            return m_used_atlas_csf;
        }
    }
}
