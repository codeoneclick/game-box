//
//  bullet.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 2/3/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "bullet.h"
#include "ces_box2d_body_component.h"
#include "ces_bullet_component.h"
#include "ces_action_component.h"
#include "scene_graph.h"
#include "scene_fabricator.h"
#include "anim_fabricator.h"
#include "animated_sprite.h"
#include "light_source.h"

#define k_bullet_min_width_scale .75f
#define k_bullet_max_width_scale 1.f
#define k_bullet_min_length_scale .1f
#define k_bullet_max_length_scale 1.f
#define k_bullet_light_min_radius 32.f
#define k_bullet_light_max_radius 256.f

namespace game
{
    bullet::bullet()
	{
		ces_bullet_component_shared_ptr bullet_component = std::make_shared<ces_bullet_component>();
		ces_entity::add_component(bullet_component);
        
        std::shared_ptr<gb::ces_action_component> action_component = std::make_shared<gb::ces_action_component>();
        action_component->set_update_callback(std::bind(&bullet::update, this,
                                                        std::placeholders::_1, std::placeholders::_2));
        bullet::add_component(action_component);
	}

	bullet::~bullet()
	{

	}

	void bullet::setup(const std::string& filename,
		const gb::scene_graph_shared_ptr& scene_graph,
		const gb::scene_fabricator_shared_ptr& scene_fabricator,
		const gb::anim::anim_fabricator_shared_ptr& anim_fabricator)
	{
		auto bullet = anim_fabricator->create_animated_sprite(filename, "bullet");
		bullet->tag = "bullet";
		bullet->goto_and_play(0);
		bullet->is_luminous = true;
		bullet->scale = glm::vec2(k_bullet_min_width_scale, k_bullet_min_length_scale);
		bullet->rotation = 180.f;
        bullet::add_child(bullet);
        m_bullet = bullet;

		auto light_source = scene_fabricator->create_light_source("light_01.xml");
		light_source->radius = k_bullet_light_max_radius;
		light_source->color = glm::vec4(1.f, 1.f, 0.f, 1.f);
		light_source->tag = "light_source";
		bullet::add_child(light_source);
        m_light_source = light_source;
    }
    
    void bullet::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        glm::vec2 current_scale = m_bullet.lock()->scale;
        if(current_scale.y < k_bullet_max_length_scale)
        {
            current_scale.x = glm::mix(current_scale.x, k_bullet_max_width_scale,
                                       deltatime * current_scale.x * 5.f);
            current_scale.y = glm::mix(current_scale.y, k_bullet_max_length_scale,
                                       deltatime * current_scale.y * 5.f);
            
            m_bullet.lock()->scale = current_scale;
        }
        
        if(!m_light_source.expired())
        {
            f32 current_radius = m_light_source.lock()->radius;
            if(current_radius > k_bullet_light_min_radius)
            {
                current_radius = glm::mix(current_radius, k_bullet_light_min_radius,
                                          deltatime * 100.f);
                m_light_source.lock()->radius = current_radius;
            }
            else
            {
                m_light_source.lock()->remove_from_parent();
            }
        }
    }
}
