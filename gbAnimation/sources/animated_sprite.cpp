//
//  animated_sprite.cpp
//  gbAnimation
//
//  Created by serhii serhiiv on 11/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "animated_sprite.h"
#include "ces_ani_animation_system.h"
#include "ces_ani_main_timeline_animation_component.h"
#include "ces_ani_timeline_animation_component.h"
#include "ces_ani_image_animation_component.h"
#include "ani_timeline.h"

namespace gb
{
    namespace anim
    {
        animated_sprite::animated_sprite()
        {
            
        }
        
        animated_sprite::~animated_sprite()
        {
            
        }
        
        std::shared_ptr<animated_sprite> animated_sprite::get_named_part_recursively(const ces_entity_shared_ptr& entity,
                                                                                     const std::string& name,
                                                                                     i32& object_id_refernce)
        {
            std::shared_ptr<animated_sprite> named_part = nullptr;
            if(object_id_refernce == -1)
            {
                std::shared_ptr<ces_ani_main_timeline_animation_component> main_timeline_animation_component =
                entity->get_component<ces_ani_main_timeline_animation_component>();
                std::shared_ptr<ces_ani_timeline_animation_component> timeline_animation_component =
                entity->get_component<ces_ani_timeline_animation_component>();
                
                std::shared_ptr<ani_timeline> timeline = nullptr;
                if(main_timeline_animation_component)
                {
                    timeline = main_timeline_animation_component->timeline;
                }
                else if(timeline_animation_component)
                {
                    timeline = timeline_animation_component->timeline;
                }
                if(timeline)
                {
                    std::shared_ptr<ani_timeline> timeline = main_timeline_animation_component->timeline;
                    ani_named_parts_t named_parts = timeline->get_named_parts();
                    const auto& iterator = named_parts.find(name);
                    if(iterator != named_parts.end())
                    {
                        object_id_refernce = iterator->second;
                    }
                }
            }
            else
            {
                std::shared_ptr<ces_ani_main_timeline_animation_component> main_timeline_animation_component =
                entity->get_component<ces_ani_main_timeline_animation_component>();
                std::shared_ptr<ces_ani_timeline_animation_component> timeline_animation_component =
                entity->get_component<ces_ani_timeline_animation_component>();
                std::shared_ptr<ces_ani_image_animation_component> image_animation_component =
                entity->get_component<ces_ani_image_animation_component>();
                
                if(main_timeline_animation_component && main_timeline_animation_component->object_id_reference == object_id_refernce)
                {
                    named_part = std::static_pointer_cast<animated_sprite>(entity);
                }
                else if(timeline_animation_component && timeline_animation_component->object_id_reference == object_id_refernce)
                {
                    named_part = std::static_pointer_cast<animated_sprite>(entity);
                }
                else if(image_animation_component && image_animation_component->object_id_reference == object_id_refernce)
                {
                    named_part = std::static_pointer_cast<animated_sprite>(entity);
                }
            }
            
            if(!named_part)
            {
                std::list<ces_entity_shared_ptr> children = entity->children;
                for(const auto& child : children)
                {
                    named_part = animated_sprite::get_named_part_recursively(child, name, object_id_refernce);
                    if(named_part)
                    {
                        break;
                    }
                }
            }
            return named_part;
        }
        
        std::weak_ptr<animated_sprite> animated_sprite::get_named_part(const std::string& name)
        {
            i32 object_id_refernce = -1;
            std::shared_ptr<animated_sprite> named_part = animated_sprite::get_named_part_recursively(shared_from_this(), name, object_id_refernce);
            return named_part;
        }
        
        void animated_sprite::goto_and_stop(i32 frame_index)
        {
            ces_ani_animation_system::goto_and_stop(shared_from_this(), frame_index);
        }
    }
}