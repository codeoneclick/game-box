//
//  characters_3d_controller.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 5/19/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "characters_3d_controller.h"
#include "camera_3d.h"
#include "scene_graph.h"
#include "scene_fabricator.h"
#include "shape_3d.h"
#include "sprite.h"

namespace game
{
    characters_3d_controller::characters_3d_controller(const gb::scene_graph_shared_ptr& scene_graph,
                                                       const gb::scene_fabricator_shared_ptr& scene_fabricator) :
    m_scene_graph(scene_graph),
    m_scene_fabricator(scene_fabricator)
    {
        m_camera_3d = std::make_shared<gb::camera_3d>(90.f, .1f, 1024.f,
                                                      glm::ivec4(0.f,
                                                                 0.f,
                                                                 16.f,
                                                                 16.f), false);
        m_camera_3d->set_distance_to_look_at(glm::vec3(8.f, 0.f, 0.f));
        scene_graph->set_camera_3d(m_camera_3d);
    }
    
    characters_3d_controller::~characters_3d_controller()
    {
        
    }
    
    std::pair<gb::sprite_shared_ptr, gb::shape_3d_shared_ptr> characters_3d_controller::create_character(const std::string& character_sprite_filename,
                                                                                                         const std::string& character_shape_3d_filename,
                                                                                                         const glm::vec2& size)
    {
        auto shape_3d = m_scene_fabricator.lock()->create_shape_3d(character_shape_3d_filename);
        m_scene_graph.lock()->add_child(shape_3d);
        
        f32 scale = .1f;
        
        shape_3d->position = glm::vec3(0.f, (16.f / 4.f) * .5f, (16.f / 4.f) * .5f);
        shape_3d->scale = glm::vec3(scale);
        shape_3d->play_animation("idle");
        shape_3d->rotation = glm::vec3(0.f, -90.f, 90.f);
        
        auto sprite = m_scene_fabricator.lock()->create_sprite(character_sprite_filename);
        sprite->size = size;
        sprite->pivot = glm::vec2(.5f);
        sprite->set_custom_texcoord(glm::vec4(0.f, .75f, .25f, 1.f));
        sprite->rotation = 180.f;
        sprite->tag = "body";
        
        auto characters_linkage = std::make_pair(sprite, shape_3d);
        m_characters_3d_container.insert(characters_linkage);
        return characters_linkage;
    }
    
    void characters_3d_controller::remove_character(const gb::ces_entity_shared_ptr& character)
    {
        auto characters_linkage = m_characters_3d_container.find(character);
        if(characters_linkage != m_characters_3d_container.end())
        {
            m_characters_3d_container.erase(characters_linkage);
        }
    }
}
