//
//  information_bubble_controller.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 3/20/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "information_bubble_controller.h"
#include "ces_transformation_component.h"
#include "ces_action_component.h"
#include "information_bubble.h"

namespace game
{
    information_bubble_controller::information_bubble_controller(const gb::game_object_shared_ptr& layer,
                                                                 const gb::scene_graph_shared_ptr& scene_graph,
                                                                 const gb::scene_fabricator_shared_ptr& scene_fabricator) :
    m_layer(layer),
    m_scene_graph(scene_graph),
    m_scene_fabricator(scene_fabricator)
    {
        std::shared_ptr<gb::ces_action_component> action_component = std::make_shared<gb::ces_action_component>();
        action_component->set_update_callback(std::bind(&information_bubble_controller::update, this,
                                                        std::placeholders::_1, std::placeholders::_2));
        information_bubble_controller::add_component(action_component);
    }
    
    information_bubble_controller::~information_bubble_controller()
    {
        
    }
    
    void information_bubble_controller::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        m_bubbles.remove_if([=](const information_bubble_weak_ptr& bubble_weak) {
            if(!bubble_weak.expired())
            {
                auto bubble = bubble_weak.lock();
                if(!bubble->is_expired)
                {
                    return false;
                }
                else
                {
                    bubble->remove_from_parent();
                    return true;
                }
            }
            else
            {
                assert(false);
            }
            return true;
        });
    }
    
    void information_bubble_controller::push_bubble(const std::string& message, const glm::u8vec4& color, const glm::vec2& position)
    {
        information_bubble_shared_ptr bubble = std::make_shared<game::information_bubble>();
        bubble->setup("information_bubble_01.xml",
                      m_scene_graph.lock(),
                      m_scene_fabricator.lock(),
                      message,
                      color);
        bubble->position = position;
        m_layer.lock()->add_child(bubble);
        m_bubbles.push_back(bubble);
    }
}
