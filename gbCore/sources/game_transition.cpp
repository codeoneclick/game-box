//
//  game_transition.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "game_transition.h"
#include "ces_systems_feeder.h"
#include "game_loop.h"
#include "configuration_accessor.h"
#include "resource_accessor.h"
#include "ces_render_system.h"
#include "ces_animation_3d_system.h"
#include "ces_deferred_lighting_system.h"
#include "ces_touch_system.h"
#include "ces_actions_system.h"
#include "ces_box2d_system.h"
#include "ces_omni_deferred_light_source_3d_system.h"
#include "transition_configuration.h"
#include "render_pipeline.h"
#include "graphics_context.h"
#include "render_technique_ws.h"
#include "render_technique_ss.h"
#include "render_technique_main.h"
#include "material.h"
#include "scene_graph.h"
#include "scene_fabricator.h"

namespace gb
{
    game_transition::game_transition(const std::string& guid, bool is_offscreen) :
    m_guid(guid),
    m_offscreen(is_offscreen),

#if USED_GRAPHICS_API != NO_GRAPHICS_API

    m_input_context(nullptr),
    m_screen_width(0),
    m_screen_height(0),

#endif

	m_scene(nullptr)
    {
        m_system_feeder = std::make_shared<ces_systems_feeder>();
    }
    
    game_transition::~game_transition()
    {
        
    }
    
    std::string game_transition::get_guid() const
    {
        return m_guid;
    }
    
    void game_transition::on_activated(const graphics_context_shared_ptr& graphics_context,
                                       const input_context_shared_ptr& input_context,
                                       const configuration_accessor_shared_ptr& configurations_accessor,
                                       const resource_accessor_shared_ptr& resource_accessor)
    {
		std::shared_ptr<transition_configuration> transition_configuration =
			std::static_pointer_cast<gb::transition_configuration>(configurations_accessor->get_transition_configuration(m_guid));
		assert(transition_configuration);

        m_configuration_accessor = configurations_accessor;
        m_resource_accessor = resource_accessor;
        
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        m_input_context = input_context;
        
        m_screen_width = graphics_context->get_width();
        m_screen_height = graphics_context->get_height();
        
        std::shared_ptr<ces_render_system> render_system = std::make_shared<ces_render_system>(graphics_context, m_offscreen);
        std::shared_ptr<render_pipeline> render_pipeline = render_system->get_render_pipeline();

        for(const auto& iterator : transition_configuration->get_ws_technique_configuration())
        {
            std::shared_ptr<ws_technique_configuration> ws_technique_configuration = std::static_pointer_cast<gb::ws_technique_configuration>(iterator);
            
            std::shared_ptr<render_technique_ws> render_technique_ws = render_technique_ws::construct(ws_technique_configuration);
            glm::vec4 color = glm::vec4(ws_technique_configuration->get_clear_color_r(),
                                        ws_technique_configuration->get_clear_color_g(),
                                        ws_technique_configuration->get_clear_color_b(),
                                        ws_technique_configuration->get_clear_color_a());
            render_technique_ws->set_clear_color(color);
            render_pipeline->add_ws_render_technique(ws_technique_configuration->get_guid(), ws_technique_configuration->get_index(), render_technique_ws);
            
            resource_accessor->add_custom_resource(ws_technique_configuration->get_guid() + ".color",
                                                   render_technique_ws->get_color_attachment_texture());
            resource_accessor->add_custom_resource(ws_technique_configuration->get_guid() + ".depth",
                                                   render_technique_ws->get_depth_attachment_texture());
            
#if USED_GRAPHICS_API == METAL_API
            
            const auto color_attachments_texture = render_technique_ws->get_color_attachments_texture();
            for (auto color_attachment_texture_it : color_attachments_texture)
            {
                resource_accessor->add_custom_resource(color_attachment_texture_it->get_guid(),
                                                       color_attachment_texture_it);
                
            }
            
#endif
            
        }
        
        for(const auto& iterator : transition_configuration->get_ss_technique_configuration())
        {
            std::shared_ptr<ss_technique_configuration> ss_technique_configuration = std::static_pointer_cast<gb::ss_technique_configuration>(iterator);
            assert(ss_technique_configuration != nullptr);
            std::shared_ptr<material_configuration> material_configuration = ss_technique_configuration->get_material_configuration();
            assert(material_configuration);
            
            std::shared_ptr<material> material = material::construct(material_configuration);
            gb::material::set_shader(material, material_configuration, resource_accessor);
            gb::material::set_textures(material, material_configuration, resource_accessor);
            
            std::shared_ptr<render_technique_ss> render_technique_ss = render_technique_ss::construct(ss_technique_configuration, material);
            render_pipeline->add_ss_render_technique(ss_technique_configuration->get_guid(), render_technique_ss);
            
            resource_accessor->add_custom_resource(ss_technique_configuration->get_guid() + ".color",
                                                   render_technique_ss->get_color_attachment_texture());
            
#if USED_GRAPHICS_API == METAL_API
            
            const auto color_attachments_texture = render_technique_ss->get_color_attachments_texture();
            for (auto color_attachment_texture_it : color_attachments_texture)
            {
                resource_accessor->add_custom_resource(color_attachment_texture_it->get_guid(),
                                                       color_attachment_texture_it);
                
            }
            
#endif
            
        }
        
        if(!m_offscreen)
        {
            std::shared_ptr<output_technique_configuration> main_technique_configuration = transition_configuration->get_output_technique_configuration();
            assert(main_technique_configuration);
            std::shared_ptr<material_configuration> material_configuration = main_technique_configuration->get_material_configuration();
            assert(material_configuration);
            
            std::shared_ptr<material> material =  material::construct(material_configuration);
            gb::material::set_shader(material, material_configuration, resource_accessor);
            gb::material::set_textures(material, material_configuration, resource_accessor);
            
            render_pipeline->create_main_render_technique(main_technique_configuration->get_guid(), material);
        }
        
        m_system_feeder->add_system(render_system);

		// auto deferred_lighting_system = std::make_shared<ces_deferred_lighting_system>();
		// deferred_lighting_system->set_order(4);
		// m_system_feeder->add_system(deferred_lighting_system);
        
        auto omni_deferred_light_source_3d_system = std::make_shared<ces_omni_deferred_light_source_3d_system>();
        omni_deferred_light_source_3d_system->set_order(4);
        m_system_feeder->add_system(omni_deferred_light_source_3d_system);
        
		auto touch_system = std::make_shared<ces_touch_system>();
		touch_system->set_order(0);
		m_system_feeder->add_system(touch_system);
		m_input_context->add_listener(touch_system);
        
#endif
        
		auto actions_system = std::make_shared<ces_actions_system>();
        m_system_feeder->add_system(actions_system);
        
        auto animation_3d_system = std::make_shared<ces_animation_3d_system>();
        m_system_feeder->add_system(animation_3d_system);
        
		auto box2d_system = std::make_shared<ces_box2d_system>();
		box2d_system->set_order(2);
        m_system_feeder->add_system(box2d_system);
        
        add_listener_to_game_loop(m_system_feeder);
        
        create_scene();
        
        m_system_feeder->set_root(m_scene);
        m_scene->set_systems_feeder(m_system_feeder);
        
        m_scene->create();
    }
    
    void game_transition::on_deactivated()
    {
        remove_listener_from_game_loop(m_system_feeder);
        
        destroy_scene();
        
        m_system_feeder->cleanup();
    }
    
    void game_transition::set_fabricator(const scene_fabricator_shared_ptr &fabricator)
    {
        if(m_scene)
        {
            m_scene->set_fabricator(fabricator);
            m_scene->get_fabricator()->set_configuration_accessor(m_configuration_accessor);
            m_scene->get_fabricator()->set_resource_accessor(m_resource_accessor);
        }
        else
        {
            assert(false);
        }
    }
    
    void game_transition::add_system(const ces_base_system_shared_ptr& system)
    {
        m_system_feeder->add_system(system);
    }
    
    ces_base_system_shared_ptr game_transition::get_system(i32 guid) const
    {
        return m_system_feeder->get_system(guid);
    }

#if USED_GRAPHICS_API != NO_GRAPHICS_API
    
    input_context_shared_ptr game_transition::get_input_context() const
    {
        return m_input_context;
    }
    
    i32 game_transition::get_screen_width() const
    {
        return m_screen_width;
    }
    
    i32 game_transition::get_screen_height() const
    {
        return m_screen_height;
    }

#endif
    
    void game_transition::set_external_commands(const game_commands_container_shared_ptr& commands)
    {
        if(m_scene)
        {
            m_scene->set_external_commands(commands);
        }
        else
        {
            assert(false);
        }
    }

	configuration_accessor_shared_ptr game_transition::get_configuration_accessor() const 
	{
		return m_configuration_accessor;
	}

	resource_accessor_shared_ptr game_transition::get_resource_accessor() const 
	{
		return m_resource_accessor;
	}
}
