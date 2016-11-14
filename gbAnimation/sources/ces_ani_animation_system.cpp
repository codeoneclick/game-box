//
//  ces_ani_animation_system.cpp
//  gbAnimation
//
//  Created by serhii serhiiv on 11/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_ani_animation_system.h"
#include "ces_ani_animation_component.h"

namespace gb
{
    namespace anim
    {
        ces_ani_animation_system::ces_ani_animation_system()
        {
            
        }
        
        ces_ani_animation_system::~ces_ani_animation_system()
        {
            
        }
        
        void ces_ani_animation_system::on_feed_start(f32 deltatime)
        {
            
        }
        
        void ces_ani_animation_system::on_feed(const ces_entity_shared_ptr& entity, f32 deltatime)
        {
            ces_ani_animation_system::update_recursively(entity, deltatime);
        }
        
        void ces_ani_animation_system::on_feed_end(f32 deltatime)
        {
            
        }
        
        void ces_ani_animation_system::update_recursively(const ces_entity_shared_ptr& entity, f32 deltatime)
        {
            std::shared_ptr<ces_ani_animation_component> animation_component = entity->get_component<ces_ani_animation_component>();
            
            if(animation_component)
            {
                
            }
            
            std::list<ces_entity_shared_ptr> children = entity->children;
            for(const auto& child : children)
            {
                ces_ani_animation_system::update_recursively(child, deltatime);
            }
        }
    }
}
