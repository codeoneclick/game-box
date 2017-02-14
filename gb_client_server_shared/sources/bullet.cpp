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
#include "scene_graph.h"
#include "scene_fabricator.h"
#include "anim_fabricator.h"
#include "animated_sprite.h"
#include "light_source.h"

namespace ns
{
	bullet::bullet()
	{
		ces_bullet_component_shared_ptr bullet_component = std::make_shared<ces_bullet_component>();
		ces_entity::add_component(bullet_component);
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

		auto light_source = scene_fabricator->create_light_source("light_01.xml");
		light_source->radius = 64.f;
		light_source->color = glm::vec4(1.f, 1.f, 1.f, 1.f);
		light_source->tag = "light_source";

		bullet::add_child(bullet);
		bullet::add_child(light_source);
	}
}