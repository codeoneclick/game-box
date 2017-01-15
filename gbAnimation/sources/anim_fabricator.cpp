//
//  anim_fabricator.cpp
//  gbAnimation
//
//  Created by serhii serhiiv on 11/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "anim_fabricator.h"
#include "scene_fabricator.h"
#include "anim_configuration_accessor.h"
#include "animated_sprite_configuration.h"
#include "resource_accessor.h"
#include "animated_sprite.h"
#include "animation.h"
#include "animation_loading_operation.h"
#include "ani_asset_metadata.h"
#include "ani_timeline.h"
#include "ani_texture_atlas.h"
#include "ani_texture_atlas_element.h"
#include "texture.h"
#include "texture_loading_operation.h"
#include "ces_ani_timeline_component.h"
#include "ces_ani_frame_component.h"
#include "ces_material_component.h"

namespace gb
{
    namespace anim
    {
        anim_fabricator::anim_fabricator(const scene_fabricator_shared_ptr& fabricator) :
        m_fabricator(fabricator)
        {
            m_anim_configuration_accessor = std::make_shared<anim_configuration_accessor>();
        }
        
        anim_fabricator::~anim_fabricator()
        {
            
        }
        
        void anim_fabricator::create_animated_objects_from_timeline(const std::shared_ptr<animated_sprite>& animated_object,
                                                                    const std::shared_ptr<ani_asset_metadata>& metadata,
                                                                    const std::shared_ptr<ani_timeline>& timeline)
        {
            ani_animation_objects_t animation_objects = timeline->get_animation_objects();
            
            for (const auto& animation_object : animation_objects)
            {
                e_ani_character_type character_type = std::get<1>(animation_object.second);
                ui32 reference = std::get<0>(animation_object.second);
                ui32 object_id = animation_object.first;
                
                anim_fabricator::create_animated_subobject(animated_object, timeline, metadata, object_id,
                                                           character_type, reference, false);
            }
        }
        
        void anim_fabricator::create_animated_subobject(const std::shared_ptr<animated_sprite>& animated_object,
                                                        const std::shared_ptr<ani_timeline>& timeline,
                                                        const std::shared_ptr<ani_asset_metadata>& metadata,
                                                        ui32 object_id, e_ani_character_type character_type, ui32 reference, bool is_mask)
        {
            std::shared_ptr<gb::anim::animated_sprite> animated_subobject = nullptr;
            if (character_type == e_ani_character_type::e_external)
            {
                assert(false);
            }
            else if (character_type == e_ani_character_type::e_timeline)
            {
                ani_timelines_t& timelines = metadata->get_timelines();
                const auto current_timeline = timelines.find(reference);
                
                ani_named_parts_t named_parts = timeline->get_named_parts();
                const ani_named_parts_t::const_iterator iterator = std::find_if(named_parts.begin(), named_parts.end(), [object_id] (const std::pair<std::string, ui32>& value){
                    return value.second == object_id;
                });
                std::string object_name = iterator != named_parts.end() ? iterator->first : current_timeline->second->get_linkage_name();
                animated_subobject = std::make_shared<gb::anim::animated_sprite>();
                animated_subobject->tag = object_name;
                animated_object->add_child(animated_subobject);
                
                auto timeline_component = std::make_shared<ces_ani_timeline_component>();
                timeline_component->timeline = current_timeline->second;
                timeline_component->object_id_reference = object_id;
                animated_subobject->add_component(timeline_component);
                animated_subobject->visible = false;
                
                anim_fabricator::create_animated_objects_from_timeline(animated_subobject, metadata, current_timeline->second);
                animated_subobject->set_custom_texcoord(glm::vec4(0.f));
            }
            else if (character_type == e_ani_character_type::e_textfield)
            {
                assert(false);
            }
            else if (character_type == e_ani_character_type::e_texture)
            {
                ani_named_parts_t named_parts = timeline->get_named_parts();
                const ani_named_parts_t::const_iterator iterator = std::find_if(named_parts.begin(), named_parts.end(), [object_id] (const std::pair<std::string, ui32>& value){
                    return value.second == object_id;
                });
                std::string object_name = iterator != named_parts.end() ? iterator->first : timeline->get_linkage_name();
                animated_subobject = std::make_shared<gb::anim::animated_sprite>();
                animated_subobject->tag = object_name;
                animated_object->add_child(animated_subobject);
                
                auto frame_component = std::make_shared<ces_ani_frame_component>();
                frame_component->object_id_reference = object_id;
                animated_subobject->add_component(frame_component);
                animated_subobject->visible = false;
                
                timeline->assign_atlas(1.f);
                std::shared_ptr<ani_texture_atlas> atlas = timeline->get_texture_atlas();
                const ani_texture_atlas::atlas_metadatas_t& atlas_metadatas = atlas->get_atlas_metadatas();
                assert(atlas_metadatas.size() > 0);
                ani_texture_atlas::ani_atlas_metadata atlas_metadata = atlas_metadatas.at(0);
                assert(atlas_metadata.m_sources.size() == 1);
                std::string atlas_resource_filename = atlas_metadata.m_sources.at(0).m_source;
                
                const ani_texture_atlas::elements_t elements = atlas->get_elements();
                const auto& element = elements.find(reference);
                if(element != elements.end())
                {
                    texture_shared_ptr texture = m_fabricator->get_resource_accessor()->get_resource<gb::texture, texture_loading_operation>(atlas_resource_filename, true);
                    std::shared_ptr<ani_texture_atlas_element> atlas_element = element->second;
                    glm::vec4 bounds = atlas_element->m_bounds;
                    
                    animated_subobject->size = glm::vec2(bounds.z, bounds.w);
                    animated_subobject->pivot = glm::vec2(atlas_element->m_pivot.x / bounds.z,
                                                          atlas_element->m_pivot.y / bounds.w);
                    animated_subobject->set_custom_texcoord(glm::vec4(bounds.x / static_cast<f32>(texture->get_width()),
                                                                      1.f - bounds.y / static_cast<f32>(texture->get_height()),
                                                                      (bounds.x + bounds.z) / static_cast<f32>(texture->get_width()),
                                                                      1.f - (bounds.y + bounds.w) / static_cast<f32>(texture->get_height())));
                    
                    frame_component->is_cw90 = atlas_element->m_rotation != ani_texture_atlas_element::e_rotation::none;
                }
                else
                {
                    assert(false);
                }
            }
        }
        
        std::shared_ptr<animated_sprite> anim_fabricator::create_animated_sprite(const std::string& filename, const std::string& timeline_name)
        {
            std::shared_ptr<gb::anim::animated_sprite> animated_sprite = nullptr;
            std::shared_ptr<gb::animated_sprite_configuration> animated_sprite_configuration =
            std::static_pointer_cast<gb::animated_sprite_configuration>(m_anim_configuration_accessor->get_animated_sprite_configuration(filename));
            assert(animated_sprite_configuration);
            if(animated_sprite_configuration)
            {
                std::shared_ptr<gb::anim::animation> animation = m_fabricator->get_resource_accessor()->get_resource<gb::anim::animation,
                gb::anim::animation_loading_operation>(animated_sprite_configuration->get_animation_filename(), true);
                
                std::shared_ptr<ani_asset_metadata> metadata = animation->get_metadata();
                std::shared_ptr<ani_timeline> timeline = metadata->get_timeline_by_name(timeline_name);
                
                animated_sprite = std::make_shared<gb::anim::animated_sprite>();
                
                auto timeline_component = std::make_shared<ces_ani_timeline_component>();
                timeline_component->timeline = timeline;
                animated_sprite->add_component(timeline_component);
                
                anim_fabricator::create_animated_objects_from_timeline(animated_sprite, metadata, timeline);
                
                anim_fabricator::apply_materials_recursively(animated_sprite, animated_sprite_configuration->get_materials_configurations());
            }
            
            return animated_sprite;
        }
        
        void anim_fabricator::apply_materials_recursively(const ces_entity_shared_ptr& entity,
                                                          const std::vector<std::shared_ptr<configuration>>& configurations)
        {
            m_fabricator->add_materials(entity, configurations);
            auto material_component = entity->get_component<ces_material_component>();
            material_component->set_is_batching(true);
            
            std::list<ces_entity_shared_ptr> children = entity->children;
            for(const auto& child : children)
            {
                anim_fabricator::apply_materials_recursively(child, configurations);
            }
        }
    };
};
