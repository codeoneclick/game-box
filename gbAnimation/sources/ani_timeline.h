#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "tags_defines.h"

namespace gb
{
    namespace anim
    {
        class ani_texture_atlas;
        
        class ani_external_object
        {
        private:
            
            ui32 m_object_id_reference;
            std::string m_name;
            ani_custom_properties_t m_custom_properties;
            
        public:
            
            ani_external_object();
            ani_external_object(ui32 object_id_reference, const std::string& name);
            
            ui32 get_object_id_reference() const { return m_object_id_reference; }
            void set_object_id_reference(ui32 object_id_reference) { m_object_id_reference = object_id_reference; }
            
            const std::string& get_name() const { return m_name; }
            void set_name(const std::string& name) { m_name = name; }
            
            const ani_custom_properties_t& get_custom_properties() const { return m_custom_properties; }
            void set_custom_properties(const ani_custom_properties_t& custom_properties) { m_custom_properties = custom_properties; }
        };
        
        class ani_timeline
        {
        private:

            ani_texture_atlases_t m_texture_atlases;
            ani_animation_masks_t m_animation_masks;
            ani_animation_objects_t m_animation_objects;
            ani_animation_frames_t m_animation_frames;
            ani_animation_sequences_t m_animation_sequences;
            ani_named_parts_t m_named_parts;
            ani_external_objects_t m_external_objects;
            
            ui32 m_id;
            glm::vec4 m_aabb;
            glm::vec2 m_pivot;
            
            ui32 m_scene_fps;
            ui32 m_scene_width;
            ui32 m_scene_height;
            glm::u8vec4 m_scene_color;
            
            ui32 m_frames_count;
            f32 m_used_atlas_csf;
            
            std::string m_linkage_name;
            std::string m_base_class;
            std::string m_origin_class;
            
            std::shared_ptr<ani_texture_atlas> m_current_texture_atlas;
            std::weak_ptr<ani_timeline> m_parent;
            
            void choose_texture_atlas(f32 desired_atlas_scale);
            
        public:
            
            ani_timeline(const std::shared_ptr<ani_timeline>& parent, ui32 id, const glm::vec4& aabb, const glm::vec2& pivot, ui32 frames_count);
            virtual ~ani_timeline();
            
            void push_texture_atlas(const std::shared_ptr<ani_texture_atlas>& atlas);
            void push_animation_mask(ui32 object_id, ui32 element_atlas_id_reference, e_ani_character_type character_type);
            void push_animation_object(ui32 object_id, ui32 element_atlas_id_reference, e_ani_character_type character_type);
            void push_animation_frame(const std::shared_ptr<ani_animation_frame>& frame);
            void push_animation_sequence(const std::string& name_id, i32 start, i32 end);
            void push_named_part(ui32 object_id_reference, const std::string& name);
            void push_external_object(const std::shared_ptr<ani_external_object>& external_object);
            
            void set_scene_fps(ui32);
            void set_scene_width(ui32);
            void set_scene_height(ui32);
            void set_scene_color(const glm::u8vec4&);
            
            void set_linkage_name(const std::string& linkage_name);
            void set_base_class(const std::string& base_class);
            void set_origin_class(const std::string& origin_class);
            
            const ani_animation_objects_t& get_animation_objects() const;
            const ani_animation_masks_t& get_animation_masks() const;
            const ani_animation_frames_t& get_animation_frames() const;
            const ani_animation_sequences_t& get_animation_sequences() const;
            const ani_named_parts_t& get_named_parts() const;
            const ani_texture_atlases_t& get_texture_atlases() const;
            const ani_external_objects_t& get_external_objects() const;
            ui32 get_frames_count() const;
            
            const glm::vec4& get_bounds() const;
            const glm::vec2& get_pivot() const;
            
            const std::string& get_linkage_name() const;
            const std::string& get_base_class() const;
            const std::string& get_origin_class() const;
            
            const std::shared_ptr<ani_animation_sequence> get_sequence(const std::string& name) const;
            const std::shared_ptr<ani_animation_sequence> get_sequence_by_last_frame(size_t frame) const;
            const std::shared_ptr<ani_animation_sequence> get_sequence_by_first_frame(size_t frame) const;
            
            std::shared_ptr<ani_timeline> get_parent() const;
            
            std::shared_ptr<ani_texture_atlas> get_texture_atlas();
            f32 used_atlas_scale() const;
            
            void assign_atlas(f32 desired_atlas_scale);
            
        public:
            
            void set_custom_properties(const ani_custom_properties_t& properties);
            const ani_custom_properties_t& get_custom_properties() const;
            
        private:
            
            ani_custom_properties_t m_custom_properties;
            
        public:
            void append_user_data(const std::string& key, const std::shared_ptr<ani_any_i>& value) { m_user_data[key] = value; }
            
            template<class T> T get_user_data(const std::string& key)
            {
                ani_custom_data_t::const_iterator iterator = m_user_data.find(key);
                if (iterator == m_user_data.end())
                {
                    return T();
                }
                return reinterpret_cast<ani_any<T>>(iterator->second)->data;
            }
            
        private:    
            typedef std::unordered_map<std::string, std::shared_ptr<ani_any_i>> ani_custom_data_t;
            ani_custom_data_t m_user_data;
        };
    };
};
