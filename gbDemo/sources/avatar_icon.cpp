//
//  avatar_icon.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 6/16/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "avatar_icon.h"
#include "scene_graph.h"
#include "scene_fabricator.h"
#include "anim_fabricator.h"
#include "animated_sprite.h"
#include "ces_transformation_2d_component.h"

namespace game
{
    const std::string avatar_icon::k_background_entity_filename = "ability.button.xml";
    const f32 avatar_icon::k_background_size = 80.f;
    
    avatar_icon::avatar_icon()
    {

    }
    
    avatar_icon::~avatar_icon()
    {
        
    }
    
    void avatar_icon::setup_components()
    {
        auto transformation_component = ces_entity::get_component<gb::ces_transformation_2d_component>();
        transformation_component->set_is_in_camera_space(false);
    }
    
    void avatar_icon::setup(const std::pair<gb::sprite_shared_ptr, gb::shape_3d_shared_ptr>& character_linkage,
                            const gb::scene_graph_shared_ptr& scene_graph,
                            const gb::scene_fabricator_shared_ptr& scene_fabricator,
                            const gb::anim::anim_fabricator_shared_ptr& anim_fabricator)
    {
        auto background = scene_fabricator->create_sprite(k_background_entity_filename);
        background->tag = k_background_entity_filename;
        background->size = glm::vec2(k_background_size);
        background->color = glm::u8vec4(0, 0, 0, 255);
        avatar_icon::add_child(background);
        
        auto transformation_component = background->get_component<gb::ces_transformation_2d_component>();
        transformation_component->set_is_in_camera_space(false);
        
        avatar_icon::set_avatar(character_linkage);
    }
    
    void avatar_icon::set_avatar(const std::pair<gb::sprite_shared_ptr, gb::shape_3d_shared_ptr>& character_linkage)
    {
        if(!m_current_avatar.expired())
        {
            avatar_icon::remove_child(m_current_avatar.lock());
        }
        auto transformation_component = character_linkage.first->get_component<gb::ces_transformation_2d_component>();
        transformation_component->set_is_in_camera_space(false);
        m_shape_3d_linkage = character_linkage.second;
        m_current_avatar = character_linkage.first;
        character_linkage.first->position = glm::vec2(40.f, 70.f);
        avatar_icon::add_child(character_linkage.first);
    }
    
    void avatar_icon::play_animation(const std::string& animation_name, bool is_looped)
    {
        
    }
}

